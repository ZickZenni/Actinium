#include "game_banana_provider.h"

#include "core/logger.h"
#include "util/lib/json.h"

#include <cpr/api.h>
#include <format>

namespace Actinium
{
    constexpr std::string_view BASE_API_URL = "https://gamebanana.com/apiv11";

    GameBananaProvider::GameBananaProvider(const int game_id)
        : m_game_id(game_id)
    {
    }

    void GameBananaProvider::GetMods(ResponseCallback<SearchResponse> callback)
    {
        const auto url = std::format("{}/Game/{}/Subfeed?_csvModelInclusions=Mod", BASE_API_URL, m_game_id);

        Logger::Debug("provider::game_provider", "Sending API request (game_id={}, url={})", m_game_id, url);

        auto cpr_callback = [callback](const cpr::Response& response)
        {
            if (response.status_code != 200)
            {
                Logger::Error("provider::game_provider", "Failed to get mods (status_code={})", response.status_code);
                callback({});
                return;
            }

            if (response.text.empty())
            {
                Logger::Error("provider::game_provider", "Received an empty response from request");
                callback({});
                return;
            }

            const auto json = nlohmann::json::parse(response.text, nullptr, false);

            if (json.is_discarded() || !json.contains("_aMetadata") || !json.contains("_aRecords"))
            {
                Logger::Error("provider::game_provider", "Received an invalid json response from request (data={})",
                    response.text);
                callback({});
                return;
            }

            const auto& metadata = json.at("_aMetadata");
            const auto& mods = json.at("_aRecords");
            const auto record_count = nlohmann::try_get<uint32_t>(metadata, "_nRecordCount");

            SearchResponse search_response {};
            search_response.total_count = record_count.value_or(0);

            if (mods.is_array())
            {
                for (const auto& mod : mods)
                {
                    const auto parsed_mod = ParseMod(mod);

                    if (parsed_mod.has_value())
                    {
                        search_response.mods.push_back(std::move(parsed_mod.value()));
                    }
                }
            }

            callback(search_response);
        };

        cpr::GetCallback(cpr_callback, cpr::Url {url});
    }

    std::optional<Provider::Mod> GameBananaProvider::ParseMod(const nlohmann::json& json)
    {
        JSON_REQUIRE_VAR_DEBUG(json, "_idRow", uint32_t, std::nullopt, "provider::game_banana", "Failed to parse mod")
        JSON_REQUIRE_VAR_DEBUG(
            json, "_sName", std::string, std::nullopt, "provider::game_banana", "Failed to parse mod")

        return Mod {.id = json.at("_idRow"), .name = json.at("_sName")};
    }
}