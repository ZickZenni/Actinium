#include "download_mods_window.h"

#include "core/logger.h"

#include <QPointer>

namespace Actinium
{
    DownloadModsWindow::DownloadModsWindow(Instance* instance, QWidget* parent)
        : BaseWindow(parent)
        , m_instance(instance)
    {
        QPointer self(this);

        m_instance->GetGame()->providers.at(0)->GetMods(
            [self](const Provider::SearchResponse& response)
            {
                if (!self)
                {
                    return;
                }

                QMetaObject::invokeMethod(
                    self,
                    [self, response]()
                    {
                        if (!self)
                        {
                            return;
                        }

                        self->m_search_response = response;

                        if (self->m_search_response.has_value())
                        {
                            for (const auto& mod : self->m_search_response.value().mods)
                            {
                                Logger::Debug("ui::download_mods_window", "Found mod: {}", mod.name);
                            }
                        }
                    },
                    Qt::QueuedConnection);
            });
    }

    DownloadModsWindow::~DownloadModsWindow()
    {
        Logger::Debug("ui::download_mods_window", "Destructor called");
    }
}