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
     * Paints the icon, covering the area that it should be in.
     *
     * Best works while also using `QPainter::setClipRect` before with the specified area.
     */
    void PaintIconCoveredInArea(QPainter* painter, const QIcon & icon, const QRect& area);

    /**
     * Creates a thread-safe callback function for QT objects.
     */
    template<typename QObjectType, typename Func>
    auto QueuedCallback(QObjectType *object, Func &&func, Qt::ConnectionType type = Qt::QueuedConnection)
    {
        QPointer<QObjectType> self(object);

        return [self, func = std::forward<Func>(func), type]<typename... Args>(Args &&...args) mutable
        {
            if (!self)
            {
                return;
            }

            QMetaObject::invokeMethod(
                self,
                [self, func, ... captured_args = std::forward<Args>(args)]() mutable
                {
                    if (!self)
                    {
                        return;
                    }

                    std::invoke(func, self, std::move(captured_args)...);
                },
                type);
        };
    }

    /**
     * Invokes a function on a Qt object on the UI thread.
     */
    template<typename QObjectType, typename Func, typename... Args>
    void Invoke(QPointer<QObjectType> self, Func &&func, Args &&...args, Qt::ConnectionType type = Qt::QueuedConnection)
    {
        if (self == nullptr)
        {
            return;
        }

        QMetaObject::invokeMethod(
            self,
            [self, func = std::forward<Func>(func), args = std::make_tuple(std::forward<Args>(args)...)]() mutable
            {
                if (self == nullptr)
                {
                    return;
                }

                std::apply(
                    [&](auto &&...captured_args)
                    {
                        std::invoke(func, std::move(captured_args)...);
                    },
                    std::move(args));
            },
            type);
    }

}