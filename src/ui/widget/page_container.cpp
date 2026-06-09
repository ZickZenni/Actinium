// ReSharper disable CppDFAMemoryLeak
#include "page_container.h"

#include "core/logger.h"

#include <QPushButton>
#include <QVBoxLayout>

namespace Actinium
{
    constexpr auto BUTTON_PROPERTY__PAGE_INDEX_KEY = "pageIndex";

    PageContainer::PageContainer(const std::vector<Page*>& pages, QWidget* parent)
        : QWidget(parent)
        , m_pages(pages)
    {
        const auto button_layout = new QVBoxLayout();
        m_page_layout = new QStackedLayout();

        for (auto index = 0; index < m_pages.size(); ++index)
        {
            const auto page = m_pages[index];
            page->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Expanding);

            m_page_layout->addWidget(page);

            const auto button_text = page->GetButtonText();
            const auto page_button = new QPushButton(button_text, this);
            page_button->setObjectName(button_text + "Button");
            page_button->setProperty(BUTTON_PROPERTY__PAGE_INDEX_KEY, index);
            button_layout->addWidget(page_button);

            connect(page_button, &QPushButton::clicked, this, &PageContainer::OnPageButtonClicked);
        }

        button_layout->addStretch();

        const auto m_layout = new QGridLayout();
        m_layout->addLayout(button_layout, 0, 0, 1, 1);
        m_layout->addLayout(m_page_layout, 0, 1, 1, 1);
        m_layout->setColumnStretch(1, 4);
        m_layout->setContentsMargins(0, 0, 0, 0);

        setLayout(m_layout);
    }

    void PageContainer::OnPageButtonClicked() const
    {
        const auto button = qobject_cast<QPushButton*>(sender());

        if (button == nullptr)
        {
            return;
        }

        const auto index = button->property(BUTTON_PROPERTY__PAGE_INDEX_KEY).toInt();

        Logger::Debug("ui::widget::page_container", "QPushButton::clicked event received (index={})", index);
        SetCurrentPage(index);
    }

    void PageContainer::SetCurrentPage(const int index) const
    {
        const auto& size_index = static_cast<std::size_t>(index);

        if (index < 0 || size_index >= m_page_layout->count())
        {
            return;
        }

        m_page_layout->setCurrentIndex(index);
    }
} 