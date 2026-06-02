#include "instance_delegate.h"

#include "util/lib/qt.h"

#include <QApplication>
#include <QPainter>

namespace Actinium
{
    InstanceDelegate::InstanceDelegate(QObject* parent)
        : QStyledItemDelegate(parent)
        , icon("./resources/instances/test.png")
    {
    }

    void InstanceDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
    {
        QStyleOptionViewItem opt = option;
        initStyleOption(&opt, index);

        painter->save();
        painter->setClipRect(opt.rect);

        opt.features |= QStyleOptionViewItem::WrapText;
        opt.text = index.data().toString();
        opt.icon = icon;
        opt.textElideMode = Qt::ElideRight;
        opt.displayAlignment = Qt::AlignTop | Qt::AlignHCenter;

        const auto style = QT::GetCorrectStyle(opt);
        const auto text_margin = style->pixelMetric(QStyle::PM_FocusFrameHMargin, nullptr, opt.widget) + 1;

        auto text_rect = opt.rect;
        text_rect.adjust(text_margin, ICON_SIZE + text_margin + 5, -text_margin, 0);

        {
            const auto selected = option.state & QStyle::State_Selected;
            const auto hovered = option.state & QStyle::State_MouseOver;

            auto bg_rect = opt.rect;
            bg_rect.adjust(0, ICON_SIZE + 5, 0, 0);

            if (selected || hovered)
            {
                auto color = option.palette.color(QPalette::Highlight);

                if (hovered)
                {
                    color.setAlpha(color.alpha() / (selected ? 2 : 3));
                }

                painter->fillRect(bg_rect, QBrush(color));
            }
            else
            {
                auto color = option.palette.color(QPalette::Shadow);
                color.setAlpha(50);

                painter->fillRect(bg_rect, QBrush(color));
            }
        }

        {
            auto mode = QIcon::Normal;

            if (!(opt.state & QStyle::State_Enabled))
            {
                mode = QIcon::Disabled;
            }
            else if (opt.state & QStyle::State_Selected)
            {
                mode = QIcon::Selected;
            }

            const auto state = opt.state & QStyle::State_Open ? QIcon::On : QIcon::Off;

            opt.rect.setHeight(ICON_SIZE);
            opt.icon.paint(painter, opt.rect, Qt::AlignCenter, mode, state);
        }

        QTextOption text_option;
        text_option.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
        text_option.setTextDirection(opt.direction);
        text_option.setAlignment(QStyle::visualAlignment(opt.direction, opt.displayAlignment));

        QTextLayout text_layout;
        text_layout.setTextOption(text_option);
        text_layout.setFont(opt.font);
        text_layout.setText(opt.text);

        qreal width, height;
        QT::ViewItemTextLayout(text_layout, text_rect.width(), height, width);

        {
            const int line_count = text_layout.lineCount();
            const auto layout_rect = QStyle::alignedRect(
                opt.direction, opt.displayAlignment, QSize(text_rect.width(), static_cast<int>(height)), text_rect);
            const auto position = layout_rect.topLeft();

            for (auto i = 0; i < line_count; ++i)
            {
                const auto line = text_layout.lineAt(i);
                line.draw(painter, position);
            }
        }

        painter->restore();
    }

    QSize InstanceDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
    {
        QStyleOptionViewItem opt = option;
        initStyleOption(&opt, index);

        opt.features |= QStyleOptionViewItem::WrapText;
        opt.text = index.data().toString();
        opt.textElideMode = Qt::ElideRight;
        opt.displayAlignment = Qt::AlignTop | Qt::AlignHCenter;

        const auto style = QT::GetCorrectStyle(opt);
        const auto text_margin = style->pixelMetric(QStyle::PM_FocusFrameHMargin, &option, opt.widget) + 1;
        const auto text_size = QT::ViewItemTextSize(opt);
        const auto height = ICON_SIZE + text_margin * 2 + 5 + text_size.height();

        return { ITEM_SIZE, height };
    }
}