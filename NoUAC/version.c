#include "version.h"

BOOL IsWindows10() {
	OSVERSIONINFOEX info;
	ZeroMemory(&info, sizeof(OSVERSIONINFOEX));
	info.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

	GetVersionEx(&info);

	return info.dwMajorVersion >= 10;
}