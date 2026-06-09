#include "base_window.h"

#include <QCloseEvent>

namespace Actinium
{
    BaseWindow::BaseWindow(QWidget* parent)
        : QMainWindow(parent)
    {
    }

    void BaseWindow::closeEvent(QCloseEvent* event)
    {
        OnClose();
        emit closed();
        event->accept();
    }
}