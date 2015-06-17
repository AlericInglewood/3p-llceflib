#include <windows.h>

// DLLMain is only required on Windows, doesn't do anything useful
BOOL APIENTRY DllMain( HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved )
{
    return TRUE;
}