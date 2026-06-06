#pragma once

#include <QApplication>
#include <QStyleOptionViewItem>
#include <QTextLayout>
#include <filesystem>

#define qstr(x) x.toStdString()
#define strq(x) QString::fromStdString(x)

namespace Actinium::QT
{
    /**
     * Computes the size required to display text within a specific view item.
     */
    QSize ViewItemTextSize(const QStyleOptionViewItem &option);

    /**
     * Calculates the width and height used to draw the text.
     */
    void ViewItemTextLayout(QTextLayout &layout, int lineWidth, qreal &height, qreal &used_width);

    /**
     * Retrieves the correct style when option has one assigned (?)
     */
    QStyle *GetCorrectStyle(const QStyleOptionViewItem &option);

    /**
     * Creates a url that directs to a file.
     */
    QUrl CreateFileUrl(const std::filesystem::path &path);

    /**
     * Converts a QStringList to a std::string vector.
     */
    std::vector<std::string> ToStdVector(const QStringList &list);
}