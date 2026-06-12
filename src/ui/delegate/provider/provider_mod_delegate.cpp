#include "provider_mod_delegate.h"

#include "core/application.h"
#include "util/lib/qt.h"

#include <QPainter>
#include <QTextLayout>

namespace Actinium
{
    constexpr auto ITEM_HEIGHT = 60;
    constexpr auto ICON_ASPECT_RATIO = 16.0f / 9.0f;
    constexpr auto ICON_MARGIN = 4;
    constexpr auto TEXT_MARGIN_LEFT = 6;

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

        const auto icon_width = opt.rect.height() * ICON_ASPECT_RATIO;

        if (!opt.icon.isNull())
        {
            const auto area = QRect(opt.rect.x() + ICON_MARGIN, opt.rect.y() + ICON_MARGIN,
                icon_width - ICON_MARGIN * 2, opt.rect.height() - ICON_MARGIN * 2);

            painter->setClipRect(area);
            QT::PaintIconCoveredInArea(painter, opt.icon, area);
            painter->setClipRect(opt.rect);
        }

        const auto style = QT::GetCorrectStyle(opt);
        style->drawItemText(painter,
            opt.rect.adjusted(icon_width + TEXT_MARGIN_LEFT, 0, icon_width + TEXT_MARGIN_LEFT, 0),
            Qt::AlignVCenter | Qt::AlignLeft, opt.palette, true, opt.text);

        painter->restore();
    }

    QSize ProviderModDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
    {
        return {0, ITEM_HEIGHT};
    }
}