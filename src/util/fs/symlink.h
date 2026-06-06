#pragma once

#include <filesystem>

/**
 * This utility namespace exists because std::filesystem's symlink functions do not work / are not implemented in Windows.
 * Which makes sense cause only Unix systems have symlinks but Windows kinda also does, so fuck off.
 */
namespace Actinium::Symlink
{
    /**
     * Creates a symbolic link between a source file or directory and a target location.
     *
     * @note This function is platform-dependent and may behave differently on various operating systems.
     *       Implementation was tested on Linux and Windows.
     */
    bool CreateSymlink(const std::filesystem::path &target, const std::filesystem::path &link);

    /**
     * Determines whether the specified path is a symbolic link.
     *
     * @note This function is platform-dependent and may behave differently on various operating systems.
     *       Implementation was tested on Linux and Windows.
     */
    bool IsSymlink(const std::filesystem::path &path);
}