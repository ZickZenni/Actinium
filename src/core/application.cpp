#include "application.h"

#ifdef WIN32
#include "util/win.h"
#endif

#include "build_config.h"
#include "ui/dialog/launch_instance_dialog.h"
#include "util/qt.h"

#include <QMessageBox>
#include <QScreen>
#include <QStandardPaths>
#include <QStyleFactory>
#include <filesystem>
#include <spdlog/spdlog.h>

namespace Actinium
{
    Application::Application(int argc, char* argv[])
        : QApplication(argc, argv)
        , m_main_window(nullptr)
    {
        setStyle(QStyleFactory::create("Fusion"));
        setOrganizationName("ZickZenni");
        setOrganizationDomain("https://github.com/ZickZenni");
        setApplicationName("actinium");
        setApplicationDisplayName("Actinium");
        setApplicationVersion(VERSION);

        spdlog::set_level(spdlog::level::trace);
        spdlog::set_pattern("[%H:%M:%S.%e] [%^%-8l%$] [%s:%#] %v");
        SPDLOG_INFO("Running {} on v{}", applicationDisplayName().toStdString(), applicationVersion().toStdString());

        LogEnvironmentInfo();
        CreatePaths();
        LoadInstances();
        PrepareUI();
    }

    Application::~Application()
    {
        m_main_window->close();

        delete m_main_window;

        for (const auto& instance : m_instances)
        {
            delete instance;
        }
    }

    int Application::Run() const
    {
        m_main_window->show();

#ifdef WIN32
        if (WinUtils::HasElevatedPrivileges())
        {
            QMessageBox::warning(m_main_window, "Warning",
                "The application is running with administrator privileges. This is not recommended unless necessary. "
                "Please run without elevated permissions whenever possible.",
                QMessageBox::Ok);
        }
#endif

        return exec();
    }

    void Application::CreateInstance(const std::string& name, const std::string& game_id)
    {
        const auto game = GetGameById(game_id);

        if (game == nullptr)
        {
            QMessageBox::critical(
                m_main_window, "Internal Error", "The selected game was not found internally.", QMessageBox::Close);
            return;
        }

        const auto instance = new Instance(game, name);
        instance->Save();

        m_instances.push_back(instance);

        SPDLOG_INFO("Created instance: {}", name);
    }

    void Application::LaunchInstance(Instance* instance)
    {
        Q_CHECK_PTR(instance);

        SPDLOG_INFO("Launching instance \"{}\"", instance->name);

        LaunchInstanceDialog dialog(instance, m_main_window);
        dialog.exec();
    }

    std::filesystem::path Application::GetAppDataPath()
    {
        return QStandardPaths::writableLocation(QStandardPaths::AppDataLocation).toStdString();
    }

    const Game* Application::GetGameById(const std::string& id)
    {
        const auto result = std::ranges::find_if(GAMES,
            [&id](const auto& v)
            {
                return v.GetId() == id;
            });

        return result != GAMES.end() ? result : nullptr;
    }

    void Application::LoadInstances()
    {
        const auto instances_path = GetAppDataPath() / "instances";
        SPDLOG_INFO("Loading instances in \"{}\"", instances_path.string());

        const auto entries = std::filesystem::directory_iterator(instances_path);

        for (const auto& entry : entries)
        {
            if (!entry.is_directory())
            {
                SPDLOG_WARN("Skipping non-directory entry \"{}\"", entry.path().string());
                continue;
            }

            const auto name = entry.path().filename().string();
            SPDLOG_INFO("Found instance directory \"{}\"", name);

            const auto instance = Instance::Load(name);

            if (instance != nullptr)
            {
                SPDLOG_INFO("Loaded instance \"{}\" from \"{}\"", instance->name, entry.path().string());
                m_instances.push_back(instance);
            }
            else
            {
                SPDLOG_ERROR("Failed to load instance \"{}\" from \"{}\"", name, entry.path().string());
            }
        }
    }

    void Application::PrepareUI()
    {
        SPDLOG_DEBUG("Preparing UI for application");

        m_main_window = new MainWindow(this);

        int width, height;
        GetInitialWindowSize(width, height);

        SPDLOG_DEBUG("Initial window size: {}x{}", width, height);

        m_main_window->resize(width, height);
    }

    void Application::CreatePaths()
    {
        const auto appdata_path = GetAppDataPath();
        SPDLOG_INFO("Using data path \"{}\"", appdata_path.string());

        std::filesystem::create_directories(appdata_path);
        std::filesystem::create_directories(appdata_path / "instances");
    }

    void Application::GetInitialWindowSize(int& out_width, int& out_height)
    {
        const auto primary_screen = primaryScreen();
        const auto screen_size = primary_screen->size();

        out_width = std::min(screen_size.width() - 20, 1280);
        out_height = std::min(screen_size.height() - 90, 720);
    }

    void Application::LogEnvironmentInfo()
    {
        SPDLOG_INFO("Architecture: {}", qstr(QSysInfo::buildCpuArchitecture()));
        SPDLOG_INFO("Platform: {} ({})", qstr(QSysInfo::prettyProductName()), qstr(QSysInfo::productVersion()));
        SPDLOG_INFO("Hostname: {}", qstr(QSysInfo::machineHostName()));
        SPDLOG_INFO("Git Commit: {} - \"{}\"", GIT_COMMIT_HASH, GIT_COMMIT_MESSAGE);
        SPDLOG_INFO("Git Branch: {}", GIT_BRANCH);
        SPDLOG_INFO("Build Timestamp: {}", BUILD_TIMESTAMP);
    }
}