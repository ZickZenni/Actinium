#pragma once

#include <QAbstractItemView>

namespace Actinium
{
    class InstanceView : public QAbstractItemView
    {
        Q_OBJECT

    public:
        explicit InstanceView(QWidget *parent = nullptr);

        void scrollTo(const QModelIndex &index, ScrollHint hint = EnsureVisible) override;

        QModelIndex indexAt(const QPoint &point) const override;

        QRect visualRect(const QModelIndex &index) const override;

    signals:
        void selectionChanged(const QModelIndex &index);

    protected:
        void updateGeometries() override;

        void paintEvent(QPaintEvent *event) override;

        void resizeEvent(QResizeEvent *event) override;

        void mouseMoveEvent(QMouseEvent *event) override;

        int verticalOffset() const override;

        int horizontalOffset() const override;

        bool isIndexHidden(const QModelIndex &index) const override;

        QModelIndex moveCursor(CursorAction action, Qt::KeyboardModifiers modifiers) override;

        void setSelection(const QRect &rect, QItemSelectionModel::SelectionFlags command) override;

        QRegion visualRegionForSelection(const QItemSelection &selection) const override;

    private:
        int m_items_per_row;
        QPoint m_mouse_position;

        int CalculateItemsPerRow() const;
    };
}