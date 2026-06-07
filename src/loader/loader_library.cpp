#include "loader_library.h"

#include "core/application.h"
#include "core/logger.h"
#include "spdlog/spdlog.h"

#include <utility>

namespace Actinium
{
    LoaderLibrary::LoaderLibrary(
        GitHub::RepoLocation repository_location, const LibraryMainLocationFunc& main_location_func)
        : m_repository_location(std::move(repository_location))
        , m_main_location_func(main_location_func)
    {
    }

    LoaderLibrary::LoaderLibrary(const std::string& repository_owner, const std::string& repository_name,
        const LibraryMainLocationFunc& main_location_func)
        : LoaderLibrary({repository_owner, repository_name}, main_location_func)
    {
    }

    const GitHub::RepoLocation& LoaderLibrary::GetRepositoryLocation() const
    {
        return m_repository_location;
    }

    const std::vector<LoaderLibrary::Version>& LoaderLibrary::GetVersions()
    {
        if (m_versions.empty())
        {
            const auto& github_releases = GitHub::GetReleases(m_repository_location.owner, m_repository_location.name);

            if (github_releases.empty())
            {
                Logger::Error("loader", "library_get_versions_failed", "Release retrieval from GitHub failed ({}/{})",
                    m_repository_location.owner, m_repository_location.name);
                return m_versions;
            }

            for (const auto& release : github_releases)
            {
                const auto& assets = release.assets;

                if (assets.empty())
                {
                    Logger::Warn(
                        "loader", "library_release_assets_empty", "No assets were found inside release {}", release.id);
                    continue;
                }

                auto raw_version = release.tag_name;

                if (raw_version.starts_with("v"))
                {
                    raw_version = raw_version.substr(1);
                }

                semver::version<> sem_version;

                if (!semver::parse(raw_version, sem_version))
                {
                    Logger::Error("loader", "library_invalid_version_format",
                        "Received invalid formatted version that semver cannot parse: {}", raw_version);
                    continue;
                }

                Version version;
                version.version = sem_version;
                version.path = Application::GetAppDataPath() / "libraries" / m_repository_location.owner
                    / m_repository_location.name / sem_version.to_string();
                /**
                 * Not pretty, should work for now.
                 */
                version.download_url = assets.front().browser_download_url;

                m_versions.push_back(version);
            }
        }

        return m_versions;
    }

    std::optional<std::string> LoaderLibrary::GetMainLocation() const
    {
        return m_main_location_func();
    }
}