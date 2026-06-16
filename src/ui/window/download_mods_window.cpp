// ReSharper disable CppDFAMemoryLeak
#include "download_mods_window.h"

#include "core/logger.h"
#include "ui/delegate/provider/provider_mod_delegate.h"
#include "util/lib/qt.h"

#include <QHBoxLayout>
#include <QListView>

namespace Actinium
{
    DownloadModsWindow::DownloadModsWindow(Instance* instance, QWidget* parent)
        : BaseWindow(parent)
        , m_instance(instance)
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

        connect(m_view->selectionModel(), &QItemSelectionModel::selectionChanged, this,
            &DownloadModsWindow::OnSelectionChanged);

        m_mod_description = new QTextBrowser(this);
        m_mod_description->setObjectName("ModDescription");
        m_mod_description->setReadOnly(true);
        m_mod_description->setOpenExternalLinks(true);

        m_mod_file_selector = new QComboBox(this);
        m_mod_file_selector->setObjectName("ModFileSelector");
        m_mod_file_selector->setSizeAdjustPolicy(QComboBox::AdjustToContents);

        layout->addWidget(m_view);
        layout->addWidget(m_mod_description);
        layout->setContentsMargins(0, 0, 0, 0);

        main_layout->addLayout(layout);
        main_layout->addWidget(m_mod_file_selector, 0, Qt::AlignmentFlag::AlignRight | Qt::AlignmentFlag::AlignVCenter);
        main_layout->setContentsMargins(0, 0, 0, 0);

        const auto& providers = m_instance->GetGame()->providers;

        if (!providers.empty())
        {
            providers.at(0)->GetMods(QT::QueuedCallback(this, &DownloadModsWindow::OnSearchResponse));
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

        if (selected_indexes.size() == 0)
        {
            m_mod_description->setPlainText("");
            return;
        }

        const auto selected_index = selected_indexes.at(0);
        const auto mod_id = static_cast<uint32_t>(selected_index.internalId());

        m_instance->GetGame()->providers.at(0)->GetMod(mod_id,
            QT::QueuedCallback(this,
                [](const DownloadModsWindow* self, const Provider::ModInfo& mod_info)
                {
                    self->m_mod_description->setHtml(QString::fromStdString(mod_info.description));
                    self->m_mod_file_selector->clear();

                    for (const auto& file : mod_info.files)
                    {
                        self->m_mod_file_selector->addItem(QString::fromStdString(file.name));
                    }
                }));
    }

    void DownloadModsWindow::OnSearchResponse(DownloadModsWindow* self, const Provider::SearchResponse& response)
    {
        Logger::Debug("ui::download_mods_window", "Received search response (total_count={}, entries_count={})",
            response.total_count, response.mods.size());

        self->m_model->SetResponse(response);
    }
}
// ReSharper restore CppDFAMemoryLeak