// ReSharper disable CppDFAMemoryLeak
#include "main_window.h"

#include "core/application.h"
#include "ui/dialog/create_instance_dialog.h"

#include <QToolBar>
#include <iostream>

namespace Actinium
{
    MainWindow::MainWindow(Application* app, QWidget* widget)
        : QMainWindow(widget)
        , m_app(app)
    {
        setObjectName("MainWindow");

        m_central_widget = new QWidget(this);
        m_central_widget->setObjectName("CentralWidget");
        setCentralWidget(m_central_widget);

        PrepareToolBar();

        QMetaObject::connectSlotsByName(this);
    }

    void MainWindow::PrepareToolBar()
    {
        const auto action_create_instance = new QAction(tr("&Create instance"), this);
        connect(action_create_instance, &QAction::triggered, this, &MainWindow::CreateInstance);

        const auto toolbar = addToolBar("MainToolbar");
        toolbar->addAction(action_create_instance);
    }

    void MainWindow::CreateInstance() const
    {
        CreateInstanceDialog dialog;

        if (!dialog.exec())
        {
            return;
        }

        const auto name_value = dialog.GetNameValue().toStdString();
        const auto game_value = dialog.GetGameValue().toStdString();

        if (name_value.empty())
        {
            std::cout << "[MainWindow::CreateInstance] User did not input a name" << std::endl;
            return;
        }

        if (game_value.empty())
        {
            std::cout << "[MainWindow::CreateInstance] User did not input a game" << std::endl;
            return;
        }

        m_app->CreateInstance(name_value, game_value);
    }
}
// ReSharper restore CppDFAMemoryLeak