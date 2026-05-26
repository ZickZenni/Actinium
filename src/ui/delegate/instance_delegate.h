#pragma once

#include <QStyledItemDelegate>

namespace Actinium
{
    class InstanceDelegate : public QStyledItemDelegate
    {
        Q_OBJECT

    public:
        static constexpr int ITEM_SIZE = 100;
        static constexpr int ICON_SIZE = 48;

        /**
         * TODO: Remove this test icon and instead display the icon of the instance we are drawing.
         */
        QIcon icon;

        explicit InstanceDelegate(QObject *parent = nullptr);

        void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

        QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    };
}