#include "game_banana_provider.h"

#include "core/logger.h"
#include "util/lib/json.h"

#include <cpr/api.h>
#include <format>

namespace Actinium
{
    constexpr std::string_view BASE_API_URL = "https://gamebanana.com/apiv12";

    GameBananaProvider::GameBananaProvider(const int game_id)
        : m_game_id(game_id)
    {
    }

    void GameBananaProvider::GetMods(uint16_t page, ResponseCallback<SearchResponse> callback)
    {
        const auto url = std::format("{}/Game/{}/Subfeed?_csvModelInclusions=Mod&_nPage={}", BASE_API_URL, m_game_id, page + 1);

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

    void GameBananaProvider::GetMod(uint32_t mod_id, ResponseCallback<ModInfo> callback)
    {
        const auto url = std::format("{}/Mod/{}/ProfilePage", BASE_API_URL, mod_id);

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

            if (json.is_discarded())
            {
                Logger::Error("provider::game_provider", "Received an invalid json response from request (data={})",
                    response.text);
                callback({});
                return;
            }

            JSON_CHECK_INVALID_VAR(json, "_sText", std::string)
            {
                Logger::Error("provider::game_provider", "Received an invalid json response from request (data={})",
                    response.text);
                callback({});
                return;
            }

            const auto parsed_mod = ParseMod(json);

            if (!parsed_mod.has_value())
            {
                callback({});
                return;
            }

            const auto& value = parsed_mod.value();

            ModInfo mod_info {};
            mod_info.id = value.id;
            mod_info.name = value.name;
            mod_info.submitter = value.submitter;
            mod_info.preview_media = value.preview_media;
            mod_info.description = json.at("_sText");

            JSON_CHECK_INVALID_VAR(json, "_aFiles", nlohmann::json::array_t)
            {
                callback({});
                return;
            }

            const auto& files = json.at("_aFiles");

            for (const auto& json_file : files)
            {
                JSON_CHECK_INVALID_VAR(json_file, "_idRow", uint32_t)
                {
                    continue;
                }

                const auto& file_id = json_file.at("_idRow").get<uint32_t>();

                JSON_CHECK_INVALID_VAR(json_file, "_sFile", std::string)
                {
                    continue;
                }

                File file;
                file.id = file_id;
                file.url = std::format("https://gamebanana.com/dl/{}", file_id);
                file.name = json_file.at("_sFile");

                mod_info.files.push_back(file);
            }

            callback(mod_info);
        };

        cpr::GetCallback(cpr_callback, cpr::Url {url});
    }

    std::optional<Provider::Mod> GameBananaProvider::ParseMod(const nlohmann::json& json)
    {
        constexpr auto COMPONENT = "provider::game_banana";
        constexpr auto MESSAGE = "Failed to parse mod";

        JSON_REQUIRE_VAR_DEBUG(json, "_idRow", uint32_t, std::nullopt, COMPONENT, MESSAGE)
        JSON_REQUIRE_VAR_DEBUG(json, "_sName", std::string, std::nullopt, COMPONENT, MESSAGE)
        JSON_REQUIRE_VAR_DEBUG(json, "_aSubmitter", nlohmann::json::object_t, std::nullopt, COMPONENT, MESSAGE)
        JSON_REQUIRE_VAR_DEBUG(json, "_aPreviewMedia", nlohmann::json::object_t, std::nullopt, COMPONENT, MESSAGE)

        const auto& json_preview_media = json.at("_aPreviewMedia");

        JSON_REQUIRE_VAR_DEBUG(
            json_preview_media, "_aImages", nlohmann::json::array_t, std::nullopt, COMPONENT, MESSAGE)

        const auto submitter = ParseSubmitter(json.at("_aSubmitter"));

        if (!submitter.has_value())
        {
            return std::nullopt;
        }

        Mod mod {};
        mod.id = json.at("_idRow");
        mod.name = json.at("_sName");
        mod.submitter = submitter.value();

        for (const auto& image : json_preview_media.at("_aImages"))
        {
            const auto preview_media = ParsePreviewMedia(image);

            if (preview_media.has_value())
            {
                mod.preview_media.push_back(preview_media.value());
            }
        }

        return mod;
    }

    std::optional<Provider::Submitter> GameBananaProvider::ParseSubmitter(const nlohmann::json& json)
    {
        JSON_REQUIRE_VAR_DEBUG(
            json, "_idRow", uint32_t, std::nullopt, "provider::game_banana", "Failed to parse submitter")
        JSON_REQUIRE_VAR_DEBUG(
            json, "_sName", std::string, std::nullopt, "provider::game_banana", "Failed to parse submitter")

        return Submitter {.id = json.at("_idRow"), .name = json.at("_sName")};
    }

    std::optional<Provider::PreviewMedia> GameBananaProvider::ParsePreviewMedia(const nlohmann::json& json)
    {
        JSON_REQUIRE_VAR_DEBUG(
            json, "_sType", std::string, std::nullopt, "provider::game_banana", "Failed to parse preview media")
        JSON_REQUIRE_VAR_DEBUG(
            json, "_sBaseUrl", std::string, std::nullopt, "provider::game_banana", "Failed to parse preview media")
        JSON_REQUIRE_VAR_DEBUG(
            json, "_sFile", std::string, std::nullopt, "provider::game_banana", "Failed to parse preview media")

        return PreviewMedia {.type = json.at("_sType"), .base_url = json.at("_sBaseUrl"), .file = json.at("_sFile")};
    }
}