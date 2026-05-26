// ReSharper disable CppDFAMemoryLeak
#include "main_window.h"

#include "core/application.h"
#include "ui/delegate/instance_delegate.h"
#include "ui/dialog/create_instance_dialog.h"

#include <QToolBar>
#include <spdlog/spdlog.h>

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

        m_instance_view = new InstanceView();
        m_instance_view->setSelectionMode(QAbstractItemView::SingleSelection);

        m_instance_list_model = new InstanceListModel(&m_app->m_instances, this);
        m_instance_view->setModel(m_instance_list_model);

        const auto delegate = new InstanceDelegate(this);
        m_instance_view->setItemDelegate(delegate);
        m_instance_view->setFrameShape(QFrame::NoFrame);
        m_instance_view->setAttribute(Qt::WA_MacShowFocusRect, false);

        const auto layout = new QVBoxLayout(m_central_widget);
        layout->addWidget(m_instance_view);

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

        if (name_value.empty() || game_value.empty())
        {
            SPDLOG_ERROR("Some of the inputs are empty, this should not be possible");
            return;
        }

        m_app->CreateInstance(name_value, game_value);
    }
}
// ReSharper restore CppDFAMemoryLeak