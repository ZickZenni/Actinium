#include "application.h"

#include "provider/game_banana_provider.h"

#ifdef WIN32
#include "util/platform/win.h"
#endif

#include "build_config.h"
#include "loader/migoto.h"
#include "logger.h"
#include "ui/dialog/launch_instance_dialog.h"
#include "util/lib/json.h"
#include "util/lib/qt.h"
#include "util/software/steam.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QScreen>
#include <QStandardPaths>
#include <QStyleFactory>
#include <efsw/efsw.hpp>
#include <filesystem>
#include <spdlog/spdlog.h>

namespace Actinium
{
    // clang-format off
    std::vector<Game> Application::GAMES = {
        {
            .id = "genshin_impact",
            .name = "Genshin Impact",
            .executable_name = "GenshinImpact.exe",
            .libraries = {
                LoaderLibrary { "SilentNightSound", "GIMI-Package", []
                    {
                        return R"(Core\GIMI\main.ini)";
                    }
                },
            },
            .steam_app_id = std::nullopt,
            .steam_start_parameters = {},
            .providers = {
                new GameBananaProvider(8552)
            }
        },
        {
            .id = "honkai_star_rail",
            .name = "Honkai Star Rail",
            .executable_name = "StarRail.exe",
            .libraries = {
                LoaderLibrary { "SpectrumQT", "SRMI-Package", []
                    {
                        return R"(Core\SRMI\main.ini)";
                    }
                },
            },
            .steam_app_id = std::nullopt,
            .steam_start_parameters = {},
            .providers = {
                new GameBananaProvider(18366)
            }
        },
        {
            .id = "zenless_zone_zero",
            .name = "Zenless Zone Zero",
            .executable_name = "ZenlessZoneZero.exe",
            .libraries = {
                LoaderLibrary { "leotorrez", "ZZMI-Package", []
                    {
                        return R"(Core\ZZMI\main.ini)";
                    }
                },
            },
            .steam_app_id = std::nullopt,
            .steam_start_parameters = {},
            .providers = {
                new GameBananaProvider(19567)
            }
        },
        {
            .id = "wuthering_waves",
            .name = "Wuthering Waves",
            .executable_name = "Client-Win64-Shipping.exe",
            .libraries = {
                LoaderLibrary { "SpectrumQT", "WWMI-Package", []
                    {
                        return R"(Core\WWMI\WuWa-Model-Importer.ini)";
                    }
                },
            },
            .steam_app_id = 3513350,
            .steam_start_parameters = {
                "-DisableModule=streamline",
                "-dx11",
                "-d3d11",
            },
            .providers = {
                new GameBananaProvider(20357)
            }
        }
    };
    // clang-format on

    Application::Application(int argc, char* argv[])
        : QApplication(argc, argv)
        , m_file_watcher(new efsw::FileWatcher())
        , m_main_window(nullptr)
        , m_image_cache(nullptr)
    {
        setStyle(QStyleFactory::create("Fusion"));
        setOrganizationName("ZickZenni");
        setOrganizationDomain("https://github.com/ZickZenni");
        setApplicationName("actinium");
        setApplicationDisplayName("Actinium");
        setApplicationVersion(VERSION);

        Logger::Info("app::startup", "Storage directory is located on {}", GetAppDataPath().string());

        m_image_cache = new ImageCache(GetAppDataPath() / "cache" / "images");

        LoadConfig();

        QList<QCommandLineOption> options;
        options.push_back({{"l", "launch"}, "Launch a instance", "instance"});
        options.push_back({"skip-launch-check",
            "Skips the launch check for the loader and libraries, and straight goes to injecting"});

        m_parser.addOptions(options);
        m_parser.addHelpOption();
        m_parser.addVersionOption();
        m_parser.process(arguments());

        if (!m_parser.value("launch").isEmpty())
        {
            return;
        }

        m_file_watcher->watch();

        CreatePaths();
        LoadInstances();
        PrepareUI();
    }

    Application::~Application()
    {
        if (m_main_window != nullptr)
        {
            m_main_window->close();
            delete m_main_window;
        }

        for (const auto& instance : m_instances)
        {
            DestroyInstance(instance);
        }
    }

