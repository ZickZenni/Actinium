#pragma once

#include "provider.h"

#include <nlohmann/json.hpp>

namespace Actinium
{
    class GameBananaProvider : public Provider
    {
    public:
        explicit GameBananaProvider(int game_id);

        void GetMods(ResponseCallback<SearchResponse> callback) override;

    private:
        int m_game_id;

        static std::optional<Mod> ParseMod(const nlohmann::json &json);
    };
}