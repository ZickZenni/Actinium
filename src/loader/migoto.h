#pragma once

#include "instance/instance.h"

namespace Actinium
{
    constexpr auto MIGOTO_PROCESS_ALREADY_RUNNING = UINT64_MAX;

    class MigotoLoader
    {
    public:
        enum class InjectResult
        {
            NO_IMPLEMENTATION = -2,
            UNKNOWN_ERROR,
            OK = 0,
            MISSING_FILES,
            OPEN_PROCESS_FAILURE,
            LOAD_KERNEL32_FAILURE,
            RETRIEVE_LOADLIBRARY_FAILURE,
            ALLOCATE_MEMORY_FAILURE,
            WRITE_MEMORY_FAILURE,
            CREATE_THREAD_FAILURE,
            THREAD_TIMEOUT,
            THREAD_FAILED,
            LOADLIBRARY_FAILED
        };

        static const std::vector<GitHub::Release> &GetReleases();

        static uint64_t StartGame(const Instance *instance);

        static bool PrepareLoader(const Instance *instance);

        static InjectResult InjectIntoProcess(const Instance *instance, uint64_t process_id);

    private:
        static uint64_t StartGameNative(const Instance *instance);

        static uint64_t StartGameUsingSteam(const Instance *instance);

        static bool PrepareConfigurationFile(const Instance *instance, const std::filesystem::path &loader_path);

        static bool PrepareLibraries(const Instance *instance, const std::filesystem::path &loader_path);

        static bool EnsureSymlink(const std::filesystem::path &source, const std::filesystem::path &target);
    };
}