// ReSharper disable CppDFAMemoryLeak
#include "instance_mods_page.h"

#include "ui/window/download_mods_window.h"

#include <QHBoxLayout>
#include <QPushButton>

namespace Actinium
{
    InstanceModsPage::InstanceModsPage(Instance* instance, QWidget* parent)
        : Page(parent)
        , m_instance(instance)
    {
        setObjectName("InstanceModsPage");
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

        const auto layout = new QHBoxLayout(this);
        layout->setContentsMargins(0, 0, 0, 0);

        m_list_proxy_model = new ProxyModel(this);
        m_list_model = new ModListModel(instance, this);
        m_list_proxy_model->setSourceModel(m_list_model);

        m_list_view = new ModListView(this);
        m_list_view->setObjectName("ModListView");
        m_list_view->setModel(m_list_proxy_model);
        m_list_view->setAlternatingRowColors(true);
        m_list_view->setUniformRowHeights(true);
        m_list_view->setSortingEnabled(true);
        m_list_view->sortByColumn(1, Qt::AscendingOrder);
        m_list_view->setFrameShape(QFrame::NoFrame);

        layout->addWidget(m_list_view);

        const auto download_mods_button = new QPushButton("Download Mods", this);
        connect(download_mods_button, &QPushButton::clicked, this, &InstanceModsPage::OnDownloadModsClicked);

        layout->addWidget(download_mods_button);
    }

    QString InstanceModsPage::GetButtonText()
    {
        return "Mods";
    }

    void InstanceModsPage::OnDownloadModsClicked()
    {
        const auto window = new DownloadModsWindow(m_instance, this);
        window->show();
    }
}
// ReSharper restore CppDFAMemoryLeak