#pragma once

#include <QMainWindow>

namespace Actinium
{
    class MainWindow final : public QMainWindow
    {
        Q_OBJECT

    public:
        explicit MainWindow(QWidget *widget = nullptr);
    };
}