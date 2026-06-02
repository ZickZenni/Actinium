#pragma once

#include <cpr/cpr.h>
#include <expected>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

namespace Actinium
{
    class GitHub
    {
    public:
        struct Repo
        {
            std::string owner;
            std::string name;
        };

        struct Asset
        {
            int id;
            std::string url;
            std::string name;
            std::string browser_download_url;
            std::string content_type;
        };

        struct Release
        {
            int id;
            std::string tag_name;
            std::vector<Asset> assets;
        };

        static std::vector<Release> GetReleases(const std::string &owner, const std::string &repo);

    private:
        static std::expected<nlohmann::json, std::string> GetJson(const cpr::Url &url);
    };
}
