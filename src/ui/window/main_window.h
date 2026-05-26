#pragma once

#include "ui/model/instance_model.h"
#include "ui/view/instance_view.h"

#include <QMainWindow>

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

        void PrepareToolBar();

        void CreateInstance() const;
    };
}