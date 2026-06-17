#include "provider_mod_model.h"

#include "core/application.h"
#include "util/lib/qt.h"

namespace Actinium
{
    ProviderModModel::ProviderModModel(QObject* parent)
        : QAbstractListModel(parent)
    {
        connect(GApp->GetImageCache(), &ImageCache::OnDownloadFinished, this,
            [this](const std::string& file_url)
            {
                if (!m_search_response.has_value())
                {
                    return;
                }

                const auto rows = rowCount({});

                for (auto i = 0; i < rows; ++i)
                {
                    const auto index = ProviderModModel::index(i, 0, {});

                    if (!index.isValid())
                    {
                        continue;
                    }

                    const auto url = data(index, Qt::DecorationRole).toString();

                    if (url == strq(file_url))
                    {
                        emit dataChanged(index, index);
                        break;
                    }
                }
            });
    }

    void ProviderModModel::SetResponse(const Provider::SearchResponse& response)
    {
        beginResetModel();

        if (m_search_response.has_value())
        {
            m_search_response.value().mods.insert(m_search_response.value().mods.end(), response.mods.begin(), response.mods.end());
        }
        else
        {
            m_search_response = response;
        }

        endResetModel();
    }

    QModelIndex ProviderModModel::index(const int row, const int column, const QModelIndex& parent) const
    {
        if (parent.isValid() || !m_search_response.has_value())
        {
            return {};
        }

        return createIndex(row, column, m_search_response.value().mods.at(row).id);
    }

    int ProviderModModel::rowCount(const QModelIndex& parent) const
    {
        if (parent.isValid() || !m_search_response.has_value())
        {
            return 0;
        }

        return m_search_response.value().mods.size();
    }

    QVariant ProviderModModel::data(const QModelIndex& index, const int role) const
    {
        if (!index.isValid() || !m_search_response.has_value())
        {
            return {};
        }

        const auto& mod = m_search_response.value().mods.at(index.row());

        switch (role)
        {
            case Qt::DisplayRole:
                return QString::fromStdString(mod.name);
            case Qt::DecorationRole:
            {
                if (mod.preview_media.empty())
                {
                    return {};
                }

                const auto& preview_media = mod.preview_media.at(0);
                return strq(std::format("{}/{}", preview_media.base_url, preview_media.file));
            }
            default:
                break;
        }

        return {};
    }
}