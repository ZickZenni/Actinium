#pragma once

#include "base_window.h"
#include "instance/instance.h"
#include "ui/model/provider/provider_mod_model.h"

#include <QListView>

namespace Actinium
{
    class DownloadModsWindow : public BaseWindow
    {
        Q_OBJECT

    public:
        explicit DownloadModsWindow(Instance *instance, QWidget *parent = nullptr);
        ~DownloadModsWindow() override;

    private:
        Instance *m_instance;
        ProviderModModel *m_model;
        QListView *m_view;

        static void OnSearchResponse(DownloadModsWindow *self, const Provider::SearchResponse &response);
    };
}