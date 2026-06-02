#pragma once

#include "task.h"

#include <cpr/cpr.h>

namespace Actinium
{
    class DownloadTask : public Task
    {
    public:
        explicit DownloadTask(Worker *worker);

    protected:
        int m_last_progress;

        cpr::Response Download(const std::string& url, std::ofstream& output_stream);

        static bool OnDownloadProgress(cpr::cpr_pf_arg_t download_total, cpr::cpr_pf_arg_t download_now,
            cpr::cpr_pf_arg_t upload_total, cpr::cpr_pf_arg_t upload_now, intptr_t user_data);
    };
}