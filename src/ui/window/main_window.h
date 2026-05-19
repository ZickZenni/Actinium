#pragma once

#include <QMainWindow>
#include <QPushButton>

namespace Actinium
{
    class MainWindow final : public QMainWindow
    {
        Q_OBJECT

    public:
        explicit MainWindow(QWidget *widget = nullptr);

    private:
        QWidget* m_central_widget;

        void CreateInstance();
    };
}