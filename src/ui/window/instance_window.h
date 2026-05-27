#pragma once
#include "instance/instance.h"

#include <QMainWindow>

namespace Actinium
{
    class InstanceWindow : public QMainWindow
    {
        Q_OBJECT

    public:
        explicit InstanceWindow(Instance *instance, QWidget *parent = nullptr);

        [[nodiscard]] Instance *GetInstance() const
        {
            return m_instance;
        }

    signals:
        void IsClosing();

    protected:
        void closeEvent(QCloseEvent *event) override;

    private:
        Instance *m_instance;
    };
}