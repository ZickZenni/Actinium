#include "archive.h"

#include <archive_entry.h>
#include <spdlog/spdlog.h>

namespace Actinium
{
    ArchiveUtils::ExtractResult ArchiveUtils::ExtractArchive(
        const std::filesystem::path& file_path, const std::filesystem::path& extract_path)
    {
        if (!std::filesystem::exists(file_path) || !std::filesystem::is_regular_file(file_path))
        {
            return { .code = ARCHIVE_FATAL, .message = "File does not exist or is not a regular file" };
        }

        archive* read_archive = archive_read_new();
        archive* write_archive = archive_write_disk_new();

        archive_read_support_filter_all(read_archive);
        archive_read_support_format_all(read_archive);
        archive_write_disk_set_options(write_archive, ARCHIVE_EXTRACT_TIME);

        const auto result = archive_read_open_filename(read_archive, file_path.string().c_str(), 10240);

        if (result != ARCHIVE_OK)
        {
            const auto message = archive_error_string(read_archive);

            archive_read_close(read_archive);
            archive_read_free(read_archive);

            archive_write_close(write_archive);
            archive_write_free(write_archive);

            return { .code = result, .message = message };
        }

        archive_entry* entry;

        while (archive_read_next_header(read_archive, &entry) == ARCHIVE_OK)
        {
            const auto entry_file_name = archive_entry_pathname(entry);
            SPDLOG_DEBUG("Found file: {}", entry_file_name);

            archive_entry_set_pathname(entry, (extract_path / entry_file_name).string().c_str());

            if (archive_write_header(write_archive, entry) == ARCHIVE_OK)
            {
                CopyData(read_archive, write_archive);

                if (archive_write_finish_entry(write_archive) != ARCHIVE_OK)
                {
                    SPDLOG_ERROR("archive_write_finish_entry() error: {}", archive_error_string(write_archive));
                }
            }
            else
            {
                SPDLOG_WARN("archive_write_header() error: {}", archive_error_string(write_archive));
            }
        }

        archive_read_close(read_archive);
        archive_read_free(read_archive);

        archive_write_close(write_archive);
        archive_write_free(write_archive);

        return { .code = result, .message = "" };
    }

    int ArchiveUtils::CopyData(archive* read_archive, archive* write_archive)
    {
        const void* buff;
        size_t size;
#if ARCHIVE_VERSION_NUMBER >= 3000000
        int64_t offset;
#else
        off_t offset;
#endif

        for (;;)
        {
            const auto result = archive_read_data_block(read_archive, &buff, &size, &offset);

            if (result == ARCHIVE_EOF)
            {
                return ARCHIVE_OK;
            }

            if (result != ARCHIVE_OK)
            {
                return result;
            }

            if (archive_write_data_block(write_archive, buff, size, offset) != ARCHIVE_OK)
            {
                return result;
            }
        }
    }
}