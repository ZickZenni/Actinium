#include "application.h"

#include <QMessageBox>
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

        m_main_window = new MainWindow(this);

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

    void Application::CreateInstance(const std::string& name, const std::string& game_id)
    {
        const auto game = std::ranges::find_if(GAMES,
            [&game_id](const auto& v)
            {
                return v.GetId() == game_id;
            });

        if (game == GAMES.end())
        {
            QMessageBox::critical(m_main_window, "Internal Error", "The selected game was not found internally.", QMessageBox::Close);
            return;
        }

        const auto instance = new Instance(game, name);
        m_instances.push_back(instance);

        std::cout << "[Application::CreateInstance] Instance created" << std::endl;
    }

    void Application::GetInitialWindowSize(int& out_width, int& out_height)
    {
        const auto primary_screen = primaryScreen();
        const auto screen_size = primary_screen->size();

        out_width = std::min(screen_size.width() - 20, 1280);
        out_height = std::min(screen_size.height() - 90, 720);
    }
}