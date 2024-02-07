#include "process.h"

VOID DoProcessInjectInside(LPCWSTR parentProcess, LPCWSTR fileName) 
{
	DWORD pid = GetProcessIdByName(parentProcess);
	CreateProcessAsTrustedInstaller(pid, fileName);
}


DWORD GetProcessIdByName(LPCWSTR processName)
{
	HANDLE hSnapshot;
	if ((hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0)) == INVALID_HANDLE_VALUE)
	{
		printf("CreateToolhelp32Snapshot failed: ", GetLastError());
	}

	DWORD pid = -1;
	PROCESSENTRY32W pe;
	ZeroMemory(&pe, sizeof(PROCESSENTRY32W));
	pe.dwSize = sizeof(PROCESSENTRY32W);
	if (Process32FirstW(hSnapshot, &pe))
	{
		while (Process32NextW(hSnapshot, &pe))
		{
			if (_wcsicmp(pe.szExeFile, processName) == 0)
			{
				pid = pe.th32ProcessID;
				break;
			}
		}
	}
	else
	{
		CloseHandle(hSnapshot);
		printf("Process32First failed: %d", GetLastError());
		return -1;
	}

	if (pid == -1)
	{
		CloseHandle(hSnapshot);
		printf("process not found: %ls",processName);
		return -1;
	}

	CloseHandle(hSnapshot);
	return pid;
}

VOID EnablePrivilege(LPWSTR privilegeName)
{
	HANDLE hToken;
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY | TOKEN_ADJUST_PRIVILEGES, &hToken)) {
		printf("OpenProcessToken failed: %d", GetLastError());
		return;
	}

	LUID luid;
	if (!LookupPrivilegeValueW(NULL, privilegeName, &luid))
	{
		CloseHandle(hToken);
		printf("LookupPrivilegeValue failed: %d", GetLastError());
		return;
	}

	TOKEN_PRIVILEGES tp;
	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = luid;
	tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	if (!AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), NULL, NULL))
	{
		CloseHandle(hToken);
		printf("AdjustTokenPrivilege failed: %d", GetLastError());
		return;
	}

	CloseHandle(hToken);
}

VOID ImpersonateSystem()
{
	DWORD systemPid = GetProcessIdByName(L"winlogon.exe");
	HANDLE hSystemProcess;
	if ((hSystemProcess = OpenProcess(PROCESS_DUP_HANDLE | PROCESS_QUERY_INFORMATION, FALSE, systemPid)) == NULL)
	{
		printf("OpenProcess failed (winlogon.exe): %d", GetLastError());
		return;
	}

	HANDLE hSystemToken;
	if (!OpenProcessToken(hSystemProcess, MAXIMUM_ALLOWED, &hSystemToken))
	{
		CloseHandle(hSystemProcess);
		printf("OpenProcessToken failed (winlogon.exe): %d", GetLastError());
		return;
	}

	HANDLE hDupToken;
	SECURITY_ATTRIBUTES tokenAttributes;
	tokenAttributes.nLength = sizeof(SECURITY_ATTRIBUTES);
	tokenAttributes.lpSecurityDescriptor = NULL;
	tokenAttributes.bInheritHandle = FALSE;
	if (!DuplicateTokenEx(hSystemToken, MAXIMUM_ALLOWED, &tokenAttributes, SecurityImpersonation, TokenImpersonation, &hDupToken))
	{
		CloseHandle(hSystemToken);
		printf("DuplicateTokenEx failed (winlogon.exe): %d", GetLastError());
		return;
	}

	if (!ImpersonateLoggedOnUser(hDupToken))
	{
		CloseHandle(hDupToken);
		CloseHandle(hSystemToken);
		printf("ImpersonateLoggedOnUser failed (winlogon.exe): %d", GetLastError());
		return;
	}

	CloseHandle(hDupToken);
	CloseHandle(hSystemToken);
}

VOID CreateProcessAsTrustedInstaller(DWORD pid, LPWSTR commandLine)
{
	EnablePrivilege(SE_DEBUG_NAME);
	EnablePrivilege(SE_IMPERSONATE_NAME);
	ImpersonateSystem();

	HANDLE hTIProcess;
	if ((hTIProcess = OpenProcess(PROCESS_DUP_HANDLE | PROCESS_QUERY_INFORMATION, FALSE, pid)) == NULL)
	{
		printf("OpenProcess failed (TrustedInstaller.exe): %d", GetLastError());
		return;
	}

	HANDLE hTIToken;
	if (!OpenProcessToken(hTIProcess, MAXIMUM_ALLOWED, &hTIToken))
	{
		CloseHandle(hTIProcess);
		printf("OpenProcessToken failed (TrustedInstaller.exe): %d", GetLastError());
		return;
	}

	HANDLE hDupToken;
	SECURITY_ATTRIBUTES tokenAttributes;
	tokenAttributes.nLength = sizeof(SECURITY_ATTRIBUTES);
	tokenAttributes.lpSecurityDescriptor = NULL;
	tokenAttributes.bInheritHandle = FALSE;
	if (!DuplicateTokenEx(hTIToken, MAXIMUM_ALLOWED, &tokenAttributes, SecurityImpersonation, TokenImpersonation, &hDupToken))
	{
		CloseHandle(hTIToken);
		printf("DuplicateTokenEx failed (TrustedInstaller.exe): %d", GetLastError());
		return;
	}

	STARTUPINFOW startupInfo;
	ZeroMemory(&startupInfo, sizeof(STARTUPINFOW));
	startupInfo.lpDesktop = L"Winsta0\\Default";
	PROCESS_INFORMATION processInfo;
	ZeroMemory(&processInfo, sizeof(PROCESS_INFORMATION));
	if (!CreateProcessWithTokenW(hDupToken, LOGON_WITH_PROFILE, NULL, commandLine, CREATE_UNICODE_ENVIRONMENT, NULL, NULL, &startupInfo, &processInfo))
	{
		printf("CreateProcessWithTokenW failed: %d", GetLastError());
		return;
	}
}