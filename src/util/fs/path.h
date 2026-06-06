#pragma once

#include <filesystem>
#include <unordered_set>

namespace Actinium::Path
{
    /**
     * Reserved file names.
     */
    inline const std::unordered_set<std::string> RESERVED
        = { "CON", "PRN", "AUX", "NUL", "COM1", "COM2", "COM3", "COM4", "COM5", "COM6", "COM7", "COM8", "COM9", "LPT1",
              "LPT2", "LPT3", "LPT4", "LPT5", "LPT6", "LPT7", "LPT8", "LPT9" };

    /**
     * Sanitizes the string value to be usable in the filesystem.
     */
    std::string SanitizeName(const std::string &name);

    /**
     * Sanitizes the string value to be usable in the filesystem.
     */
    std::string SanitizeName(const std::filesystem::path &path);

    /**
     * Removes characters that are not allowed either on Windows, Mac, or Linux.
     */
    std::string RemoveUnallowedChars(const std::string &value);

    /**
     * Creates a unique path to a file stored in the operating system's temporary directory.
     */
    std::filesystem::path CreateTempFilePath();

    /**
     * Generates a unique file path in case of name collisions.
     */
    std::filesystem::path CreateNonCollidingPath(const std::filesystem::path &desired_path);
}