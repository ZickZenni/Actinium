#pragma once

#include "main_window.h"

#include <QApplication>

namespace Actinium
{
    class Application final : public QApplication
    {
    public:
        Application(int argc, char *argv[]);
        ~Application() override;

        int Run() const;

    private:
        MainWindow *m_main_window;

        static void GetInitialWindowSize(int& out_width, int& out_height);
    };

    inline Application *GApp = nullptr;
}