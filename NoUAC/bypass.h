#include <Windows.h>

const LPCSTR msSettingKey = "SOFTWARE\\Classes\\ms-settings\\Shell\\Open\\command";
const LPCSTR sdcltKey = "SOFTWARE\\Classes\\Folder\\Shell\\Open\\command";

VOID BypassUsingComputerDefaults();

VOID Clean();

VOID DeleteKey(LPCSTR key);