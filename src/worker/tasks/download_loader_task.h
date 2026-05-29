#pragma once
#include "worker/task.h"

#include <filesystem>

namespace Actinium
{
    class DownloadLoaderTask : public Task
    {
    public:
        explicit DownloadLoaderTask(const std::filesystem::path& path, Worker *worker);

        void Run() override;

    private:
        std::filesystem::path m_path;
    };
}