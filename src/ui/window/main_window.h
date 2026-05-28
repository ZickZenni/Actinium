#pragma once

#include "instance_window.h"
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

    protected:
        void closeEvent(QCloseEvent *event) override;

    private:
        Application *m_app;

        std::vector<InstanceWindow *> m_instance_windows;
        QWidget *m_central_widget;
        InstanceListModel *m_instance_list_model;
        InstanceView *m_instance_view;
        SideBar *m_instance_sidebar;

        QPushButton *m_sidebar_rename_button;
        QPushButton *m_sidebar_edit_button;
        QPushButton *m_sidebar_delete_button;
        QPushButton *m_sidebar_folder_button;

        void PrepareInstanceView();

        void PrepareSideBar();

        void PrepareToolBar();

        void CreateInstance();

        void EditInstance();

        void DeleteInstance();

        void OpenInstanceFolder() const;

        void OnInstanceSelected(const QModelIndex &index) const;

        void OnInstanceWindowClosed();

        void SetSideBarState(bool instance_selected) const;

        Instance *GetSelectedInstance() const;
    };
}