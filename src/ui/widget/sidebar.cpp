#include "sidebar.h"

namespace Actinium
{
    SideBar::SideBar(const QString& title, QWidget* parent)
        : QToolBar(title, parent)
    {
        setOrientation(Qt::Vertical);
    }

    SideBar::SideBar(QWidget* parent)
        : QToolBar(parent)
    {
        setOrientation(Qt::Vertical);
    }
}