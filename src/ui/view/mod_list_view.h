#pragma once

#include "instance/instance.h"

#include <QTreeView>

namespace Actinium
{
    class ModListModel : public QAbstractItemModel
    {
        Q_OBJECT

    public:
        explicit ModListModel(Instance *instance, QWidget *parent = nullptr);

        QModelIndex index(int row, int column, const QModelIndex &parent) const override;

        int columnCount(const QModelIndex &parent) const override;

        QVariant data(const QModelIndex &index, int role) const override;

        QModelIndex parent(const QModelIndex &child) const override;

        QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

        int rowCount(const QModelIndex &parent) const override;

        Qt::ItemFlags flags(const QModelIndex& index) const override;

        bool setData(const QModelIndex& index, const QVariant& value, int role) override;

    private:
        Instance *m_instance;
    };

    class ModListView : public QTreeView
    {
        Q_OBJECT

    public:
        explicit ModListView(QWidget *parent = nullptr);
    };
}