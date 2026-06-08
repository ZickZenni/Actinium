#pragma once

#include <filesystem>
#include <string>

namespace Actinium
{
    struct InstalledMod
    {
        std::string name;
        std::filesystem::path path;
        bool disabled;
    };
}