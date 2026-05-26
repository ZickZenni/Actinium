#pragma once

#include "instance/instance.h"

#include <QAbstractItemModel>

namespace Actinium
{
    class InstanceListModel : public QAbstractListModel
    {
    public:
        explicit InstanceListModel(std::vector<Instance *> *instance_list, QObject *parent = nullptr);

        QVariant data(const QModelIndex &index, int role) const override;

        int rowCount(const QModelIndex &parent = QModelIndex()) const override;

        QModelIndex index(int row, int column = 0, const QModelIndex &parent = QModelIndex()) const override;

    private:
        std::vector<Instance *> *m_instance_list;
    };
}