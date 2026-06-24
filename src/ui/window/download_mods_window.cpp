// ReSharper disable CppDFAMemoryLeak
#include "download_mods_window.h"

#include "core/application.h"
#include "core/logger.h"
#include "ui/delegate/provider/provider_mod_delegate.h"
#include "util/lib/qt.h"

#include <QHBoxLayout>
#include <QLineEdit>
#include <QListView>
#include <QScrollBar>
#include <QTimer>

namespace Actinium
{
    DownloadModsWindow::DownloadModsWindow(Instance* instance, QWidget* parent)
        : BaseWindow(parent)
        , m_instance(instance)
        , m_current_page(0)
        , m_waiting_for_response(false)
        , m_end_of_list(false)
        , m_ready_to_scroll_search(true)
    {
        setObjectName("DownloadModsWindow");
        setWindowTitle("Download Mods");
        resize(828, 465);

        m_model = new ProviderModModel(this);

        const auto delegate = new ProviderModDelegate(this);

        const auto main_layout = new QVBoxLayout();
        const auto layout = new QHBoxLayout();

        m_view = new QListView(this);
        m_view->setObjectName("ModsListView");
        m_view->setModel(m_model);
        m_view->setItemDelegate(delegate);
        m_view->setResizeMode(QListView::Adjust);
        m_view->setUniformItemSizes(false);
        m_view->setAlternatingRowColors(true);
        m_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        m_view->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

        connect(m_view->verticalScrollBar(), &QScrollBar::valueChanged, this, &DownloadModsWindow::OnScrollBarChanged);
        connect(m_view->selectionModel(), &QItemSelectionModel::selectionChanged, this,
            &DownloadModsWindow::OnSelectionChanged);

        m_mod_description = new QTextBrowser(this);
        m_mod_description->setObjectName("ModDescription");
        m_mod_description->setReadOnly(true);
        m_mod_description->setOpenExternalLinks(true);

        m_download_button = new QPushButton("Download", this);
        m_download_button->setObjectName("DownloadButton");
        m_download_button->setEnabled(false);

        connect(m_download_button, &QPushButton::clicked, this, &DownloadModsWindow::OnClickDownload);

        layout->addWidget(m_view);
        layout->addWidget(m_mod_description);
        layout->setContentsMargins(0, 0, 0, 0);

        m_search_field = new QLineEdit();
        m_search_field->setObjectName("SearchField");

        m_search_field_timer = new QTimer(this);
        m_search_field_timer->setObjectName("SearchFieldTimer");
        m_search_field_timer->setSingleShot(true);

        connect(m_search_field, &QLineEdit::textChanged, this, &DownloadModsWindow::OnSearchFieldChanged);
        connect(m_search_field_timer, &QTimer::timeout, this, &DownloadModsWindow::OnSearchFieldTimerTimeout);

        main_layout->addWidget(m_search_field, 0, Qt::AlignmentFlag::AlignLeft | Qt::AlignmentFlag::AlignVCenter);
        main_layout->addLayout(layout);
        main_layout->addWidget(m_download_button, 0, Qt::AlignmentFlag::AlignRight | Qt::AlignmentFlag::AlignVCenter);
        main_layout->setContentsMargins(0, 0, 0, 0);

        const auto& providers = m_instance->GetGame()->providers;

        if (!providers.empty())
        {
            m_current_provider = providers.at(0);
            Search();
        }

        const auto central_widget = new QWidget(this);
        central_widget->setLayout(main_layout);
        setCentralWidget(central_widget);
    }

    DownloadModsWindow::~DownloadModsWindow()
    {
        Logger::Debug("ui::download_mods_window", "Destructor called");
    }

    void DownloadModsWindow::OnSelectionChanged(
        const QItemSelection& selected, [[maybe_unused]] const QItemSelection& deselected)
    {
        const auto selected_indexes = selected.indexes();

        if (selected_indexes.empty())
        {
            m_mod_description->setPlainText("");
            return;
        }

        const auto selected_index = selected_indexes.at(0);
        const auto mod_id = static_cast<uint32_t>(selected_index.internalId());

        if (m_mod_cache.contains(mod_id))
        {
            const auto& mod_info = m_mod_cache.at(mod_id);

            m_mod_description->setHtml(QString::fromStdString(mod_info.description));
            m_download_button->setEnabled(true);

            return;
        }

        m_mod_description->setPlainText("");
        m_current_provider->GetMod(mod_id,
            QT::QueuedCallback(this,
                [](DownloadModsWindow* self, const Provider::ModInfo& mod_info)
                {
                    self->m_mod_cache.insert({mod_info.id, mod_info});
                    self->m_mod_description->setHtml(QString::fromStdString(mod_info.description));
                    self->m_download_button->setEnabled(true);
                }));
    }

    void DownloadModsWindow::OnScrollBarChanged(const int value)
    {
        const auto maximum = m_view->verticalScrollBar()->maximum();

        if (maximum - value <= 10)
        {
            m_current_page += 1;
            Search();
        }
    }

    void DownloadModsWindow::OnSearchFieldChanged([[maybe_unused]] const QString& text) const
    {
        m_search_field_timer->start(300);
    }

    void DownloadModsWindow::OnSearchFieldTimerTimeout()
    {
        m_ready_to_scroll_search = true;
        m_end_of_list = false;
        m_waiting_for_response = false;
        m_current_page = 0;
        m_model->ClearResponse();

        Search();
    }

    void DownloadModsWindow::OnClickDownload()
    {
        const auto& indices = m_view->selectionModel()->selectedIndexes();

        if (indices.empty())
        {
            return;
        }

        const auto index = indices.at(0);
        const auto mod_id = static_cast<uint32_t>(index.internalId());

        if (!m_mod_cache.contains(mod_id))
        {
            return;
        }

        const auto& mod_info = m_mod_cache.at(mod_id);

        if (mod_info.files.empty())
        {
            return;
        }

        if (mod_info.files.size() == 1)
        {
            GApp->DownloadMod(m_instance, mod_info, mod_info.files.at(0));
        }
        else
        {
        }
    }

    void DownloadModsWindow::Search()
    {
        if (m_waiting_for_response || m_end_of_list || !m_ready_to_scroll_search)
        {
            return;
        }

        m_waiting_for_response = true;
        m_current_provider->GetMods(qstr(m_search_field->text()), m_current_page,
            QT::QueuedCallback(this, &DownloadModsWindow::OnSearchResponse));
    }

    void DownloadModsWindow::OnSearchResponse(DownloadModsWindow* self, const Provider::SearchResponse& response)
    {
        Logger::Debug("ui::download_mods_window", "Received search response (total_count={}, entries_count={})",
            response.total_count, response.mods.size());

        self->m_model->SetResponse(response);
        self->m_ready_to_scroll_search = true;
        self->m_waiting_for_response = false;

        QTimer::singleShot(200, self,
            [self]
            {
                self->m_ready_to_scroll_search = true;
            });

        if (response.mods.empty())
        {
            self->m_end_of_list = true;
        }
    }
}
// ReSharper restore CppDFAMemoryLeak