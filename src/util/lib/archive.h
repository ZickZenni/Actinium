#pragma once

#include <archive.h>
#include <filesystem>

namespace Actinium
{
    class ArchiveUtils
    {
    public:
        struct ExtractResult
        {
            int code;
            std::string message;
        };

        ArchiveUtils() = delete;

        /**
         * Extracts an archive file using LibArchive to a specified extraction path.
         */
        static ExtractResult ExtractArchive(const std::filesystem::path &file_path, const std::filesystem::path &extract_path);

    private:
        /**
         * Copies data from the current file inside the read archive to the write archive (?, hopefully)
         * Stolen from LibArchive.
         */
        static int CopyData(archive *read_archive, archive *write_archive);
    };
}