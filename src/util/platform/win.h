#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>
#include <windows.h>

namespace Actinium::Windows
{
    /**
     * Quotes the argument when it contains spaces.
     */
    std::wstring QuoteArgument(const char *value);

    /**
     * Builds a single std::wstring used for a ShellExecuteW call.
     */
    std::wstring BuildParameters(const std::vector<std::string> &args);

    /**
     * Checks if the current process has elevated privileges.
     */
    bool HasElevatedPrivileges();

    /**
     * Executes itself with elevated privileges, no fucking wonder.
     */
    bool ExecuteItselfWithElevatePrivileges(const std::vector<std::string> &args);

    /**
     * Checks if the process is running.
     */
    bool IsProcessRunning(const std::string &process_name);

    /**
     * Retrieves the id of a process by it's name.
     */
    uint64_t GetProcessId(const std::string &process_name);

    /**
     * Converts a std::string to std::wstring, what did you expect?
     */
    std::wstring FromString(const std::string &str);

    /**
     * Reads a string value from the Windows Registry.
     */
    std::optional<std::wstring> ReadRegistryString(
        HKEY root, const std::wstring &sub_key, const std::wstring &value, REGSAM view = 0);
}