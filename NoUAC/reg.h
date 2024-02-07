#pragma once
#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>

VOID ReadConfig(LPSTR value, DWORD* pvData);

BOOL IsValidConfig();

VOID CleanRegistry();

VOID BypassUsingComputerDefaults();

VOID BypassUsingSDCLT();