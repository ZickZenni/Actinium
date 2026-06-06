#pragma once

#include "instance/instance.h"

namespace Actinium
{
    /**
     * The OS should hopefully not use this value for a legitimate process.
     */
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

        /**
         * Retrieves all available releases of the loader.
         */
        static const std::vector<GitHub::Release> &GetReleases();

        /**
         * Starts the game either natively (executable) or steam.
         *
         * @note Implementation only includes Windows.
         */
        static uint64_t StartGame(const Instance *instance);

        /**
         * Prepares everything for the loader before injection:
         *  - Creates symlinks for libraries and instance mods
         *  - Configures `d3dx.ini` for everything to work
         */
        static bool PrepareLoader(const Instance *instance);

        /**
         * Injects the loader into the process by it's id using the OS API.
         *
         * @note Implementation only includes Windows.
         */
        static InjectResult InjectIntoProcess(const Instance *instance, uint64_t process_id);

    private:
        /**
         * Starts the game natively by just opening the executable.
         *
         * @return The process id of the newly created game process
         *         (0, if an error occurred)
         *         (MIGOTO_PROCESS_ALREADY_RUNNING, if the game is already running)
         * @note Implementation only includes Windows.
         */
        static uint64_t StartGameNative(const Instance *instance);

        /**
         * Starts the game using steam by running a command for steam to open the game's app.
         *
         * @return The process id of the newly created game process
         *         (0, if an error occurred)
         *         (MIGOTO_PROCESS_ALREADY_RUNNING, if the game is already running)
         * @note Implementation only includes Windows.
         */
        static uint64_t StartGameUsingSteam(const Instance *instance);

        /**
         * Prepares the configuration file (`d3dx.ini`) for the loader.
         */
        static bool PrepareConfigurationFile(const Instance *instance, const std::filesystem::path &loader_path);

        /**
         * Prepares the libraries for the loader.
         */
        static bool PrepareLibraries(const Instance *instance, const std::filesystem::path &loader_path);

        /**
         * Ensures that the symlink is being created by:
         *  - Checking if a symlink in the target path does already exist, if yes delete it.
         *  - If in the target path a non-symlink exist, try renaming it (deletion of user-data is not very fucking good)
         *  - Finally create a symlink.
         */
        static bool EnsureSymlink(const std::filesystem::path &source, const std::filesystem::path &target);
    };
}