#pragma once

#include <QAbstractItemView>

namespace Actinium
{
    class InstanceView : public QAbstractItemView
    {
        Q_OBJECT

    public:
        explicit InstanceView(QWidget *parent = nullptr);

        void scrollTo(const QModelIndex &index, ScrollHint hint) override;

        [[nodiscard]] QModelIndex indexAt(const QPoint &point) const override;

        [[nodiscard]] QRect visualRect(const QModelIndex &index) const override;

    signals:
        void SelectionChanged(const QModelIndex &index);

    protected:
        void updateGeometries() override;

        void paintEvent(QPaintEvent *event) override;

        void resizeEvent(QResizeEvent *event) override;

        void mouseMoveEvent(QMouseEvent *event) override;

        [[nodiscard]] int verticalOffset() const override;

        [[nodiscard]] int horizontalOffset() const override;

        [[nodiscard]] bool isIndexHidden(const QModelIndex &index) const override;

        QModelIndex moveCursor(CursorAction action, Qt::KeyboardModifiers modifiers) override;

        void setSelection(const QRect &rect, QItemSelectionModel::SelectionFlags command) override;

        [[nodiscard]] QRegion visualRegionForSelection(const QItemSelection &selection) const override;

    private:
        int m_items_per_row;
        QPoint m_mouse_position;

        void PaintInstances(QPainter& painter) const;

        void PaintWelcomeMessage(QPainter& painter) const;

        [[nodiscard]] int GetContentWidth() const;

        [[nodiscard]] int CalculateItemsPerRow() const;
    };
}