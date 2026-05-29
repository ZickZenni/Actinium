#include "download_loader_task.h"

#include "spdlog/spdlog.h"
#include "util/github.h"
#include "worker/worker.h"

namespace Actinium
{
    DownloadLoaderTask::DownloadLoaderTask(const std::filesystem::path& path, Worker* worker)
        : Task(worker)
        , m_path(path)
    {
    }

    void DownloadLoaderTask::Run()
    {
        const auto releases = GitHub::GetReleases("SpectrumQT", "XXMI-Libs-Package");

        if (releases.empty())
        {
            emit m_worker->Error("Failed to get releases");
            return;
        }

        const auto release = releases.front();

        if (release.assets.empty())
        {
            emit m_worker->Error("No assets found");
            return;
        }

        const auto asset = std::ranges::find_if(release.assets,
            [](const GitHub::Asset& v)
            {
                return v.name.contains("XXMI");
            });

        if (asset == release.assets.end())
        {
            emit m_worker->Error("No assets found");
            return;
        }

        SPDLOG_INFO("Downloading loader version \"{}\"...", release.tag_name);

        std::ofstream file(m_path / asset->name, std::ios::binary);

        int last_progress = -1;

        cpr::Response r = cpr::Download(file, cpr::Url { asset->browser_download_url },
            cpr::ProgressCallback {
                [&](const cpr::cpr_pf_arg_t download_total, const cpr::cpr_pf_arg_t download_now,
                    [[maybe_unused]] cpr::cpr_pf_arg_t upload_total, [[maybe_unused]] cpr::cpr_pf_arg_t upload_now,
                    [[maybe_unused]] intptr_t user_data)
                {
                    if (m_worker->IsAborted())
                    {
                        return false;
                    }

                    if (download_total != 0)
                    {
                        const auto progress = static_cast<int>(download_now / download_total * 100.0);

                        if (last_progress != progress)
                        {
                            SPDLOG_INFO("Downloading loader version \"{}\"... {}%", release.tag_name, progress);

                            emit m_worker->ProgressChanged(progress);

                            last_progress = progress;
                        }
                    }

                    return true;
                } });

        file.close();

        if (r.status_code != 200)
        {
            emit m_worker->Error("Failed to download loader");
            return;
        }

        emit m_worker->ProgressChanged(100);

        SPDLOG_INFO("Downloaded loader version \"{}\"", release.tag_name);
    }
}