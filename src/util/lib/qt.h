#pragma once

#include <QApplication>
#include <QStyleOptionViewItem>
#include <QTextLayout>
#include <filesystem>

#define qstr(x) x.toStdString()
#define strq(x) QString::fromStdString(x)

namespace Actinium::QT
{
    QSize ViewItemTextSize(const QStyleOptionViewItem &option);

    void ViewItemTextLayout(QTextLayout &layout, int lineWidth, qreal &height, qreal &used_width);

    QStyle *GetCorrectStyle(const QStyleOptionViewItem &option);

    QUrl CreateFileUrl(const std::filesystem::path &path);
}