    int Application::Run()
    {
        const auto& launch_parameter = m_parser.value("launch");
        const auto& skip_launch_check = m_parser.isSet("skip-launch-check");

        if (!launch_parameter.isEmpty())
        {
            const auto instance = Instance::Load(launch_parameter.toStdString());

            if (instance == nullptr)
            {
                QMessageBox::critical(nullptr, "Error", "The specified instance does not exist.", QMessageBox::Close);
                return 1;
            }

            /**
             * This prevents infinite loops.
             */
            if (skip_launch_check)
            {
                return LaunchGameWithLoader(instance);
            }

            return LaunchInstance(instance);
        }

        m_main_window->show();

#ifdef WIN32
        if (Windows::HasElevatedPrivileges())
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
        const auto mods_directory = instance->GetAbsolutePath() / "mods";

        instance->m_mods_folder_watch_id = m_file_watcher->addWatch(mods_directory.string(), instance, false);
        instance->Save();

        m_instances.push_back(instance);
    }

    void Application::DeleteInstance(Instance* instance)
    {
        Q_CHECK_PTR(instance);

        std::erase_if(m_instances,
            [&instance](const auto& inst)
            {
                return inst == instance;
            });

        const auto instance_path = instance->GetAbsolutePath();

        if (!std::filesystem::exists(instance_path))
        {
            return;
        }

        std::error_code delete_code;
        std::filesystem::remove_all(instance_path, delete_code);

        if (delete_code)
        {
            return;
        }

        DestroyInstance(instance);
    }

    int Application::LaunchInstance(Instance* instance)
    {
        Q_CHECK_PTR(instance);

        if (!GetGameExecutable(instance->GetGame()->id).has_value())
        {
            return 1;
        }

        LaunchInstanceDialog dialog(instance, m_main_window);
        return dialog.exec();
    }

    int Application::LaunchGameWithLoader(const Instance* instance, QWidget* parent)
    {
#ifdef WIN32
        if (!GetGameExecutable(instance->GetGame()->id).has_value())
        {
            return 1;
        }

        if (!Windows::HasElevatedPrivileges())
        {
            const auto args = arguments();
            auto new_arguments = QT::ToStdVector(args);

            m_parser.parse(args);

            if (!m_parser.isSet("skip-launch-check"))
            {
                new_arguments.emplace_back("--skip-launch-check");
            }

            if (!m_parser.isSet("launch"))
            {
                new_arguments.emplace_back("--launch");
                new_arguments.push_back(instance->name);
            }

            Windows::ExecuteItselfWithElevatePrivileges(new_arguments);
            return 0;
        }

        if (!MigotoLoader::PrepareLoader(instance))
        {
            QMessageBox::critical(parent, "Error", "Failed to prepare loader for injection.", QMessageBox::Close);
            return 1;
        }

        const auto process_id = MigotoLoader::StartGame(instance);

        /**
         * The game is already running.
         */
        if (process_id == MIGOTO_PROCESS_ALREADY_RUNNING)
        {
            return 0;
        }

        if (process_id == 0)
        {
            QMessageBox::critical(parent, "Error", "Failed to launch game process.", QMessageBox::Close);
            return 1;
        }

        const auto result = MigotoLoader::InjectIntoProcess(instance, process_id);

        if (result == MigotoLoader::InjectResult::OK)
        {
            return 0;
        }

        QMessageBox::critical(parent, "Error", "Failed to inject loader into game process.", QMessageBox::Close);
        return 1;
#else
        QMessageBox::critical(parent, "Error", "This feature is not supported on this platform.");
        return 1;
#endif
    }

    std::optional<std::filesystem::path> Application::GetGameExecutable(const std::string& game_id)
    {
        const auto& game = GetGameById(game_id);

        if (game == nullptr)
        {
            return std::nullopt;
        }

        if (game->steam_app_id.has_value())
        {
            const auto& steam_installation = GetSteamExecutablePath();

            if (steam_installation.has_value())
            {
                return steam_installation;
            }
        }

        if (!m_config.game_executables.contains(game_id))
        {
            QFileDialog file_dialog(m_main_window);
            file_dialog.setFileMode(QFileDialog::ExistingFile);
            file_dialog.setNameFilter(tr(std::string("Game Executable (" + game->executable_name + ")").c_str()));

            if (file_dialog.exec())
            {
                const auto& selected_files = file_dialog.selectedFiles();
                const auto& selected_file = selected_files.front();

                m_config.game_executables.insert({game_id, selected_file.toStdString()});
                SaveConfig();
            }
            else
            {
                return std::nullopt;
            }
        }

        return m_config.game_executables.at(game_id);
    }

