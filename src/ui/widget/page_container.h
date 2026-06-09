#pragma once

#include "ui/page/page.h"

#include <QStackedLayout>
#include <QWidget>

namespace Actinium
{
    class PageContainer : public QWidget
    {
        Q_OBJECT

    public:
        explicit PageContainer(const std::vector<Page *> &pages, QWidget *parent = nullptr);

    private:
        std::vector<Page *> m_pages;
        QStackedLayout *m_page_layout;

        void OnPageButtonClicked() const;

        /**
         * Sets the current page in the container.
         */
        void SetCurrentPage(int index) const;
    };
}