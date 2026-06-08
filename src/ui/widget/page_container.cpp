// ReSharper disable CppDFAMemoryLeak
#include "page_container.h"

#include <QPushButton>
#include <QVBoxLayout>

namespace Actinium
{
    PageContainer::PageContainer(const std::vector<Page*>& pages, QWidget* parent)
        : QWidget(parent)
        , m_pages(pages)
        , m_current_page(-1)
    {
        const auto layout = new QHBoxLayout(this);
        layout->setContentsMargins(0, 0, 0, 0);

        const auto button_container = new QWidget(this);
        const auto button_layout = new QVBoxLayout(button_container);
        button_container->setLayout(button_layout);
        button_container->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
        button_container->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
        button_container->setMinimumWidth(150);

        const auto page_container = new QWidget(this);
        layout->addWidget(button_container);
        layout->addWidget(page_container);

        int index = 0;

        for (const auto& page : m_pages)
        {
            const auto button_text = page->GetButtonText();
            const auto page_button = new QPushButton(button_text, this);
            page_button->setObjectName(button_text + "Button");

            button_layout->addWidget(page_button);

            connect(page_button, &QPushButton::clicked, page,
                [this, index]
                {
                    SetCurrentPage(index);
                });

            page->setParent(page_container);
            page->setVisible(false);
            index++;
        }

        button_layout->addStretch();

        SetCurrentPage(0);
    }

    void PageContainer::SetCurrentPage(const int index)
    {
        if (index < 0 || index >= m_pages.size())
        {
            return;
        }

        if (m_current_page >= 0 || m_current_page < m_pages.size())
        {
            m_pages.at(m_current_page)->setVisible(false);
        }

        m_pages.at(index)->setVisible(true);
        m_current_page = index;
    }
}
// ReSharper restore CppDFAMemoryLeak