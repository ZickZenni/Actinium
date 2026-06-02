#include "github.h"

#include "util/lib/json.h"

#include <cpr/cpr.h>
#include <spdlog/spdlog.h>

namespace Actinium
{
    std::vector<GitHub::Release> GitHub::GetReleases(const std::string& owner, const std::string& repo)
    {
        const auto result = GetJson(cpr::Url { "https://api.github.com/repos/" + owner + "/" + repo + "/releases" });

        if (!result.has_value())
        {
            return {};
        }

        const auto j = result.value();

        if (!j.is_array())
        {
            return {};
        }

        std::vector<Release> releases;

        for (const auto& release : j)
        {
            if (!release.is_object())
            {
                return {};
            }

            const auto id = nlohmann::try_get<int>(release, "id");
            const auto tag_name = nlohmann::try_get<std::string>(release, "tag_name");

            if (!id.has_value() || !tag_name.has_value() || !release.contains("assets"))
            {
                return {};
            }

            const auto assets = release["assets"];

            if (!assets.is_array())
            {
                return {};
            }

            Release release_obj;
            release_obj.id = id.value();
            release_obj.tag_name = tag_name.value();

            for (const auto& asset : assets)
            {
                if (!asset.is_object())
                {
                    return {};
                }

                const auto asset_id = nlohmann::try_get<int>(asset, "id");
                const auto name = nlohmann::try_get<std::string>(asset, "name");
                const auto browser_download_url = nlohmann::try_get<std::string>(asset, "browser_download_url");
                const auto content_type = nlohmann::try_get<std::string>(asset, "content_type");

                if (!asset_id.has_value() || !name.has_value() || !browser_download_url.has_value()
                    || !content_type.has_value())
                {
                    return {};
                }

                Asset asset_obj;
                asset_obj.id = asset_id.value();
                asset_obj.name = name.value();
                asset_obj.browser_download_url = browser_download_url.value();
                asset_obj.content_type = content_type.value();

                release_obj.assets.push_back(asset_obj);
            }

            releases.push_back(release_obj);
        }

        return releases;
    }

    std::expected<nlohmann::json, std::string> GitHub::GetJson(const cpr::Url& url)
    {
        const auto result = cpr::Get(url);

        if (result.status_code != 200)
        {
            SPDLOG_ERROR("Request failed with status code {}", result.status_code);

            return std::unexpected("Status code is non 200");
        }

        const auto body = result.text;

        if (body.empty())
        {
            SPDLOG_ERROR("Body is empty");

            return std::unexpected("Body is empty");
        }

        return nlohmann::json::parse(body);
    }
}