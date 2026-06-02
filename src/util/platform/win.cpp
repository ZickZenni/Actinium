#include "win.h"

#include <windows.h>

namespace Actinium
{
    bool WinUtils::HasElevatedPrivileges()
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
}