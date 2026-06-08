#pragma once

#include "instance/instance.h"
#include "ui/page/page.h"

namespace Actinium
{
    class InstanceSettingsPage : public Page
    {
    public:
        explicit InstanceSettingsPage(Instance* instance, QWidget* parent = nullptr);

        QString GetButtonText() override;

    private:
        Instance* m_instance;
    };
}