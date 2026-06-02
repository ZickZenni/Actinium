#pragma once

#include "download_task.h"
#include "util/api/github.h"

namespace Actinium
{
    class DownloadLibrariesTask : public DownloadTask
    {
    public:
        explicit DownloadLibrariesTask(const std::vector<GitHub::Repo> &libraries, Worker *worker);

        void Run() override;

    private:
        std::vector<GitHub::Repo> m_libraries;
    };
}