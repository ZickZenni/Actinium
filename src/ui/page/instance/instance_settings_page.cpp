#include "instance_settings_page.h"

namespace Actinium
{
    InstanceSettingsPage::InstanceSettingsPage(Instance* instance, QWidget* parent)
        : Page(parent)
        , m_instance(instance)
    {
        setObjectName("InstanceSettingsPage");
    }

    QString InstanceSettingsPage::GetButtonText()
    {
        return "Settings";
    }
}