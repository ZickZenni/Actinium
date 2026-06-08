#pragma once

#include "ui/page/page.h"

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
        int m_current_page;

        /**
         * Sets the current page in the container.
         */
        void SetCurrentPage(int index);
    };
}