#include "application.h"

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
}