#include "mod_list_view.h"

#include "core/logger.h"

namespace Actinium
{
    ModListModel::ModListModel(Instance* instance, QWidget* parent)
        : QAbstractItemModel(parent)
        , m_instance(instance)
    {
    }

    QModelIndex ModListModel::index(const int row, int column, const QModelIndex& parent) const
    {
        if (parent.isValid())
        {
            return {};
        }

        return createIndex(row, column);
    }

    int ModListModel::columnCount(const QModelIndex& parent) const
    {
        if (parent.isValid())
        {
            return 0;
        }

        return 2;
    }

    QVariant ModListModel::data(const QModelIndex& index, const int role) const
    {
        if (!index.isValid())
        {
            return {};
        }

        const auto& mod = m_instance->GetMods().at(index.row());

        switch (role)
        {
            case Qt::DisplayRole:
            {
                switch (index.column())
                {
                    case 0:
                        return QString::fromStdString(mod.name);
                    case 1:
                        return QString::fromStdString("Unknown");
                    default:
                        return {};
                }
            }
            default:
                return {};
        }
    }

    QModelIndex ModListModel::parent(const QModelIndex& child) const
    {
        return {};
    }

    QVariant ModListModel::headerData(const int section, const Qt::Orientation orientation, const int role) const
    {
        if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        {
            switch (section)
            {
                case 0:
                    return "Name";
                case 1:
                    return "Version";
                default:
                    break;
            }
        }

        return {};
    }

    int ModListModel::rowCount(const QModelIndex& parent) const
    {
        if (parent.isValid())
        {
            return 0;
        }

        return m_instance->GetMods().size();
    }

    ModListView::ModListView(QWidget* parent)
        : QTreeView(parent)
    {
    }
}