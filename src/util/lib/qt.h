#pragma once

#include <QPointer>
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

    /**
     * Creates a thread-safe callback function for QT objects.
     */
    template<typename QObjectType, typename Func> auto QueuedCallback(QObjectType *object, Func &&func)
    {
        QPointer<QObjectType> self(object);

        return [self, func = std::forward<Func>(func)]<typename... T>(T &&...args) mutable
        {
            if (self == nullptr)
            {
                return;
            }

            QMetaObject::invokeMethod(
                self,
                [self, func, ... captured_args = std::forward<T>(args)]() mutable
                {
                    if (self == nullptr)
                    {
                        return;
                    }

                    std::invoke(func, self, std::move(captured_args)...);
                },
                Qt::QueuedConnection);
        };
    }
}