#include <Windows.h>
#include <TlHelp32.h>

VOID DoProcessInjectInside(LPCWSTR parentProcess, LPCWSTR fileName);

DWORD GetProcessIdByName(LPCWSTR processName);

VOID EnablePrivilege(LPWSTR privilegeName);

VOID ImpersonateSystem();

VOID CreateProcessAsTrustedInstaller(DWORD pid, LPWSTR commandLine);