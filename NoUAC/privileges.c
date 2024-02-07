#include "privileges.h"

BOOL IsElevated() {
    BOOL fRet = FALSE;
    HANDLE hToken = NULL;
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) {
        TOKEN_ELEVATION Elevation;
        DWORD cbSize = sizeof(TOKEN_ELEVATION);
        if (GetTokenInformation(hToken, TokenElevation, &Elevation, sizeof(Elevation), &cbSize)) {
            fRet = Elevation.TokenIsElevated;
        }
    }
    if (hToken) {
        CloseHandle(hToken);
    }
    return fRet;
}

BOOL IsSystem()
{
    DWORD i, dwSize = 0, dwResult = 0;
    HANDLE hToken;
    PTOKEN_USER Ptoken_User;

    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken))
    {
        printf("OpenProcessToken Error %u\n", GetLastError());
        return FALSE;
    }

    if (!GetTokenInformation(hToken, TokenUser, NULL, dwSize, &dwSize))
    {
        dwResult = GetLastError();
        if (dwResult != ERROR_INSUFFICIENT_BUFFER)
        {
            printf("GetTokenInformation Error %u\n", dwResult);
            return FALSE;
        }
    }

    Ptoken_User = (PTOKEN_USER)GlobalAlloc(GPTR, dwSize);

    if (!GetTokenInformation(hToken, TokenUser, Ptoken_User, dwSize, &dwSize))
    {
        printf("GetTokenInformation Error %u\n", GetLastError());
        return FALSE;
    }

    PSID pSystemSid;
    SID_IDENTIFIER_AUTHORITY siaNT = SECURITY_NT_AUTHORITY;
    if (!AllocateAndInitializeSid(&siaNT, 1, SECURITY_LOCAL_SYSTEM_RID,
        0, 0, 0, 0, 0, 0, 0, &pSystemSid))
        return FALSE;

    BOOL RunningAsSystem = EqualSid(Ptoken_User->User.Sid, pSystemSid);

    if (Ptoken_User) GlobalFree(Ptoken_User);

    return RunningAsSystem;

}