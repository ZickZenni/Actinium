#pragma once

#include <QApplication>
#include <QStyleOptionViewItem>
#include <QTextLayout>
#include <filesystem>

#define qstr(x) x.toStdString()
#define strq(x) QString::fromStdString(x)

namespace Actinium
{
    class QTUtils
    {
    public:
        QTUtils() = delete;

        static QSize ViewItemTextSize(const QStyleOptionViewItem &option);

        static void ViewItemTextLayout(QTextLayout &layout, int lineWidth, qreal &height, qreal &used_width);

        static QStyle *GetCorrectStyle(const QStyleOptionViewItem &option);

        static QUrl CreateFileUrl(const std::filesystem::path &path);
    };
}