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
        Instance* instance, const std::vector<LoaderLibrary>& libraries, Worker* worker)
        : DownloadTask(worker)
        , m_instance(instance)
        , m_libraries(libraries)
    {
    }

    void DownloadLibrariesTask::Run()
    {
        for (auto& library : m_libraries)
        {
            emit m_worker->TaskProgressChanged(0, 100);

            const auto& [owner, name] = library.GetRepositoryLocation();
            const auto& versions = library.GetVersions();

            if (versions.empty())
            {
                emit m_worker->Error("No versions were found");
                return;
            }

            const auto& latest_version = versions.front();

            const TempFile temp_file(Path::CreateTempFilePath());
            std::ofstream out(temp_file.GetPath(), std::ios::binary);

            if (!out)
            {
                emit m_worker->Error("Failed to create temp file");
                return;
            }

            const auto response = Download(latest_version.download_url, out);

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

            const auto extract_path
                = Application::GetAppDataPath() / "libraries" / owner / name / latest_version.version.to_string();
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

            std::filesystem::copy_file(config_path, m_instance->GetAbsolutePath() / "d3dx.ini",
                std::filesystem::copy_options::overwrite_existing);
        }
    }
}