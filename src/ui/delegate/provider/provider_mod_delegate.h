#pragma once

#include <QStyledItemDelegate>

namespace Actinium
{
    class ProviderModDelegate : public QStyledItemDelegate
    {
        Q_OBJECT

    public:
        explicit ProviderModDelegate(QObject *parent = nullptr);

        void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

        QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    };
}