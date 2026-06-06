#include "migoto.h"

#include "core/application.h"
#include "util/fs/path.h"
#include "util/fs/symlink.h"
#include "util/platform/win.h"

#include <SimpleIni.h>
#include <semver.hpp>
#include <spdlog/spdlog.h>

namespace Actinium
{
    const std::vector<GitHub::Release>& MigotoLoader::GetReleases()
    {
        static std::vector<GitHub::Release> s_cached_releases;

        if (s_cached_releases.empty())
        {
            s_cached_releases = GitHub::GetReleases("SpectrumQT", "XXMI-Libs-Package");
        }

        for (auto& release : s_cached_releases)
        {
            if (release.tag_name.starts_with("v"))
            {
                release.tag_name.erase(0, 1);
            }
        }

        std::ranges::sort(s_cached_releases,
            [](const GitHub::Release& lhs, const GitHub::Release& rhs)
            {
                semver::version lhs_version;
                semver::version rhs_version;

                if (!semver::parse(lhs.tag_name, lhs_version))
                {
                    return false;
                }

                if (!semver::parse(rhs.tag_name, rhs_version))
                {
                    return true;
                }

                return lhs_version >= rhs_version;
            });

        return s_cached_releases;
    }

    uint64_t MigotoLoader::StartGame(const Instance* instance)
    {
#ifdef WIN32
        if (!instance->GetGame()->steam_app_id.has_value())
        {
            return StartGameNative(instance);
        }
        else
        {
            return StartGameUsingSteam(instance);
        }
#else
        return 0;
#endif
    }

    bool MigotoLoader::PrepareLoader(const Instance* instance)
    {
        const auto& loader_versions = GetReleases();

        if (loader_versions.empty())
        {
            return false;
        }

        const auto& latest_version = loader_versions.front().tag_name;
        const auto game_directory_name = Path::SanitizeName(instance->GetGame()->name);
        const auto loader_path
            = Application::GetAppDataPath() / "loaders" / game_directory_name / "3dmigoto" / latest_version;

        if (!PrepareConfigurationFile(instance, loader_path))
        {
            return false;
        }

        std::filesystem::create_directories(loader_path / "ShaderCache");
        std::filesystem::create_directories(loader_path / "ShaderFixes");
        std::filesystem::create_directories(loader_path / "Libraries");

        EnsureSymlink(instance->GetAbsolutePath() / "mods", loader_path / "Mods");

        return PrepareLibraries(instance, loader_path);
    }

