#ifdef _MSC_VER

#pragma managed(push, off)

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <stdio.h>
#include <delayimp.h>
#include <string.h>

#define HOST_BINARY "node.exe"

static FARPROC WINAPI load_exe_hook(unsigned int event, DelayLoadInfo* info)
{
    HMODULE m;
    if (event != dliNotePreLoadLibrary)
        return NULL;

    if (_stricmp(info->szDll, HOST_BINARY) != 0)
        return NULL;

    m = GetModuleHandle(NULL);
    return (FARPROC)m;
}

decltype(__pfnDliNotifyHook2) __pfnDliNotifyHook2 = load_exe_hook;

#pragma managed(pop)

#endif