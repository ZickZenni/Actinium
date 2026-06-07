// ReSharper disable CppDFAMemoryLeak
#include "main_window.h"

#include "core/application.h"
#include "core/logger.h"
#include "ui/delegate/instance_delegate.h"
#include "ui/dialog/create_instance_dialog.h"
#include "util/lib/qt.h"

#include <QCloseEvent>
#include <QDesktopServices>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QPushButton>
#include <QToolBar>
#include <spdlog/spdlog.h>

namespace Actinium
{
    MainWindow::MainWindow(Application* app, QWidget* widget)
        : QMainWindow(widget)
        , m_app(app)
        , m_central_widget(nullptr)
        , m_instance_list_model(nullptr)
        , m_instance_view(nullptr)
        , m_instance_sidebar(nullptr)
        , m_sidebar_rename_button(nullptr)
        , m_sidebar_launch_button(nullptr)
        , m_sidebar_edit_button(nullptr)
        , m_sidebar_delete_button(nullptr)
        , m_sidebar_folder_button(nullptr)
    {
        setObjectName("MainWindow");

        m_central_widget = new QWidget(this);
        m_central_widget->setObjectName("CentralWidget");
        setCentralWidget(m_central_widget);

        PrepareInstanceView();
        PrepareSideBar();
        PrepareToolBar();

        const auto layout = new QHBoxLayout(m_central_widget);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->addWidget(m_instance_view);

        SetSideBarState(false);
        QMetaObject::connectSlotsByName(this);
    }

    void MainWindow::closeEvent(QCloseEvent* event)
    {
        Logger::Debug("ui", "main_window_close_event", "Closing main window");

        for (const auto& window : m_instance_windows)
        {
            window->close();
            delete window;
        }

        event->accept();
    }

