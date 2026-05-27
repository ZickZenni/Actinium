#include "instance_window.h"

#include <QCloseEvent>

namespace Actinium
{
    InstanceWindow::InstanceWindow(Instance* instance, QWidget* parent)
        : QMainWindow(parent)
        , m_instance(instance)
    {
        setAttribute(Qt::WA_DeleteOnClose);
        setWindowTitle(QString::fromStdString(instance->name));
    }

    void InstanceWindow::closeEvent(QCloseEvent* event)
    {
        emit IsClosing();
        event->accept();
    }
}