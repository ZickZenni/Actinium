#pragma once

#include "provider/provider.h"

#include <QAbstractListModel>

namespace Actinium
{
    class ProviderModModel : public QAbstractListModel
    {
    public:
        explicit ProviderModModel(QObject *parent = nullptr);

        void SetResponse(const Provider::SearchResponse& response);

        QModelIndex index(int row, int column, const QModelIndex& parent) const override;

        [[nodiscard]] int rowCount(const QModelIndex &parent) const override;

        [[nodiscard]] QVariant data(const QModelIndex &index, int role) const override;

    private:
        std::optional<Provider::SearchResponse> m_search_response;
    };
}