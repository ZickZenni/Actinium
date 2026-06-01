#pragma once

namespace Actinium
{
    class WinUtils
    {
    public:
        WinUtils() = delete;

        /**
         * Checks if the current process has elevated privileges.
         */
        static bool HasElevatedPrivileges();
    };
}