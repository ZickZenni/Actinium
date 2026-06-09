#include "instance_mods_page.h"

#include <QPushButton>

namespace Actinium
{
    InstanceModsPage::InstanceModsPage(Instance* instance, QWidget* parent)
        : Page(parent)
        , m_instance(instance)
    {
        setObjectName("InstanceModsPage");

        const auto test_button = new QPushButton("Mods", this);
    }

    QString InstanceModsPage::GetButtonText()
    {
        return "Mods";
    }
}