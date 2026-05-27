#pragma once

#include "ui/model/instance_model.h"
#include "ui/view/instance_view.h"
#include "ui/widget/sidebar.h"

#include <QMainWindow>
#include <QPushButton>

namespace Actinium
{
    class Application;

    class MainWindow final : public QMainWindow
    {
        Q_OBJECT

    public:
        explicit MainWindow(Application *app, QWidget *widget = nullptr);

    private:
        Application *m_app;

        QWidget *m_central_widget;
        InstanceListModel *m_instance_list_model;
        InstanceView *m_instance_view;
        SideBar* m_instance_sidebar;

        QPushButton* m_sidebar_rename_button;
        QPushButton* m_sidebar_edit_button;
        QPushButton* m_sidebar_delete_button;

        void PrepareInstanceView();

        void PrepareSideBar();

        void PrepareToolBar();

        void CreateInstance() const;

        void DeleteInstance();

        void OnInstanceSelected(const QModelIndex &index) const;

        void SetSideBarState(bool instance_selected) const;
    };
}