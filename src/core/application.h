#pragma once

#include "game/game.h"
#include "instance/instance.h"
#include "ui/window/main_window.h"

#include <QApplication>
#include <filesystem>

namespace Actinium
{
    constexpr std::array GAMES = {
        Game("genshin_impact", "Genshin Impact"),
        Game("honkai_star_rail", "Honkai Star Rail"),
        Game("zenless_zone_zero", "Zenless Zone Zero"),
    };

    class Application final : public QApplication
    {
    public:
        Application(int argc, char *argv[]);
        ~Application() override;

        int Run() const;

        /**
         * Creates a new instance for a game.
         */
        void CreateInstance(const std::string &name, const std::string &game_id);

        /**
         * Retrieves the appdata path for the application.
         */
        static std::filesystem::path GetAppDataPath();

        /**
         * Retrieves a support game by its identifier.
         */
        static const Game* GetGameById(const std::string& id);

    private:
        std::vector<Instance *> m_instances;

        MainWindow *m_main_window;

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
    };

    inline Application *GApp = nullptr;
}