#include "application.h"

#include <QMessageBox>
#include <QScreen>
#include <QStandardPaths>
#include <filesystem>

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
        const auto game = std::ranges::find_if(GAMES,
            [&game_id](const auto& v)
            {
                return v.GetId() == game_id;
            });

        if (game == GAMES.end())
        {
            QMessageBox::critical(
                m_main_window, "Internal Error", "The selected game was not found internally.", QMessageBox::Close);
            return;
        }

        const auto instance = new Instance(game, name);
        instance->Save();

        m_instances.push_back(instance);

        std::cout << "[Application::CreateInstance] Instance created" << std::endl;
    }

    std::filesystem::path Application::GetAppDataPath()
    {
        return QStandardPaths::writableLocation(QStandardPaths::AppDataLocation).toStdString();
    }

    const Game* Application::GetGameById(const std::string& id)
    {
        return std::ranges::find_if(GAMES,
            [&id](const auto& v)
            {
                return v.GetId() == id;
            });
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
                std::cout << "[Application::LoadInstances] Loaded instance: " << name << std::endl;
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