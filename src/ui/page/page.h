#pragma once

#include <QWidget>

namespace Actinium
{
    class Page : public QWidget
    {
        Q_OBJECT

    public:
        explicit Page(QWidget *parent = nullptr)
            : QWidget(parent)
        {
        }

        /**
         * Retrieves the text for the button.
         */
        virtual QString GetButtonText() = 0;
    };
}