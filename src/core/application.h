#pragma once

#include "download_manager.h"
#include "game/game.h"
#include "instance/instance.h"
#include "ui/image_cache.h"
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
         * Deletes the instance and it's content saved on the disk.
         */
        void DeleteInstance(Instance *instance);

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

        void DownloadMod(Instance* instance, const Provider::ModInfo& mod, const Provider::File &file) const;

        /**
         * Retrieves the path to the game's executable file.
         * Has no value if the user does not select the executable or something went wrong.
         */
        std::optional<std::filesystem::path> GetGameExecutable(const std::string &game_id);

        /**
         * Retrieves the image cache of the application.
         */
        [[nodiscard]] ImageCache* GetImageCache() const;

        /**
         * Retrieves the download manager of the application.
         */
        [[nodiscard]] DownloadManager* GetDownloadManager() const;

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

        efsw::FileWatcher *m_file_watcher;
        Config m_config;
        QCommandLineParser m_parser;
        std::vector<Instance *> m_instances;
        MainWindow *m_main_window;
        ImageCache* m_image_cache;
        DownloadManager* m_download_manager;

        /**
         * Loads the configuration settings for the application.
         */
        void LoadConfig();

        /**
         * Saves the current configuration settings.
         */
        void SaveConfig();

        /**
         * Loads all instances.
         */
        void LoadInstances();

        /**
         * Destroys the current instance and releases any associated resources.
         */
        void DestroyInstance(const Instance *instance) const;

        /**
         * Prepares everything UI-related.
         */
        void PrepareUI();

        void OnDownloadFinished(DownloadId id, const std::filesystem::path& file_path) const;

        static void UpdatePalette();

        /**
         * Creates all required directories.
         */
        static void CreatePaths();

        /**
         * Calculates the initial window size for the main window.
         */
        static void GetInitialWindowSize(int &out_width, int &out_height);

    private:
        friend class MainWindow;
    };

    inline Application *GApp = nullptr;
}