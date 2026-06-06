#pragma once

#include "download_task.h"
#include "instance/instance.h"
#include "task.h"
#include "util/api/github.h"

namespace Actinium
{
    class DownloadLoaderTask : public DownloadTask
    {
    public:
        explicit DownloadLoaderTask(Instance* instance, Worker *worker);

        void Run() override;

    private:
        Instance* m_instance;

        /**
         * Checks if the specified GitHub asset is usable as a loader asset.
         */
        static bool IsLoaderAsset(const GitHub::Asset& asset);
    };
}