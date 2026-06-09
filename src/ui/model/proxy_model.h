#pragma once

#include <QSortFilterProxyModel>

namespace Actinium
{
    class ProxyModel : public QSortFilterProxyModel
    {
        Q_OBJECT

    public:
        explicit ProxyModel(QObject *parent = nullptr)
            : QSortFilterProxyModel(parent)
        {
        }

    protected:
        bool filterAcceptsColumn(const int source_column, const QModelIndex &source_parent) const override
        {
            return QSortFilterProxyModel::filterAcceptsColumn(source_column, source_parent);
        }

        bool filterAcceptsRow(const int source_row, const QModelIndex &source_parent) const override
        {
            return QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent);
        }

        bool lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const override
        {
            return QSortFilterProxyModel::lessThan(source_left, source_right);
        }
    };
}