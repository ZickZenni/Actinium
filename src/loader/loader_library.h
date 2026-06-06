#pragma once

#include "util/api/github.h"

#include <semver.hpp>

namespace Actinium
{
    class LoaderLibrary
    {
    public:
        struct Version
        {
            semver::version<> version;
            std::filesystem::path path;
            std::string download_url;
        };

        explicit LoaderLibrary(GitHub::RepoLocation repository_location);

        const GitHub::RepoLocation& GetRepositoryLocation() const;

        const std::vector<Version>& GetVersions();

    private:
        GitHub::RepoLocation m_repository_location;
        std::vector<Version> m_versions;
    };
}