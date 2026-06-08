#include "symlink.h"

#include "core/logger.h"

#include <spdlog/spdlog.h>
#include <windows.h>

namespace Actinium::Symlink
{
    bool CreateSymlink(const std::filesystem::path& target, const std::filesystem::path& link)
    {
#ifdef WIN32
        const auto is_directory = std::filesystem::exists(target) && std::filesystem::is_directory(target);
        const auto result = CreateSymbolicLinkW(
            link.wstring().c_str(), target.wstring().c_str(), is_directory ? SYMBOLIC_LINK_FLAG_DIRECTORY : 0);

        if (!result)
        {
            const auto error = GetLastError();
        }

        return result;
#else
        std::error_code error_code;
        std::filesystem::create_symlink(target, link, error_code);

        if (error_code)
        {
        }

        return !error_code;
#endif
    }

    bool IsSymlink(const std::filesystem::path& path)
    {
#ifdef WIN32
        const auto result = GetFileAttributesW(path.wstring().c_str()) & FILE_ATTRIBUTE_REPARSE_POINT;

        if (result == INVALID_FILE_ATTRIBUTES)
        {
            const auto error = GetLastError();
        }

        return result;
#else
        std::error_code error_code;

        const auto result = std::filesystem::is_symlink(path, error_code);

        if (error_code)
        {
        }

        return result;
#endif
    }
}