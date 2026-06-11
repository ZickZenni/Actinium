// ReSharper disable CppDFAMemoryLeak
#include "download_mods_window.h"

#include "core/logger.h"
#include "ui/delegate/provider/provider_mod_delegate.h"
#include "util/lib/qt.h"

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

        m_view = new QListView(this);
        m_view->setObjectName("ModsListView");
        m_view->setModel(m_model);
        m_view->setItemDelegate(delegate);
        m_view->setResizeMode(QListView::Adjust);
        m_view->setUniformItemSizes(false);
        m_view->setAlternatingRowColors(true);
        m_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        m_view->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

        const auto& providers = m_instance->GetGame()->providers;

        if (!providers.empty())
        {
            providers.at(0)->GetMods(QT::QueuedCallback(this, &DownloadModsWindow::OnSearchResponse));
        }

        setCentralWidget(m_view);
    }

    DownloadModsWindow::~DownloadModsWindow()
    {
        Logger::Debug("ui::download_mods_window", "Destructor called");
    }

    void DownloadModsWindow::OnSearchResponse(DownloadModsWindow* self, const Provider::SearchResponse& response)
    {
        Logger::Debug("ui::download_mods_window", "Received search response (total_count={}, entries_count={})",
            response.total_count, response.mods.size());

        self->m_model->SetResponse(response);
    }
}
// ReSharper restore CppDFAMemoryLeak