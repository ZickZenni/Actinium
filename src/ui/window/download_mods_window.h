#pragma once

#include "base_window.h"
#include "instance/instance.h"

namespace Actinium
{
    class DownloadModsWindow : public BaseWindow
    {
        Q_OBJECT

    public:
        explicit DownloadModsWindow(Instance* instance, QWidget *parent = nullptr);
        ~DownloadModsWindow() override;

    private:
        Instance* m_instance;
        std::optional<Provider::SearchResponse> m_search_response;

        static void OnSearchResponse(DownloadModsWindow* self, const Provider::SearchResponse& response);
    };
}