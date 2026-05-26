#pragma once

#include <QApplication>
#include <QStyleOptionViewItem>
#include <QTextLayout>

namespace Actinium
{
    class QTUtils
    {
    public:
        QTUtils() = delete;

        static QSize ViewItemTextSize(const QStyleOptionViewItem &option);

        static void ViewItemTextLayout(QTextLayout &layout, int lineWidth, qreal &height, qreal &used_width);

        static QStyle *GetCorrectStyle(const QStyleOptionViewItem &option);
    };
}