    ImageCache* Application::GetImageCache() const
    {
        return m_image_cache;
    }

    std::filesystem::path Application::GetAppDataPath()
    {
        return QStandardPaths::writableLocation(QStandardPaths::AppDataLocation).toStdString();
    }

    const std::vector<Game>& Application::GetSupportedGames()
    {
        return GAMES;
    }

    Game* Application::GetGameById(const std::string& id)
    {
        const auto result = std::ranges::find_if(GAMES,
            [&id](const Game& v)
            {
                return v.id == id;
            });

        return result != GAMES.end() ? std::to_address(result) : nullptr;
    }

    std::optional<std::filesystem::path> Application::GetSteamExecutablePath()
    {
        /**
         * Try to only run this a single time.
         */
        static const auto& path = Steam::DetectSteamInstallation();

        return path;
    }

    void Application::LoadConfig()
    {
        const auto& config_path = GetAppDataPath() / "config.json";

        Logger::Info("app::config::load", "Loading config file (file={})", config_path.string());

        if (!std::filesystem::exists(config_path))
        {
            Logger::Warn("app::config::load", "Could not load config file (error=File does not exist)");

            SaveConfig();
            return;
        }

        const auto& json = nlohmann::json::parse(std::ifstream(config_path));

        if (json.contains("game_executables"))
        {
            m_config.game_executables.clear();

            const auto& game_executables = json.at("game_executables");

            for (const auto& game : GAMES)
            {
                const auto& game_executable_path = nlohmann::try_get<std::string>(game_executables, game.id);

                if (!game_executable_path.has_value())
                {
                    Logger::Warn("app::config::load",
                        "Could not receive game executable value (game={}, error=Invalid value type or key does not "
                        "exist)",
                        game.id);
                    continue;
                }

                const auto& value = game_executable_path.value();

                if (!std::filesystem::exists(value))
                {
                    Logger::Warn("app::config::load",
                        "Could not find game executable (game={}, path={}, error=File does not exist)", game.id, value);
                    continue;
                }

                m_config.game_executables.insert({game.id, value});
                Logger::Info("app::config::load", "Found game executable (game={}, path={})", game.id, value);
            }
        }
    }

    void Application::SaveConfig()
    {
        const auto& config_path = GetAppDataPath() / "config.json";

        Logger::Info("app::config::save", "Saving config file (file={})", config_path.string());

        const auto& json = nlohmann::json {{"game_executables", m_config.game_executables}};

        std::ofstream(config_path) << json.dump(4);
    }

    void Application::LoadInstances()
    {
        const auto instances_path = GetAppDataPath() / "instances";

        Logger::Info("app::instances::load", "Loading instances (path={})", instances_path.string());

        const auto entries = std::filesystem::directory_iterator(instances_path);

        for (const auto& entry : entries)
        {
            if (!entry.is_directory())
            {
                Logger::Warn("app::instances::load", "Skipping non-directory entry (path={})", entry.path().string());
                continue;
            }

            const auto name = entry.path().filename().string();
            const auto instance = Instance::Load(name);

            if (instance != nullptr)
            {
                instance->m_mods_folder_watch_id
                    = m_file_watcher->addWatch((instance->GetAbsolutePath() / "mods").string(), instance, false);

                m_instances.push_back(instance);

                Logger::Info("app::instances::load", "Loaded instance (name={}, path={})", name,
                    instance->GetAbsolutePath().string());
            }
            else
            {
                Logger::Warn(
                    "app::instances::load", "Failed to load instance (name={}, path={})", name, entry.path().string());
            }
        }
    }

    void Application::DestroyInstance(const Instance* instance) const
    {
        Q_CHECK_PTR(instance);

        m_file_watcher->removeWatch(instance->m_mods_folder_watch_id);
        delete instance;
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
        std::filesystem::create_directories(appdata_path / "loaders");
        std::filesystem::create_directories(appdata_path / "libraries");
    }

    void Application::GetInitialWindowSize(int& out_width, int& out_height)
    {
        const auto primary_screen = primaryScreen();
        const auto screen_size = primary_screen->size();

        out_width = std::min(screen_size.width() - 20, 1280);
        out_height = std::min(screen_size.height() - 90, 720);
    }
}