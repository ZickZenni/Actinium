#include "download_loader_task.h"

#include "core/application.h"
#include "util/api/github.h"
#include "util/fs/path.h"
#include "util/fs/temp_file.h"
#include "util/lib/archive.h"
#include "worker/worker.h"

#include <spdlog/spdlog.h>

namespace Actinium
{
    DownloadLoaderTask::DownloadLoaderTask(const std::filesystem::path& path, Worker* worker)
        : DownloadTask(worker)
        , m_path(path)
    {
    }

    void DownloadLoaderTask::Run()
    {
        static std::vector<GitHub::Release> s_cached_releases;

        if (s_cached_releases.empty())
        {
            s_cached_releases = GitHub::GetReleases("SpectrumQT", "XXMI-Libs-Package");
        }

        if (s_cached_releases.empty())
        {
            emit m_worker->Error("Failed to get releases");
            return;
        }

        const auto& release = s_cached_releases.front();
        const auto& asset = std::ranges::find_if(release.assets, IsLoaderAsset);

        if (asset == release.assets.end())
        {
            emit m_worker->Error("No suitable loader asset found");
            return;
        }

        SPDLOG_INFO("Downloading loader version \"{}\"...", release.tag_name);

        const TempFile temp_file(Path::CreateTempFilePath());
        std::ofstream out(temp_file.GetPath(), std::ios::binary);

        if (!out)
        {
            emit m_worker->Error("Failed to create temp file");
            return;
        }

        const auto response = Download(asset->browser_download_url, out);

        out.close();

        if (m_worker->IsAborted())
        {
            return;
        }

        if (response.status_code != 200)
        {
            emit m_worker->Error("Failed to download loader");
            return;
        }

        emit m_worker->TaskProgressChanged(100, 100);
        SPDLOG_INFO("Downloaded loader version \"{}\"", release.tag_name);

        const auto extract_path = Application::GetAppDataPath() / "loader" / "3dmigoto" / release.tag_name;
        const auto [code, message] = ArchiveUtils::ExtractArchive(temp_file.GetPath(), extract_path);

        if (code != ARCHIVE_OK)
        {
            emit m_worker->Error(std::format("Archive extraction failed: {}", message));
        }
    }

    bool DownloadLoaderTask::IsLoaderAsset(const GitHub::Asset& asset)
    {
        return asset.name.contains("XXMI") && asset.content_type == "application/zip";
    }
}