#pragma once

#include <QMainWindow>
#include <QPushButton>

namespace Actinium
{
    class Application;

    class MainWindow final : public QMainWindow
    {
        Q_OBJECT

    public:
        explicit MainWindow(Application* app, QWidget *widget = nullptr);

    private:
        Application* m_app;

        QWidget* m_central_widget;

        void CreateInstance();
    };
}