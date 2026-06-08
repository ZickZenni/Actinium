#pragma once

#include "instance/instance.h"
#include "ui/page/page.h"

namespace Actinium
{
    class InstanceModsPage : public Page
    {
    public:
        explicit InstanceModsPage(Instance* instance, QWidget* parent = nullptr);

        QString GetButtonText() override;

    private:
        Instance* m_instance;
    };
}