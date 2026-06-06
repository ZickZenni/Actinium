#pragma once

#include "game/game.h"
#include "instance/instance.h"
#include "ui/window/main_window.h"
#include "util/api/github.h"

#include <QApplication>
#include <QCommandLineParser>
#include <filesystem>

namespace Actinium
{
    class Application final : public QApplication
    {
    public:
        struct Config
        {
            std::unordered_map<std::string_view, std::string> game_executables;
        };

    public:
        Application(int argc, char *argv[]);
        ~Application() override;

        /**
         * Runs the application.
         */
        int Run();

        /**
         * Creates a new instance for a game.
         */
        void CreateInstance(const std::string &name, const std::string &game_id);

        /**
         * Launches an instance.
         */
        int LaunchInstance(Instance *instance);

        /**
         * Launches the game (that the instance is for) and injects the mod loader into the game process.
         *
         * @return Exit code for this process.
         */
        int LaunchGameWithLoader(const Instance *instance, QWidget *parent = nullptr);

        /**
         * Retrieves the path to the game's executable file.
         * Has no value if the user does not select the executable or something went wrong.
         */
        std::optional<std::filesystem::path> GetGameExecutable(const std::string &game_id);

        /**
         * Retrieves the appdata path for the application.
         */
        static std::filesystem::path GetAppDataPath();

        /**
         * Retrieves all supported games.
         */
        static const std::vector<Game> &GetSupportedGames();

        /**
         * Retrieves a support game by its identifier.
         */
        static Game *GetGameById(const std::string &id);

        /**
         * Retrieves the path of the steam executable, if installed.
         */
        static std::optional<std::filesystem::path> GetSteamExecutablePath();

    private:
        static std::vector<Game> GAMES;

        Config m_config;
        QCommandLineParser m_parser;
        std::vector<Instance *> m_instances;
        MainWindow *m_main_window;

        void LoadConfig();

        void SaveConfig();

        /**
         * Loads all instances.
         */
        void LoadInstances();

        /**
         * Prepares everything UI-related.
         */
        void PrepareUI();

        /**
         * Creates all required directories.
         */
        static void CreatePaths();

        /**
         * Calculates the initial window size for the main window.
         */
        static void GetInitialWindowSize(int &out_width, int &out_height);

        /**
         * Logs environmental information to the console / logger (spdlog) for debugging purposes.
         */
        static void LogEnvironmentInfo();

        friend class MainWindow;
    };

    inline Application *GApp = nullptr;
}