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
    constexpr auto TEXT_MARGIN_LEFT = 5;
    constexpr auto NAME_MARGIN_TOP = 7;

    ProviderModDelegate::ProviderModDelegate(QObject* parent)
        : QStyledItemDelegate(parent)
    {
    }

    void ProviderModDelegate::paint(
        QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
    {
        QStyleOptionViewItem opt = option;
        initStyleOption(&opt, index);

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
        const auto text_rect = opt.rect.adjusted(
            icon_width + TEXT_MARGIN_LEFT, NAME_MARGIN_TOP, icon_width + TEXT_MARGIN_LEFT, NAME_MARGIN_TOP);

        style->drawItemText(painter, text_rect, Qt::AlignTop | Qt::AlignLeft, opt.palette, true,
            index.data(Qt::DisplayRole).toString());

        painter->restore();
    }

    QSize ProviderModDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
    {
        return {0, ITEM_HEIGHT};
    }
}