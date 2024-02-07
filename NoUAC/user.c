#include <Windows.h>
#include <sddl.h>
#include <stdio.h>
#include <lm.h>
#include <tchar.h>

#pragma comment(lib, "Netapi32.lib")

#define MAX_NAME 256

BOOL IsMemberOfAdminGroup() {
    HANDLE hToken;
    DWORD dwLength;

    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) {
        printf("Error opening process token (%d)\n", GetLastError());
        goto Abort;
    }

    GetTokenInformation(hToken, TokenUser, NULL, 0, &dwLength);

    LPCWSTR sid = NULL;

    DWORD dwSize = MAX_NAME;
    BOOL bSuccess = FALSE;
    PTOKEN_USER ptu = NULL;

    if (NULL == hToken)
        goto Abort;

    if (!GetTokenInformation(hToken, TokenUser, (LPVOID)ptu, 0, &dwLength))
    {
        ptu = (PTOKEN_USER)HeapAlloc(GetProcessHeap(),
            HEAP_ZERO_MEMORY, dwLength);

        if (ptu == NULL)
            goto Abort;
    }

    if (!GetTokenInformation(hToken, TokenUser, (LPVOID)ptu, dwLength, &dwLength))
    {
        goto Abort;
    }

    if (ptu != NULL)
        HeapFree(GetProcessHeap(), 0, (LPVOID)ptu);

    PSID pCurrentUserSID = ptu->User.Sid;
    LPCWSTR adminGroupName = NULL;
    SID_NAME_USE SidType;
    wchar_t lpName[MAX_NAME];
    wchar_t lpDomain[MAX_NAME];

    LPCWSTR adminGroupSidString = L"S-1-5-32-544";
    PSID pAdminGroupSID = NULL;

    if (!ConvertStringSidToSidW(adminGroupSidString, &pAdminGroupSID)) {
        printf("Convert SID Error %u\n", GetLastError());
        goto Abort;
    }

    if (!LookupAccountSidW(NULL, pAdminGroupSID, lpName, &dwSize, lpDomain, &dwSize, &SidType))
    {
        DWORD dwResult = GetLastError();
        if (dwResult == ERROR_NONE_MAPPED)
            strcpy_s(lpName, sizeof("NONE_MAPPED"), "NONE_MAPPED");
        else
        {
            printf("LookupAccountSid Error %u\n", GetLastError());
            goto Abort;
        }
    }
    else
    {
        adminGroupName = lpName;
    }

    DWORD dwLevel = 1;
    LPLOCALGROUP_MEMBERS_INFO_1 pMembersInfo = NULL;
    DWORD dwEntriesRead = 0;
    DWORD dwTotalEntries = 0;
    NET_API_STATUS nStatus;
 
    nStatus = NetLocalGroupGetMembers(NULL, adminGroupName, dwLevel, (LPBYTE*)&pMembersInfo, MAX_PREFERRED_LENGTH, &dwEntriesRead, &dwTotalEntries, NULL);

    if (nStatus != NERR_Success) {
        printf("NetLocalGroupGetMembers Error %u\n", nStatus);
        goto Abort;
    }

    for (DWORD i = 0; i < dwEntriesRead; i++) {
        if (EqualSid(pMembersInfo[i].lgrmi1_sid, pCurrentUserSID)) {
            return TRUE;
        }
    }

    return FALSE;

Abort:
    perror("Unable to check user group membership. Cannot continue.");
    return FALSE; 
}