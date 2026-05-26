#pragma once

#include <QToolBar>

namespace Actinium
{
    class SideBar : public QToolBar
    {
    public:
        explicit SideBar(const QString &title, QWidget *parent = nullptr);
        explicit SideBar(QWidget *parent = nullptr);
    };
}