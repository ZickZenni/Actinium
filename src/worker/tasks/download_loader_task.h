#pragma once

#include "util/github.h"
#include "worker/task.h"

#include <cpr/cpr.h>
#include <filesystem>

namespace Actinium
{
    class DownloadLoaderTask : public Task
    {
    public:
        explicit DownloadLoaderTask(const std::filesystem::path &path, Worker *worker);

        void Run() override;

    private:
        static std::vector<GitHub::Release> s_cached_releases;

        int m_last_progress = -1;
        std::filesystem::path m_path;

        static bool OnDownloadProgress(cpr::cpr_pf_arg_t download_total, cpr::cpr_pf_arg_t download_now,
            cpr::cpr_pf_arg_t upload_total, cpr::cpr_pf_arg_t upload_now, intptr_t user_data);
    };
}