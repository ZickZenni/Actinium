#include "download_libraries_task.h"

#include "core/application.h"
#include "util/api/github.h"
#include "util/fs/path.h"
#include "util/fs/temp_file.h"
#include "util/lib/archive.h"
#include "worker/worker.h"

namespace Actinium
{
    DownloadLibrariesTask::DownloadLibrariesTask(
        Instance* instance, const std::vector<GitHub::Repo>& libraries, Worker* worker)
        : DownloadTask(worker)
        , m_instance(instance)
        , m_libraries(libraries)
    {
    }

    void DownloadLibrariesTask::Run()
    {
        for (const auto& [owner, name] : m_libraries)
        {
            emit m_worker->TaskProgressChanged(0, 100);

            const auto releases = GitHub::GetReleases(owner, name);
            const auto& release = releases.front();

            if (release.assets.empty())
            {
                emit m_worker->Error("No suitable library asset found");
                return;
            }

            const auto& asset = release.assets.front();

            const TempFile temp_file(Path::CreateTempFilePath());
            std::ofstream out(temp_file.GetPath(), std::ios::binary);

            if (!out)
            {
                emit m_worker->Error("Failed to create temp file");
                return;
            }

            const auto response = Download(asset.browser_download_url, out);

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

            const auto extract_path = Application::GetAppDataPath() / "libraries" / owner / name / release.tag_name;
            const auto [code, message] = ArchiveUtils::ExtractArchive(temp_file.GetPath(), extract_path);

            if (code != ARCHIVE_OK)
            {
                emit m_worker->Error(std::format("Archive extraction failed: {}", message));
                return;
            }

            const auto config_path = extract_path / "d3dx.ini";

            if (!std::filesystem::exists(config_path))
            {
                return;
            }

            std::filesystem::copy_file(config_path, m_instance->GetAbsolutePath() / "d3dx.ini");
        }
    }
}