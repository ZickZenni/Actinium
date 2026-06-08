#include "instance_mods_page.h"

namespace Actinium
{
    InstanceModsPage::InstanceModsPage(Instance* instance, QWidget* parent)
        : Page(parent)
        , m_instance(instance)
    {
        setObjectName("InstanceModsPage");
    }

    QString InstanceModsPage::GetButtonText()
    {
        return "Mods";
    }
}