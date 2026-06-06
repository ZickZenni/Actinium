#include "steam.h"

#include "util/platform/win.h"

#include <windows.h>

namespace Actinium::Steam
{
    std::optional<std::filesystem::path> DetectSteamInstallation()
    {
#ifdef WIN32
        struct RegistryCandidate
        {
            HKEY root;
            const wchar_t* subkey;
            const wchar_t* value;
            REGSAM view;
        };

        // clang-format off
        static const std::array registry_candidates {
            RegistryCandidate {HKEY_CURRENT_USER, L"Software\\Valve\\Steam", L"SteamExe", 0},
            RegistryCandidate {HKEY_CURRENT_USER, L"Software\\Valve\\Steam", L"SteamPath", 0},
            RegistryCandidate {HKEY_LOCAL_MACHINE, L"SOFTWARE\\Valve\\Steam", L"InstallPath", KEY_WOW64_32KEY},
            RegistryCandidate {HKEY_LOCAL_MACHINE, L"SOFTWARE\\Valve\\Steam", L"InstallPath", KEY_WOW64_64KEY}
        };
        // clang-format on

        for (const auto& candidate : registry_candidates)
        {
            auto value = Windows::ReadRegistryString(candidate.root, candidate.subkey, candidate.value, candidate.view);

            if (!value)
            {
                continue;
            }

            const auto& path = std::filesystem::path(*value);

            if (path.filename() == L"steam.exe")
            {
                if (std::filesystem::exists(path) && std::filesystem::is_regular_file(path))
                {
                    return path;
                }

                continue;
            }

            const auto& exe = path / L"steam.exe";

            if (std::filesystem::exists(exe) && std::filesystem::is_regular_file(exe))
            {
                return exe;
            }
        }

        constexpr std::array fallback_paths {
            L"C:\\Program Files (x86)\\Steam\\steam.exe", L"C:\\Program Files\\Steam\\steam.exe"};

        for (const auto* fallback : fallback_paths)
        {
            std::filesystem::path path(fallback);

            if (std::filesystem::exists(path) && std::filesystem::is_regular_file(path))
            {
                return path;
            }
        }

        return std::nullopt;
#else
        return std::nullopt;
#endif
    }
}