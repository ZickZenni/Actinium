#include "instance_settings_page.h"

#include <QPushButton>

namespace Actinium
{
    InstanceSettingsPage::InstanceSettingsPage(Instance* instance, QWidget* parent)
        : Page(parent)
        , m_instance(instance)
    {
        setObjectName("InstanceSettingsPage");

        const auto test_button = new QPushButton("Settings", this);
    }

    QString InstanceSettingsPage::GetButtonText()
    {
        return "Settings";
    }
}