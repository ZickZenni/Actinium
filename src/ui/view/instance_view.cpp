#include "instance_view.h"

#include "ui/delegate/instance_delegate.h"

#include <QPainter>
#include <QScrollBar>
#include <qevent.h>

namespace Actinium
{
    constexpr int ITEM_MARGIN = 12;
    constexpr int ITEM_SPACING = 12;

    InstanceView::InstanceView(QWidget* parent)
        : QAbstractItemView(parent)
        , m_items_per_row(0)
    {
        setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        setAutoFillBackground(false);
        viewport()->setAutoFillBackground(false);
        viewport()->setMouseTracking(true);
    }

    void InstanceView::scrollTo(const QModelIndex& index, ScrollHint hint)
    {
    }

    QModelIndex InstanceView::indexAt(const QPoint& point) const
    {
        for (int i = 0; i < model()->rowCount(); ++i)
        {
            const auto index = model()->index(i, 0);

            if (visualRect(index).contains(point))
            {
                return index;
            }
        }

        return {};
    }

    QRect InstanceView::visualRect(const QModelIndex& index) const
    {
        if (!index.isValid() || isIndexHidden(index) || index.column() > 0)
        {
            return QRect();
        }

        QStyleOptionViewItem option;
        initViewItemOption(&option);

        const auto row = index.row();
        const auto col = row / m_items_per_row;
        const auto visual_row = row - col * m_items_per_row;
        const auto size = itemDelegate()->sizeHint(option, index);

        QRect out;
        out.setTop(ITEM_MARGIN + col * (100 + ITEM_SPACING));
        out.setLeft(ITEM_MARGIN + visual_row * size.width() + visual_row * ITEM_SPACING);
        out.setSize(size);

        return out;
    }

    void InstanceView::updateGeometries()
    {
        viewport()->update();
    }

    void InstanceView::paintEvent(QPaintEvent* event)
    {
        QPainter painter(viewport());

        QStyleOptionViewItem option;
        initViewItemOption(&option);
        option.widget = this;

        for (auto i = 0; i < model()->rowCount(); ++i)
        {
            const auto index = model()->index(i, 0);
            const auto flags = index.flags();
            const auto is_selectable = flags & Qt::ItemIsSelectable;

            if (isIndexHidden(index))
            {
                continue;
            }

            option.rect = visualRect(index);
            option.features |= QStyleOptionViewItem::WrapText;

            /**
             * Selected state.
             */
            if (is_selectable && selectionModel()->isSelected(index))
            {
                option.state |= selectionModel()->isSelected(index) ? QStyle::State_Selected : QStyle::State_None;
            }
            else
            {
                option.state &= ~QStyle::State_Selected;
            }

            /**
             * Hover state.
             */
            if (is_selectable && option.rect.contains(m_mouse_position, true))
            {
                option.state |= QStyle::State_MouseOver;
            }
            else
            {
                option.state &= ~QStyle::State_MouseOver;
            }

            option.state |= index == currentIndex() ? QStyle::State_HasFocus : QStyle::State_None;

            if (!(flags & Qt::ItemIsEnabled))
            {
                option.state &= ~QStyle::State_Enabled;
            }

            itemDelegate()->paint(&painter, option, index);
        }
    }

    void InstanceView::resizeEvent(QResizeEvent* event)
    {
        const auto new_items_per_row = CalculateItemsPerRow();

        if (new_items_per_row != m_items_per_row)
        {
            m_items_per_row = new_items_per_row;
            updateGeometries();
        }
    }

    void InstanceView::mouseMoveEvent(QMouseEvent* event)
    {
        m_mouse_position = event->pos();

        const auto index = indexAt(m_mouse_position);

        if (index.isValid())
        {
            setCursor(Qt::PointingHandCursor);
            update();
        }
        else
        {
            setCursor(Qt::ArrowCursor);
        }

        QAbstractItemView::mouseMoveEvent(event);
    }

    int InstanceView::verticalOffset() const
    {
        return verticalScrollBar()->value();
    }

    int InstanceView::horizontalOffset() const
    {
        return horizontalScrollBar()->value();
    }

    bool InstanceView::isIndexHidden(const QModelIndex& index) const
    {
        return false;
    }

    QModelIndex InstanceView::moveCursor(CursorAction action, Qt::KeyboardModifiers modifiers)
    {
        return {};
    }

    void InstanceView::setSelection(const QRect& rect, QItemSelectionModel::SelectionFlags command)
    {
        for (int i = 0; i < model()->rowCount(); ++i)
        {
            const auto index = model()->index(i, 0);
            const auto item_rect = visualRect(index);

            if (item_rect.intersects(rect))
            {
                selectionModel()->select(index, command);
                update(item_rect.translated(-QPoint(horizontalOffset(), verticalOffset())));
            }
        }
    }

    QRegion InstanceView::visualRegionForSelection(const QItemSelection& selection) const
    {
        QRegion region;

        for (const auto& range : selection)
        {
            const auto start_row = range.top();
            const auto end_row = range.bottom();

            for (auto row = start_row; row <= end_row; ++row)
            {
                const auto start_column = range.left();
                const auto end_column = range.right();

                for (auto column = start_column; column <= end_column; ++column)
                {
                    const auto index = model()->index(row, column, rootIndex());
                    region += visualRect(index);
                }
            }
        }

        return region;
    }

    int InstanceView::CalculateItemsPerRow() const
    {
        const auto content_width = width() - ITEM_MARGIN * 2;
        const auto items_per_row
            = static_cast<int>(qFloor(content_width) / static_cast<qreal>(InstanceDelegate::ITEM_SIZE + ITEM_MARGIN));

        return std::clamp(items_per_row, 1, INT32_MAX);
    }
}