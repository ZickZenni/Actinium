// ReSharper disable CppDFAMemoryLeak
#include "main_window.h"

#include "ui/dialog/create_instance_dialog.h"

#include <QPushButton>
#include <iostream>

namespace Actinium
{
    MainWindow::MainWindow(QWidget* widget)
        : QMainWindow(widget)
    {
        setObjectName("MainWindow");

        m_central_widget = new QWidget(this);
        m_central_widget->setObjectName("CentralWidget");
        setCentralWidget(m_central_widget);

        const auto create_instance_button = new QPushButton("Create Instance", m_central_widget);
        create_instance_button->setObjectName("CreateInstanceButton");
        connect(create_instance_button, &QPushButton::clicked, this, &MainWindow::CreateInstance);

        QMetaObject::connectSlotsByName(this);
    }

    void MainWindow::CreateInstance()
    {
        if (CreateInstanceDialog dialog; !dialog.exec())
        {
            return;
        }

        std::cout << "[MainWindow] Success?" << std::endl;
    }
}
// ReSharper restore CppDFAMemoryLeak