    MigotoLoader::InjectResult MigotoLoader::InjectIntoProcess(const Instance* instance, const uint64_t process_id)
    {
#ifdef WIN32
        const auto& loader_versions = GetReleases();

        if (loader_versions.empty())
        {
            return InjectResult::UNKNOWN_ERROR;
        }

        const auto& latest_version = loader_versions.front().tag_name;

        SPDLOG_INFO("Using loader version: {}", latest_version);

        const auto game_directory_name = Path::SanitizeName(instance->GetGame()->name);
        const auto module_path = Application::GetAppDataPath() / "loaders" / game_directory_name / "3dmigoto"
            / latest_version / "d3d11.dll";

        if (!std::filesystem::exists(module_path))
        {
            return InjectResult::MISSING_FILES;
        }

        constexpr auto DESIRED_ACCESS = PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION
            | PROCESS_VM_WRITE | PROCESS_VM_READ;

        const auto process = OpenProcess(DESIRED_ACCESS, FALSE, process_id);

        if (process == nullptr)
        {
            return InjectResult::OPEN_PROCESS_FAILURE;
        }

        const auto kernel = GetModuleHandleW(L"kernel32.dll");

        if (kernel == nullptr)
        {
            CloseHandle(process);

            return InjectResult::LOAD_KERNEL32_FAILURE;
        }

        const auto load_library = GetProcAddress(kernel, "LoadLibraryW");

        if (load_library == nullptr)
        {
            CloseHandle(process);

            return InjectResult::RETRIEVE_LOADLIBRARY_FAILURE;
        }

        const auto module_path_length = (wcslen(module_path.c_str()) + 1) * sizeof(wchar_t);

        /**
         * Allocate memory to hold the module path.
         */
        const auto memory
            = VirtualAllocEx(process, nullptr, module_path_length, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

        if (memory == nullptr)
        {
            CloseHandle(process);

            return InjectResult::ALLOCATE_MEMORY_FAILURE;
        }

        if (!WriteProcessMemory(process, memory, module_path.c_str(), module_path_length, nullptr))
        {
            VirtualFreeEx(process, memory, 0, MEM_RELEASE);
            CloseHandle(process);

            return InjectResult::WRITE_MEMORY_FAILURE;
        }

        /**
         * Create a thread in the process to load the module from path in memory
         */
        const auto thread = CreateRemoteThreadEx(
            process, nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(load_library), memory, 0, nullptr, nullptr);

        if (!thread)
        {
            VirtualFreeEx(process, memory, 0, MEM_RELEASE);
            CloseHandle(process);

            return InjectResult::CREATE_THREAD_FAILURE;
        }

        const auto wait_code = WaitForSingleObject(thread, 15 * 1000);

        if (wait_code == WAIT_TIMEOUT)
        {
            CloseHandle(thread);
            VirtualFreeEx(process, memory, 0, MEM_RELEASE);
            CloseHandle(process);

            return InjectResult::THREAD_TIMEOUT;
        }

        if (wait_code == WAIT_FAILED)
        {
            CloseHandle(thread);
            VirtualFreeEx(process, memory, 0, MEM_RELEASE);
            CloseHandle(process);

            return InjectResult::THREAD_FAILED;
        }

        DWORD thread_exit_code = 0;
        auto result = InjectResult::OK;

        if (!GetExitCodeThread(thread, &thread_exit_code))
        {
            result = InjectResult::UNKNOWN_ERROR;
        }
        else if (thread_exit_code == 0)
        {
            result = InjectResult::LOADLIBRARY_FAILED;
        }

        CloseHandle(thread);
        VirtualFreeEx(process, memory, 0, MEM_RELEASE);
        CloseHandle(process);

        return result;
#else
        return InjectResult::NO_IMPLEMENTATION;
#endif
    }

    uint64_t MigotoLoader::StartGameNative(const Instance* instance)
    {
#ifdef WIN32
        const auto game_executable_path = GApp->GetGameExecutable(instance->GetGame()->id);

        if (!game_executable_path.has_value())
        {
            return 0;
        }

        if (Windows::IsProcessRunning(game_executable_path.value().filename().string()))
        {
            return MIGOTO_PROCESS_ALREADY_RUNNING;
        }

        SHELLEXECUTEINFOW exec_info = {};
        exec_info.cbSize = sizeof(SHELLEXECUTEINFOW);
        exec_info.fMask = SEE_MASK_NOCLOSEPROCESS;
        exec_info.hwnd = nullptr;
        exec_info.lpFile = game_executable_path.value().c_str();
        exec_info.lpDirectory = game_executable_path.value().parent_path().c_str();
        exec_info.nShow = SW_SHOWNORMAL;
        exec_info.lpVerb = L"runas";
        exec_info.hInstApp = nullptr;
        exec_info.lpParameters = nullptr;

        if (!ShellExecuteExW(&exec_info))
        {
            return 0;
        }

        const auto pid = GetProcessId(exec_info.hProcess);
        CloseHandle(exec_info.hProcess);

        return pid;
#else
        return 0;
#endif
    }

    uint64_t MigotoLoader::StartGameUsingSteam(const Instance* instance)
    {
#ifdef WIN32
        const auto& steam_path = Application::GetSteamExecutablePath();

        if (!steam_path.has_value())
        {
            return 0;
        }

        const auto game = instance->GetGame();

        if (!game->steam_app_id.has_value())
        {
            return 0;
        }

        if (Windows::IsProcessRunning(game->executable_name))
        {
            return MIGOTO_PROCESS_ALREADY_RUNNING;
        }

        std::vector<std::string> start_parameters;
        start_parameters.push_back("-applaunch");
        start_parameters.push_back(std::to_string(game->steam_app_id.value()));

        for (const auto& parameter : game->steam_start_parameters)
        {
            start_parameters.push_back(parameter);
        }

        const auto parameters = Windows::BuildParameters(start_parameters);

        SHELLEXECUTEINFOW exec_info = {};
        exec_info.cbSize = sizeof(SHELLEXECUTEINFOW);
        exec_info.fMask = SEE_MASK_DEFAULT;
        exec_info.hwnd = nullptr;
        exec_info.lpFile = steam_path.value().c_str();
        exec_info.lpDirectory = steam_path.value().parent_path().c_str();
        exec_info.nShow = SW_SHOWNORMAL;
        exec_info.lpVerb = L"runas";
        exec_info.hInstApp = nullptr;
        exec_info.lpParameters = parameters.c_str();

        if (!ShellExecuteExW(&exec_info))
        {
            return 0;
        }

        auto tries = 0;

        do
        {
            tries++;
            std::this_thread::sleep_for(std::chrono::seconds(1));
        } while (!Windows::IsProcessRunning(game->executable_name) && tries < 15);

        if (tries >= 15)
        {
            SPDLOG_ERROR("Failed to find game process \"{}\" after 15 seconds", game->executable_name);
            return 0;
        }

        return Windows::GetProcessId(game->executable_name);
#else
        return 0;
#endif
    }

    bool MigotoLoader::PrepareConfigurationFile(const Instance* instance, const std::filesystem::path& loader_path)
    {
        const auto loader_d3dini_path = loader_path / "d3dx.ini";
        const auto instance_d3dini_path = instance->GetAbsolutePath() / "d3dx.ini";

        if (!std::filesystem::exists(instance_d3dini_path))
        {
            SPDLOG_ERROR("Instance d3dx.ini file does not exist");
            return false;
        }

        if (!EnsureSymlink(instance_d3dini_path, loader_d3dini_path))
        {
            SPDLOG_ERROR("Failed to create symlink");
            return false;
        }

        return ModifyConfigurationFile(instance);
    }

    bool MigotoLoader::ModifyConfigurationFile(const Instance* instance)
    {
        const auto d3dini_path = instance->GetAbsolutePath() / "d3dx.ini";

        CSimpleIniA ini;
        ini.SetUnicode();
        ini.SetMultiKey(true);

        if (ini.LoadFile(d3dini_path.string().c_str()) < 0)
        {
            SPDLOG_ERROR("Failed to load d3dx.ini");
            return false;
        }

        ini.SetValue("Loader", "target", instance->GetGame()->executable_name.c_str());

        CSimpleIniA::TNamesDepend include_keys;
        ini.GetAllKeys("Include", include_keys);

        /**
         * We remove everything that was configured before, which isn't good if the user set something up.
         * For now its okay per say, but that should be fixed.
         */
        for (const auto& key : include_keys)
        {
            ini.Delete("Include", key.pItem, false);
        }

        /**
         * We mostly write back almost the same values that we deleted before but add obviously the correct paths
         * directing to the libraries used.
         */
        for (const auto& library : instance->GetGame()->libraries)
        {
            const auto& library_repository = library.GetRepositoryLocation();
            const auto library_directory_name
                = Path::SanitizeName(library_repository.owner + "." + library_repository.name);
            const auto library_main_file = library.GetMainLocation();

            if (library_main_file.has_value())
            {
                ini.SetValue("Include", "include",
                    std::format(R"(Libraries\{}\{})", library_directory_name, library_main_file.value()).c_str());
            }
        }

        ini.SetValue("Include", "include_recursive", "Mods");
        ini.SetValue("Include", "exclude_recursive", "DISABLED*");
        ini.SetValue("Include", "exclude_recursive", "desktop.ini");

        return ini.SaveFile(d3dini_path.string().c_str()) == SI_OK;
    }

    bool MigotoLoader::PrepareLibraries(const Instance* instance, const std::filesystem::path& loader_path)
    {
        const auto loader_shader_fixes_directory = loader_path / "ShaderFixes";

        for (auto& game_library : instance->GetGame()->libraries)
        {
            const auto& library_repository = game_library.GetRepositoryLocation();
            const auto& library_versions = game_library.GetVersions();

            if (library_versions.empty())
            {
                SPDLOG_WARN(
                    "No library versions were retrieved ({}/{})", library_repository.owner, library_repository.name);
                continue;
            }

            const auto& latest_library_version = library_versions.front();
            const auto& library_directory = latest_library_version.path;

            if (!std::filesystem::exists(library_directory))
            {
                SPDLOG_WARN("Library path points to a non-existing directory \"{}\"", library_directory.string());
                continue;
            }

            const auto library_link_name = Path::SanitizeName(library_repository.owner + "." + library_repository.name);
            EnsureSymlink(library_directory, loader_path / "Libraries" / library_link_name);

            const auto library_shader_fixes_directory = library_directory / "ShaderFixes";

            if (std::filesystem::exists(library_shader_fixes_directory))
            {
                EnsureSymlink(library_shader_fixes_directory, loader_shader_fixes_directory / library_link_name);
            }
        }

        return true;
    }

    bool MigotoLoader::EnsureSymlink(const std::filesystem::path& source, const std::filesystem::path& target)
    {
        if (std::filesystem::exists(target))
        {
            if (Symlink::IsSymlink(target))
            {
                std::filesystem::remove(target);
            }
            else
            {
                const auto bak_sufix = std::filesystem::is_directory(target) ? " bak" : ".bak";
                const auto bak_path
                    = Path::CreateNonCollidingPath(target.parent_path() / (target.filename().string() + bak_sufix));

                SPDLOG_WARN(
                    "Found non-symlink \"{}\", renaming it to \"{}\"", target.string(), bak_path.filename().string());

                std::filesystem::rename(target, bak_path);
            }
        }

        return Symlink::CreateSymlink(source, target);
    }
}