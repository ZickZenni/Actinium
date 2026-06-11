#include "provider_mod_delegate.h"

#include "core/application.h"
#include "util/lib/qt.h"

#include <QPainter>
#include <QTextLayout>

namespace Actinium
{
    ProviderModDelegate::ProviderModDelegate(QObject* parent)
        : QStyledItemDelegate(parent)
    {
    }

    void ProviderModDelegate::paint(
        QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
    {
        QStyleOptionViewItem opt = option;
        initStyleOption(&opt, index);

        opt.text = index.data(Qt::DisplayRole).toString();
        opt.textElideMode = Qt::ElideRight;
        opt.displayAlignment = Qt::AlignVCenter | Qt::AlignLeft;

        painter->save();
        painter->setClipRect(opt.rect);

        if (option.state & QStyle::State_Selected)
        {
            const auto& color = option.palette.color(QPalette::Highlight);
            painter->fillRect(opt.rect, QBrush(color));
        }

        const auto icon_size = opt.rect.height();

        if (!opt.icon.isNull())
        {
            opt.icon.paint(painter, 0, opt.rect.y(), icon_size, icon_size);
        }

        const auto style = QT::GetCorrectStyle(opt);
        style->drawItemText(painter, opt.rect.adjusted(icon_size, 0, icon_size, 0), Qt::AlignVCenter | Qt::AlignLeft,
            opt.palette, true, opt.text);

        painter->restore();
    }

    QSize ProviderModDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
    {
        return {0, 72};
    }
}