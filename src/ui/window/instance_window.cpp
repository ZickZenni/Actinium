#include "instance_window.h"

#include "ui/page/instance/instance_mods_page.h"
#include "ui/page/instance/instance_settings_page.h"
#include "ui/widget/page_container.h"

#include <QCloseEvent>

namespace Actinium
{
    InstanceWindow::InstanceWindow(Instance* instance, QWidget* parent)
        : QMainWindow(parent)
        , m_instance(instance)
    {
        setAttribute(Qt::WA_DeleteOnClose);
        setWindowTitle(QString::fromStdString(instance->name));

        const auto container
            = new PageContainer({new InstanceModsPage(instance, this), new InstanceSettingsPage(instance, this)}, this);
        setCentralWidget(container);
        setContentsMargins(0, 0, 0, 0);
    }

    void InstanceWindow::closeEvent(QCloseEvent* event)
    {
        emit IsClosing();
        event->accept();
    }
}