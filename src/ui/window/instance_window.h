#pragma once

#include "base_window.h"
#include "instance/instance.h"

namespace Actinium
{
    class InstanceWindow : public BaseWindow
    {
        Q_OBJECT

    public:
        explicit InstanceWindow(Instance *instance, QWidget *parent = nullptr);

        [[nodiscard]] Instance *GetInstance() const
        {
            return m_instance;
        }

    private:
        Instance *m_instance;
    };
}