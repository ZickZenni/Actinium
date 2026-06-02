#pragma once

#include "download_task.h"
#include "task.h"
#include "util/api/github.h"

#include <filesystem>

namespace Actinium
{
    class DownloadLoaderTask : public DownloadTask
    {
    public:
        explicit DownloadLoaderTask(const std::filesystem::path &path, Worker *worker);

        void Run() override;

    private:
        std::filesystem::path m_path;

        static bool IsLoaderAsset(const GitHub::Asset& asset);
    };
}