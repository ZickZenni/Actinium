#include "application.h"

#include <QScreen>

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

        m_main_window = new MainWindow();

        {
            int width, height;
            GetInitialWindowSize(width, height);

            m_main_window->resize(width, height);
        }

        m_main_window->show();
    }

    Application::~Application()
    {
        delete m_main_window;
    }

    int Application::Run() const
    {
        return exec();
    }

    void Application::GetInitialWindowSize(int& out_width, int& out_height)
    {
        const auto primary_screen = primaryScreen();
        const auto screen_size = primary_screen->size();

        out_width = std::min(screen_size.width() - 20, 1280);
        out_height = std::min(screen_size.height() - 90, 720);
    }
}