#pragma once

#include <filesystem>
#include <optional>

namespace Actinium::Steam
{
    std::optional<std::filesystem::path> DetectSteamInstallation();
}