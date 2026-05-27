// ReSharper disable CppDFAMemoryLeak
#include "main_window.h"

#include "core/application.h"
#include "ui/delegate/instance_delegate.h"
#include "ui/dialog/create_instance_dialog.h"

#include <QMessageBox>
#include <QPushButton>
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

        PrepareInstanceView();
        PrepareSideBar();
        PrepareToolBar();

        const auto layout = new QHBoxLayout(m_central_widget);
        layout->addWidget(m_instance_view);

        SetSideBarState(false);
        QMetaObject::connectSlotsByName(this);
    }

    void MainWindow::PrepareInstanceView()
    {
        m_instance_view = new InstanceView(this);
        m_instance_view->setSelectionMode(QAbstractItemView::SingleSelection);

        connect(m_instance_view, &InstanceView::selectionChanged, this, &MainWindow::OnInstanceSelected);

        m_instance_list_model = new InstanceListModel(&m_app->m_instances, this);
        m_instance_view->setModel(m_instance_list_model);

        const auto delegate = new InstanceDelegate(this);
        m_instance_view->setItemDelegate(delegate);
        m_instance_view->setFrameShape(QFrame::NoFrame);
        m_instance_view->setAttribute(Qt::WA_MacShowFocusRect, false);
    }

    void MainWindow::PrepareSideBar()
    {
        m_instance_sidebar = new SideBar(this);
        m_instance_sidebar->setObjectName("InstanceSidebar");
        m_instance_sidebar->setFloatable(false);
        m_instance_sidebar->setAllowedAreas(Qt::LeftToolBarArea | Qt::RightToolBarArea);
        m_instance_sidebar->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
        m_instance_sidebar->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
        m_instance_sidebar->setMinimumWidth(150);
        m_instance_sidebar->setOrientation(Qt::Vertical);

        connect(m_instance_sidebar, &QToolBar::orientationChanged,
            [this](Qt::Orientation)
            {
                m_instance_sidebar->setOrientation(Qt::Vertical);
            });

        m_sidebar_rename_button = new QPushButton(this);
        m_sidebar_rename_button->setObjectName("RenameInstanceButton");
        m_sidebar_rename_button->setText("");
        m_sidebar_rename_button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

        m_sidebar_edit_button = new QPushButton(this);
        m_sidebar_edit_button->setObjectName("EditInstanceButton");
        m_sidebar_edit_button->setText("Edit Instance");
        m_sidebar_edit_button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

        m_sidebar_delete_button = new QPushButton(this);
        m_sidebar_delete_button->setObjectName("DeleteInstanceButton");
        m_sidebar_delete_button->setText("Delete Instance");
        m_sidebar_delete_button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

        connect(m_sidebar_delete_button, &QPushButton::clicked, this, &MainWindow::DeleteInstance);

        m_instance_sidebar->addWidget(m_sidebar_rename_button);
        m_instance_sidebar->addSeparator();
        m_instance_sidebar->addWidget(m_sidebar_edit_button);
        m_instance_sidebar->addWidget(m_sidebar_delete_button);

        addToolBar(Qt::RightToolBarArea, m_instance_sidebar);
    }

    void MainWindow::PrepareToolBar()
    {
        const auto toolbar = new QToolBar(this);
        toolbar->setObjectName("MainToolbar");
        toolbar->setFloatable(false);
        toolbar->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);

        const auto action_create_instance = new QAction(tr("&Create instance"), this);
        connect(action_create_instance, &QAction::triggered, this, &MainWindow::CreateInstance);

        toolbar->addAction(action_create_instance);

        addToolBar(Qt::TopToolBarArea, toolbar);
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

    void MainWindow::DeleteInstance()
    {
        const auto index = m_instance_view->selectionModel()->currentIndex();

        if (!index.isValid())
        {
            return;
        }

        const auto instance = m_instance_list_model->at(index.row());

        if (instance == nullptr)
        {
            return;
        }

        const auto message
            = std::format("You are about to delete \"{}\".\nThis may be permanent and will completely delete the "
                          "instance.\n\nAre you sure?",
                instance->name);
        const auto result = QMessageBox::warning(
            this, "Confirm Deletion", message.c_str(), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

        if (result == QMessageBox::No)
        {
            return;
        }

        std::erase_if(m_app->m_instances,
            [&instance](const auto& inst)
            {
                return inst == instance;
            });

        const auto instance_path = instance->GetAbsolutePath();

        if (!std::filesystem::exists(instance_path))
        {
            SPDLOG_ERROR("Instance path does not exist: {}", instance_path.string());
            return;
        }

        std::error_code delete_code;
        std::filesystem::remove_all(instance_path, delete_code);

        if (delete_code)
        {
            SPDLOG_ERROR("Failed to delete instance on the disk: {}", delete_code.message());
            return;
        }

        SPDLOG_INFO("Deleted instance: {}", instance->name);
        delete instance;
    }

    void MainWindow::OnInstanceSelected(const QModelIndex& index) const
    {
        if (!index.isValid())
        {
            m_sidebar_rename_button->setText("");
            SetSideBarState(false);
            return;
        }

        const auto instance = m_instance_list_model->at(index.row());

        if (instance == nullptr)
        {
            m_sidebar_rename_button->setText("");
            SetSideBarState(false);
            return;
        }

        SPDLOG_DEBUG("Selected instance: {}", instance->name);
        m_sidebar_rename_button->setText(QString::fromStdString(instance->name));

        SetSideBarState(true);
    }

    void MainWindow::SetSideBarState(const bool instance_selected) const
    {
        m_sidebar_rename_button->setEnabled(instance_selected);
        m_sidebar_edit_button->setEnabled(instance_selected);
        m_sidebar_delete_button->setEnabled(instance_selected);
    }
}
// ReSharper restore CppDFAMemoryLeak