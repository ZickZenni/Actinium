#pragma once

#include <filesystem>
#include <optional>

namespace Actinium::Steam
{
    /**
     * Detects the current steam installation and retrieves the path to the steam executable.
     */
    std::optional<std::filesystem::path> DetectSteamInstallation();
}