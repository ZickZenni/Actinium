#pragma once

#include <cstdint>
#include <functional>
#include <string>
#include <vector>

namespace Actinium
{
    class Provider
    {
    public:
        struct Submitter
        {
            uint32_t id;
            std::string name;
        };

        struct PreviewMedia
        {
            std::string type;
            std::string base_url;
            std::string file;
        };

        struct Mod
        {
            uint32_t id;
            std::string name;
            Submitter submitter;
            std::vector<PreviewMedia> preview_media;
        };

        struct File
        {
            uint32_t id;
            std::string name;
            std::string url;
        };

        struct ModInfo : Mod
        {
            std::string description;
            std::vector<File> files;
        };

        struct SearchResponse
        {
            uint32_t total_count;
            std::vector<Mod> mods;
        };

        template<typename T> using ResponseCallback = std::function<void(T)>;

        virtual ~Provider() = default;

        virtual void GetMods(ResponseCallback<SearchResponse> callback) = 0;

        virtual void GetMod(uint32_t mod_id, ResponseCallback<ModInfo> callback) = 0;
    };
}