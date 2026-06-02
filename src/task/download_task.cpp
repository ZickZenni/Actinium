#include "download_task.h"

#include "worker/worker.h"

#include <QtAssert>

namespace Actinium
{
    DownloadTask::DownloadTask(Worker* worker)
        : Task(worker)
        , m_last_progress(-1)
    {
    }

    cpr::Response DownloadTask::Download(const std::string& url, std::ofstream& output_stream)
    {
        const auto callback = cpr::ProgressCallback { OnDownloadProgress, reinterpret_cast<intptr_t>(this) };
        return cpr::Download(output_stream, cpr::Url { url }, callback);
    }

    bool DownloadTask::OnDownloadProgress(const cpr::cpr_pf_arg_t download_total, const cpr::cpr_pf_arg_t download_now,
        [[maybe_unused]] cpr::cpr_pf_arg_t upload_total, [[maybe_unused]] cpr::cpr_pf_arg_t upload_now,
        const intptr_t user_data)
    {
        const auto _this = reinterpret_cast<DownloadTask*>(user_data);
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