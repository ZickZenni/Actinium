#pragma once

#include <QMainWindow>

namespace Actinium
{
    class BaseWindow : public QMainWindow
    {
        Q_OBJECT

    public:
        explicit BaseWindow(QWidget *parent = nullptr);

    signals:
        void closed();

    protected:
        virtual void OnClose()
        {
        }

        void closeEvent(QCloseEvent *event) override;
    };
}