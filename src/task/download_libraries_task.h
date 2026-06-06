#pragma once

#include "download_task.h"
#include "instance/instance.h"

namespace Actinium
{
    class DownloadLibrariesTask : public DownloadTask
    {
    public:
        explicit DownloadLibrariesTask(Instance* instance, const std::vector<LoaderLibrary> &libraries, Worker *worker);

        void Run() override;

    private:
        Instance* m_instance;
        std::vector<LoaderLibrary> m_libraries;

        /**
         * Copies the configuration file to the instance if it does not already have one.
         */
        static void CopyMissingConfigurationFile(const Instance * instance, const std::filesystem::path &source);
    };
}