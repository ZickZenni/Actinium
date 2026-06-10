#pragma once

#include "instance/instance.h"
#include "ui/model/proxy_model.h"
#include "ui/page/page.h"
#include "ui/view/mod_list_view.h"

namespace Actinium
{
    class InstanceModsPage : public Page
    {
    public:
        explicit InstanceModsPage(Instance *instance, QWidget *parent = nullptr);

        QString GetButtonText() override;

    private:
        Instance *m_instance;
        ProxyModel *m_list_proxy_model;
        ModListModel *m_list_model;
        ModListView *m_list_view;

        void OnDownloadModsClicked();
    };
}