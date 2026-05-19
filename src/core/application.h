#pragma once

#include "game/game.h"
#include "ui/window/main_window.h"

#include <QApplication>

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

    private:
        MainWindow *m_main_window;

        static void GetInitialWindowSize(int &out_width, int &out_height);
    };

    inline Application *GApp = nullptr;
}