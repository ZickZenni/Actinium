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

        static ExtractResult ExtractArchive(const std::filesystem::path &file_path, const std::filesystem::path &extract_path);

    private:
        static int CopyData(archive *read_archive, archive *write_archive);
    };
}