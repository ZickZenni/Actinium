#include "application.h"

#include <QMessageBox>
#include <QScreen>
#include <QStandardPaths>
#include <filesystem>
#include <spdlog/spdlog.h>

namespace Actinium
{
    Application::Application(int argc, char* argv[])
        : QApplication(argc, argv)
        , m_main_window(nullptr)
    {
        setOrganizationName("ZickZenni");
        setOrganizationDomain("https://github.com/ZickZenni");
        setApplicationName("actinium");
        setApplicationDisplayName("Actinium");
        setApplicationVersion("0.0.1");

        spdlog::set_pattern("[%d/%m/%Y %T] [%^%l%$] [%s:%# %!()] %v");
        SPDLOG_INFO("Running {} on v{}", applicationDisplayName().toStdString(), applicationVersion().toStdString());

        CreatePaths();
        LoadInstances();
        PrepareUI();
    }

    Application::~Application()
    {
        for (const auto& instance : m_instances)
        {
            delete instance;
        }

        delete m_main_window;
    }

    int Application::Run() const
    {
        m_main_window->show();

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
        const auto entries = std::filesystem::directory_iterator(instances_path);

        for (const auto& entry : entries)
        {
            if (!entry.is_directory())
            {
                continue;
            }

            const auto name = entry.path().filename().string();
            const auto instance = Instance::Load(name);

            if (instance != nullptr)
            {
                SPDLOG_INFO("Loaded instance: {}", name);
                m_instances.push_back(instance);
            }
        }
    }

    void Application::PrepareUI()
    {
        m_main_window = new MainWindow(this);

        int width, height;
        GetInitialWindowSize(width, height);

        m_main_window->resize(width, height);
    }

    void Application::CreatePaths()
    {
        const auto appdata_path = GetAppDataPath();

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
}