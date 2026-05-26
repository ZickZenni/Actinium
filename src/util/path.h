#pragma once

#include <filesystem>
#include <unordered_set>

namespace Actinium
{
    class PathUtils
    {
    public:
        static const std::unordered_set<std::string> RESERVED;

        PathUtils() = delete;

        static std::string SanitizeName(const std::filesystem::path &path);

        static std::string RemoveUnallowedChars(const std::string &value);
    };
}