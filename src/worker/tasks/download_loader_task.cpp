#include "download_loader_task.h"

#include "core/application.h"
#include "util/archive.h"
#include "util/github.h"
#include "util/path.h"
#include "worker/worker.h"

#include <spdlog/spdlog.h>

namespace Actinium
{
    std::vector<GitHub::Release> DownloadLoaderTask::s_cached_releases = {};

    DownloadLoaderTask::DownloadLoaderTask(const std::filesystem::path& path, Worker* worker)
        : Task(worker)
        , m_path(path)
    {
    }

    void DownloadLoaderTask::Run()
    {
        emit m_worker->ProgressChanged(0);

        if (s_cached_releases.empty())
        {
            s_cached_releases = GitHub::GetReleases("SpectrumQT", "XXMI-Libs-Package");
        }

        if (s_cached_releases.empty())
        {
            emit m_worker->Error("Failed to get releases");
            return;
        }

        const auto release = s_cached_releases.front();

        if (release.assets.empty())
        {
            emit m_worker->Error("No assets found");
            return;
        }

        const auto asset = std::ranges::find_if(release.assets,
            [](const GitHub::Asset& v)
            {
                return v.name.contains("XXMI") && v.content_type == "application/zip";
            });

        if (asset == release.assets.end())
        {
            emit m_worker->Error("No assets found");
            return;
        }

        SPDLOG_INFO("Downloading loader version \"{}\"...", release.tag_name);

        const std::filesystem::path temp_file_path = PathUtils::CreateTempFilePath();
        std::ofstream temp_file(temp_file_path, std::ios::binary);

        if (!temp_file)
        {
            emit m_worker->Error("Failed to create temp file");
            return;
        }

        const auto url = cpr::Url { asset->browser_download_url };
        const auto callback = cpr::ProgressCallback { OnDownloadProgress, reinterpret_cast<intptr_t>(this) };
        const auto response = cpr::Download(temp_file, url, callback);

        temp_file.close();

        if (m_worker->IsAborted())
        {
            std::filesystem::remove(temp_file_path);
            return;
        }

        if (response.status_code != 200)
        {
            std::filesystem::remove(temp_file_path);
            emit m_worker->Error("Failed to download loader");
            return;
        }

        emit m_worker->ProgressChanged(100);
        SPDLOG_INFO("Downloaded loader version \"{}\"", release.tag_name);

        const auto [code, message] = ArchiveUtils::ExtractArchive(temp_file_path, Application::GetAppDataPath() / "loader" / "3dmigoto" / release.tag_name);

        if (code != ARCHIVE_OK)
        {
            emit m_worker->Error(std::format("Archive extraction failed: {}", message));
        }

        std::filesystem::remove(temp_file_path);
    }

    bool DownloadLoaderTask::OnDownloadProgress(const cpr::cpr_pf_arg_t download_total,
        const cpr::cpr_pf_arg_t download_now, [[maybe_unused]] cpr::cpr_pf_arg_t upload_total,
        [[maybe_unused]] cpr::cpr_pf_arg_t upload_now, const intptr_t user_data)
    {
        const auto _this = reinterpret_cast<DownloadLoaderTask*>(user_data);
        Q_CHECK_PTR(_this);

        if (_this->m_worker->IsAborted())
        {
            return false;
        }

        if (download_total > 0)
        {
            const auto progress = static_cast<int>(download_now / download_total * 100.0);

            if (_this->m_last_progress != progress)
            {
                emit _this->m_worker->ProgressChanged(progress);

                _this->m_last_progress = progress;
            }
        }

        return true;
    }
}