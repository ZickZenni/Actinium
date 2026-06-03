#pragma once

#include "download_task.h"
#include "instance/instance.h"
#include "util/api/github.h"

namespace Actinium
{
    class DownloadLibrariesTask : public DownloadTask
    {
    public:
        explicit DownloadLibrariesTask(Instance* instance, const std::vector<GitHub::Repo> &libraries, Worker *worker);

        void Run() override;

    private:
        Instance* m_instance;
        std::vector<GitHub::Repo> m_libraries;
    };
}