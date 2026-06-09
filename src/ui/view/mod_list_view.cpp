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

        return 3;
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
                    case 1:
                        return QString::fromStdString(mod.name);
                    case 2:
                        return QString::fromStdString("Unknown");
                    default:
                        return {};
                }
            }
            case Qt::CheckStateRole:
            {
                if (index.column() == 0)
                {
                    return mod.disabled ? Qt::Unchecked : Qt::Checked;
                }

                return {};
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
                    return "Enabled";
                case 1:
                    return "Name";
                case 2:
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

    Qt::ItemFlags ModListModel::flags(const QModelIndex& index) const
    {
        const auto default_flags = QAbstractItemModel::flags(index);

        if (index.column() == 0)
        {
            return default_flags | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled;
        }

        return default_flags;
    }

    bool ModListModel::setData(const QModelIndex& index, const QVariant& value, const int role)
    {
        if (!index.isValid())
        {
            return false;
        }

        if (role == Qt::CheckStateRole && index.column() == 0)
        {
            const auto state = static_cast<Qt::CheckState>(value.toInt());
            m_instance->SetModEnabled(m_instance->GetMods().at(index.row()), state == Qt::Checked);

            emit dataChanged(index, index, {Qt::CheckStateRole});
            return true;
        }

        return false;
    }

    ModListView::ModListView(QWidget* parent)
        : QTreeView(parent)
    {
    }
}