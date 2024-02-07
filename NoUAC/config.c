#include "config.h"

LPCSTR configKey = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\System";

VOID ReadConfig(LPSTR regValue, DWORD *pvData) 
{
	DWORD bufferSize = sizeof(pvData);
	LONG err = RegGetValueA(HKEY_LOCAL_MACHINE, configKey, regValue, RRF_RT_REG_DWORD, NULL, pvData, &bufferSize);
	wprintf(L"Value data: %d\n", *pvData);

	if (err != ERROR_SUCCESS) 
	{
		wprintf(L"Error getting value. Code: %x\n", err);
	}
}

BOOL IsValidConfig()
{
	DWORD consentPromptBehaviorAdmin = 0;
	PDWORD pCPBA = &consentPromptBehaviorAdmin;

	DWORD enableLUA = 0;
	PDWORD pEnableLUA = &enableLUA;

	ReadConfig("ConsentPromptBehaviorAdmin", pCPBA);
	ReadConfig("EnableLUA", pEnableLUA);

	return consentPromptBehaviorAdmin != 2 && enableLUA == 1;
}