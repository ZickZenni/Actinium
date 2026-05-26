#include "instance_model.h"

namespace Actinium
{
    InstanceListModel::InstanceListModel(std::vector<Instance*>* instance_list, QObject* parent)
        : QAbstractListModel(parent)
        , m_instance_list(instance_list)
    {
    }

    QVariant InstanceListModel::data(const QModelIndex& index, const int role) const
    {
        if (!index.isValid() || index.parent().isValid())
        {
            return {};
        }

        const auto row = index.row();

        if (row < 0 || row >= m_instance_list->size())
        {
            return {};
        }

        const auto instance = m_instance_list->at(row);

        switch (role)
        {
            case Qt::DisplayRole:
                return QString::fromStdString(instance->name);
            default:
                break;
        }

        return {};
    }

    int InstanceListModel::rowCount(const QModelIndex& parent) const
    {
        return m_instance_list->size();
    }

    QModelIndex InstanceListModel::index(const int row, const int column, const QModelIndex& parent) const
    {
        if (row < 0 || row >= m_instance_list->size())
        {
            return {};
        }

        return createIndex(row, column, m_instance_list->at(row));
    }

    Instance* InstanceListModel::at(const int index) const
    {
        if (index < 0 || index >= m_instance_list->size())
        {
            return nullptr;
        }

        return m_instance_list->at(index);
    }
}