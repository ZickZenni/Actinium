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

        /**
         * Downloads a file from a specified url to a output stream and also handles progress update events.
         */
        cpr::Response Download(const std::string& url, std::ofstream& output_stream);

        /**
         * Handles a progress update event from the download.
         * This should probably be rewritten for multi downloads.
         */
        static bool OnDownloadProgress(cpr::cpr_pf_arg_t download_total, cpr::cpr_pf_arg_t download_now,
            cpr::cpr_pf_arg_t upload_total, cpr::cpr_pf_arg_t upload_now, intptr_t user_data);
    };
}