#pragma once

#include "util/api/github.h"

#include <semver.hpp>

namespace Actinium
{
    typedef std::function<std::optional<std::string>()> LibraryMainLocationFunc;

    class LoaderLibrary
    {
    public:
        struct Version
        {
            semver::version<> version;
            std::filesystem::path path;
            std::string download_url;
        };

        explicit LoaderLibrary(GitHub::RepoLocation repository_location, const LibraryMainLocationFunc &main_location_func);
        explicit LoaderLibrary(const std::string &repository_owner, const std::string &repository_name,
            const LibraryMainLocationFunc &main_location_func);

        /**
         * Retrieves the location pointing to the GitHub repository where the library is stored.
         */
        const GitHub::RepoLocation &GetRepositoryLocation() const;

        /**
         * Retrieves all versions available.
         */
        const std::vector<Version> &GetVersions();

        [[nodiscard]] std::optional<std::string> GetMainLocation() const;

    private:
        GitHub::RepoLocation m_repository_location;
        std::vector<Version> m_versions;
        LibraryMainLocationFunc m_main_location_func;
    };
}