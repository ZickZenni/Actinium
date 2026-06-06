#include "win.h"

#include <cstdint>
#include <string>
#include <tchar.h>
#include <tlhelp32.h>

namespace Actinium::Windows
{
    std::wstring QuoteArgument(const char* value)
    {
        const auto result = FromString(value);

        if (result.find_first_of(L" \t\"") == std::wstring::npos)
        {
            return result;
        }

        std::wstring quoted = L"\"";

        for (const auto ch : result)
        {
            if (ch == L'"')
            {
                quoted += L'\\';
            }

            quoted += ch;
        }

        quoted += L'"';
        return quoted;
    }

    std::wstring BuildParameters(const std::vector<std::string>& args)
    {
        std::wstring parameters;

        for (const auto& arg : args)
        {
            if (!parameters.empty())
            {
                parameters += L' ';
            }

            parameters += QuoteArgument(arg.c_str());
        }

        return parameters;
    }

    bool HasElevatedPrivileges()
    {
        static SID_IDENTIFIER_AUTHORITY NT_AUTHORITY = SECURITY_NT_AUTHORITY;

        BOOL state = FALSE;
        PSID group = nullptr;

        if (AllocateAndInitializeSid(
                &NT_AUTHORITY, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0, &group))
        {
            if (!CheckTokenMembership(nullptr, group, &state))
            {
                state = FALSE;
            }

            FreeSid(group);
        }

        return state;
    }

    bool ExecuteItselfWithElevatePrivileges(const std::vector<std::string>& args)
    {
        wchar_t path[MAX_PATH];

        if (!GetModuleFileNameW(nullptr, path, MAX_PATH))
        {
            return false;
        }

        const auto parameters = BuildParameters(args);
        const auto result = ShellExecuteW(
            nullptr, L"runas", path, parameters.empty() ? nullptr : parameters.c_str(), nullptr, SW_SHOWNORMAL);
        const auto code = static_cast<int>(reinterpret_cast<std::uintptr_t>(result));

        return code > 32;
    }

    bool IsProcessRunning(const std::string& process_name)
    {
        PROCESSENTRY32 entry;
        entry.dwSize = sizeof(PROCESSENTRY32);

        const auto snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

        if (!Process32First(snapshot, &entry))
        {
            CloseHandle(snapshot);
            return false;
        }

        do
        {
            if (!_tcsicmp(entry.szExeFile, FromString(process_name).c_str()))
            {
                CloseHandle(snapshot);
                return true;
            }
        } while (Process32Next(snapshot, &entry));

        CloseHandle(snapshot);
        return false;
    }

    uint64_t GetProcessId(const std::string& process_name)
    {
        PROCESSENTRY32 entry;
        entry.dwSize = sizeof(PROCESSENTRY32);

        const auto snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

        if (!Process32First(snapshot, &entry))
        {
            CloseHandle(snapshot);
            return 0;
        }

        do
        {
            if (!_tcsicmp(entry.szExeFile, FromString(process_name).c_str()))
            {
                const auto pid = static_cast<uint64_t>(entry.th32ProcessID);

                CloseHandle(snapshot);

                return pid;
            }
        } while (Process32Next(snapshot, &entry));

        CloseHandle(snapshot);
        return 0;
    }

    std::wstring FromString(const std::string& str)
    {
        const auto length = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);

        if (length <= 0)
        {
            return L"";
        }

        std::wstring result;
        result.resize(static_cast<std::size_t>(length - 1));

        MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, result.data(), length);

        return result;
    }

    std::optional<std::wstring> ReadRegistryString(
        const HKEY root, const std::wstring& sub_key, const std::wstring& value, REGSAM view)
    {
        HKEY key = nullptr;

        if (RegOpenKeyExW(root, sub_key.c_str(), 0, KEY_READ | view, &key) != ERROR_SUCCESS)
        {
            return std::nullopt;
        }

        DWORD type = 0;
        DWORD size = 0;

        if (RegQueryValueExW(key, value.c_str(), nullptr, &type, nullptr, &size) != ERROR_SUCCESS
            || (type != REG_SZ && type != REG_EXPAND_SZ))
        {
            RegCloseKey(key);
            return std::nullopt;
        }

        std::wstring data(size / sizeof(wchar_t), L'\0');

        RegCloseKey(key);

        if (RegQueryValueExW(key, value.c_str(), nullptr, nullptr, reinterpret_cast<LPBYTE>(data.data()), &size)
            != ERROR_SUCCESS)
        {
            return std::nullopt;
        }

        while (!data.empty() && data.back() == L'\0')
        {
            data.pop_back();
        }

        if (type == REG_EXPAND_SZ)
        {
            wchar_t expanded[MAX_PATH];

            const auto expanded_size = ExpandEnvironmentStringsW(data.c_str(), expanded, std::size(expanded));

            if (expanded_size > 0 && expanded_size <= std::size(expanded))
            {
                data = expanded;
            }
        }

        if (data.empty())
        {
            return std::nullopt;
        }

        return data;
    }
}