    void MainWindow::PrepareInstanceView()
    {
        m_instance_view = new InstanceView(this);
        m_instance_view->setSelectionMode(QAbstractItemView::SingleSelection);
        m_instance_view->setFrameShape(QFrame::NoFrame);
        m_instance_view->setAttribute(Qt::WA_MacShowFocusRect, false);
        m_instance_view->setContentsMargins(0, 0, 0, 0);

        connect(m_instance_view, &InstanceView::SelectionChanged, this, &MainWindow::OnInstanceSelected);

        m_instance_list_model = new InstanceListModel(&m_app->m_instances, this);
        m_instance_view->setModel(m_instance_list_model);

        const auto delegate = new InstanceDelegate(this);
        m_instance_view->setItemDelegate(delegate);
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

        m_sidebar_launch_button = new QPushButton(this);
        m_sidebar_launch_button->setObjectName("LaunchInstanceButton");
        m_sidebar_launch_button->setText("Launch");
        m_sidebar_launch_button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

        connect(m_sidebar_launch_button, &QPushButton::clicked, this, &MainWindow::LaunchInstance);

        m_sidebar_edit_button = new QPushButton(this);
        m_sidebar_edit_button->setObjectName("EditInstanceButton");
        m_sidebar_edit_button->setText("Edit Instance");
        m_sidebar_edit_button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

        connect(m_sidebar_edit_button, &QPushButton::clicked, this, &MainWindow::EditInstance);

        m_sidebar_delete_button = new QPushButton(this);
        m_sidebar_delete_button->setObjectName("DeleteInstanceButton");
        m_sidebar_delete_button->setText("Delete Instance");
        m_sidebar_delete_button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

        connect(m_sidebar_delete_button, &QPushButton::clicked, this, &MainWindow::DeleteInstance);

        m_sidebar_folder_button = new QPushButton(this);
        m_sidebar_folder_button->setObjectName("OpenInstanceFolderButton");
        m_sidebar_folder_button->setText("Open Folder");
        m_sidebar_folder_button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

        connect(m_sidebar_folder_button, &QPushButton::clicked, this, &MainWindow::OpenInstanceFolder);

        m_instance_sidebar->addWidget(m_sidebar_rename_button);
        m_instance_sidebar->addSeparator();
        m_instance_sidebar->addWidget(m_sidebar_launch_button);
        m_instance_sidebar->addSeparator();
        m_instance_sidebar->addWidget(m_sidebar_edit_button);
        m_instance_sidebar->addWidget(m_sidebar_delete_button);
        m_instance_sidebar->addWidget(m_sidebar_folder_button);

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

    void MainWindow::CreateInstance()
    {
        CreateInstanceDialog dialog(this);

        if (!dialog.exec())
        {
            return;
        }

        const auto name_value = dialog.GetInstanceName().toStdString();
        const auto game_value = dialog.GetGameId().toStdString();

        if (name_value.empty() || game_value.empty())
        {
            Logger::Error(
                "ui", "create_instance_invalid_inputs", "Some of the inputs are empty, this should not be possible");
            return;
        }

        m_app->CreateInstance(name_value, game_value);
    }

    void MainWindow::LaunchInstance() const
    {
        const auto instance = GetSelectedInstance();

        if (instance == nullptr)
        {
            return;
        }

        GApp->LaunchInstance(instance);
    }

    void MainWindow::EditInstance()
    {
        const auto instance = GetSelectedInstance();

        if (instance == nullptr)
        {
            return;
        }

        Logger::Debug("ui", "open_instance_window", "Opening window for instance: {}", instance->name);

        const auto instance_window = new InstanceWindow(instance, this);
        instance_window->show();

        connect(instance_window, &InstanceWindow::IsClosing, this, &MainWindow::OnInstanceWindowClosed);

        m_instance_windows.push_back(instance_window);
    }

    void MainWindow::DeleteInstance()
    {
        constexpr QMessageBox::StandardButtons BUTTONS = QMessageBox::Yes | QMessageBox::No;
        constexpr QMessageBox::Button DEFAULT_BUTTON = QMessageBox::No;

        static const auto TITLE = QStringLiteral("Confirm Deletion");
        static const auto MESSAGE = QStringLiteral("You are about to delete \"%1\".\n"
                                                   "This may be permanent and will completely delete the instance.\n\n"
                                                   "Are you sure?");

        const auto instance = GetSelectedInstance();

        if (instance == nullptr)
        {
            return;
        }

        const auto message = MESSAGE.arg(strq(instance->name));
        const auto result = QMessageBox::warning(this, TITLE, message, BUTTONS, DEFAULT_BUTTON);

        if (result == QMessageBox::No)
        {
            return;
        }

        for (const auto& window : m_instance_windows)
        {
            if (window->GetInstance() == instance)
            {
                window->close();
            }
        }

        std::erase_if(m_app->m_instances,
            [&instance](const auto& inst)
            {
                return inst == instance;
            });

        const auto instance_path = instance->GetAbsolutePath();

        if (!std::filesystem::exists(instance_path))
        {
            Logger::Error(
                "ui", "delete_instance_disk_failed", "Instance path does not exist: {}", instance_path.string());
            return;
        }

        std::error_code delete_code;
        std::filesystem::remove_all(instance_path, delete_code);

        if (delete_code)
        {
            Logger::Error("ui", "delete_instance_disk_failed", "Failed to delete instance on the disk: {}",
                delete_code.message());
            return;
        }

        Logger::Info("ui", "delete_instance_success", "Deleted instance: {}", instance->name);
        delete instance;
    }

    void MainWindow::OpenInstanceFolder() const
    {
        const auto instance = GetSelectedInstance();

        if (instance == nullptr)
        {
            return;
        }

        const auto url = QT::CreateFileUrl(instance->GetAbsolutePath());
        Logger::Debug("ui", "open_url", "Opening url: {}", url.toEncoded().toStdString());

        QDesktopServices::openUrl(url);
    }

    void MainWindow::OnInstanceSelected([[maybe_unused]] const QModelIndex& index) const
    {
        const auto instance = GetSelectedInstance();

        if (instance == nullptr)
        {
            m_sidebar_rename_button->setText("");
            SetSideBarState(false);
            return;
        }

        Logger::Debug("ui", "select_instance_event", "Selected instance: {}", instance->name);
        m_sidebar_rename_button->setText(QString::fromStdString(instance->name));

        SetSideBarState(true);
    }

    void MainWindow::OnInstanceWindowClosed()
    {
        const auto instance_window = qobject_cast<InstanceWindow*>(sender());

        if (instance_window == nullptr)
        {
            Logger::Error("ui", "invalid_instance_window_closed", "Received non instance window pointer as sender");
            return;
        }

        Logger::Debug("ui", "instance_window_close_event", "Closing window for instance: {}",
            instance_window->GetInstance()->name);

        std::erase_if(m_instance_windows,
            [instance_window](const auto& window)
            {
                return window == instance_window;
            });
    }

    void MainWindow::SetSideBarState(const bool instance_selected) const
    {
        m_sidebar_rename_button->setEnabled(instance_selected);
        m_sidebar_launch_button->setEnabled(instance_selected);
        m_sidebar_edit_button->setEnabled(instance_selected);
        m_sidebar_delete_button->setEnabled(instance_selected);
        m_sidebar_folder_button->setEnabled(instance_selected);
    }

    Instance* MainWindow::GetSelectedInstance() const
    {
        const auto index = m_instance_view->selectionModel()->currentIndex();

        if (!index.isValid())
        {
            return nullptr;
        }

        return m_instance_list_model->at(index.row());
    }
}
// ReSharper restore CppDFAMemoryLeak