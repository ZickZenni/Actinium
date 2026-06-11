#include "download_mods_window.h"

#include "core/logger.h"
#include "util/lib/qt.h"

namespace Actinium
{
    DownloadModsWindow::DownloadModsWindow(Instance* instance, QWidget* parent)
        : BaseWindow(parent)
        , m_instance(instance)
    {
        const auto& providers = m_instance->GetGame()->providers;

        if (!providers.empty())
        {
            providers.at(0)->GetMods(QT::QueuedCallback(this, &DownloadModsWindow::OnSearchResponse));
        }
    }

    DownloadModsWindow::~DownloadModsWindow()
    {
        Logger::Debug("ui::download_mods_window", "Destructor called");
    }

    void DownloadModsWindow::OnSearchResponse(DownloadModsWindow* self, const Provider::SearchResponse& response)
    {
        self->m_search_response = response;

        if (self->m_search_response.has_value())
        {
            for (const auto& mod : self->m_search_response.value().mods)
            {
                Logger::Debug("ui::download_mods_window", "Found mod: {}", mod.name);
            }
        }
    }
}