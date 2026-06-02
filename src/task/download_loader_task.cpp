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
    std::vector<GitHub::Release> DownloadLoaderTask::s_cached_releases = {};

    DownloadLoaderTask::DownloadLoaderTask(const std::filesystem::path& path, Worker* worker)
        : Task(worker)
        , m_last_progress(-1)
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

        const auto& release = s_cached_releases.front();

        // ReSharper disable once CppDeclarationHidesUncapturedLocal
        const auto& asset = std::ranges::find_if(release.assets,
            [](const GitHub::Asset& asset)
            {
                return asset.name.contains("XXMI") && asset.content_type == "application/zip";
            });

        if (asset == release.assets.end())
        {
            emit m_worker->Error("No suitable loader asset found");
            return;
        }

        SPDLOG_INFO("Downloading loader version \"{}\"...", release.tag_name);

        TempFile tmp_file(Path::CreateTempFilePath());
        std::ofstream out(tmp_file.GetPath(), std::ios::binary);

        if (!out)
        {
            emit m_worker->Error("Failed to create temp file");
            return;
        }

        const auto url = cpr::Url { asset->browser_download_url };
        const auto callback = cpr::ProgressCallback { OnDownloadProgress, reinterpret_cast<intptr_t>(this) };
        const auto response = cpr::Download(out, url, callback);

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

        const auto [code, message] = ArchiveUtils::ExtractArchive(
            tmp_file.GetPath(), Application::GetAppDataPath() / "loader" / "3dmigoto" / release.tag_name);

        if (code != ARCHIVE_OK)
        {
            emit m_worker->Error(std::format("Archive extraction failed: {}", message));
        }
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
            const auto progress
                = static_cast<int>(static_cast<double>(download_now) / static_cast<double>(download_total) * 100.0);

            if (_this->m_last_progress != progress)
            {
                emit _this->m_worker->TaskProgressChanged(progress, 100);

                _this->m_last_progress = progress;
            }
        }

        return true;
    }
}