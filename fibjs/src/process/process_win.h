#ifdef _WIN32
#include <windows.h>
#include <tlhelp32.h>

static DWORD GetParentProcessID(DWORD pid = GetCurrentProcessId())
{
    int parent_pid = -1;
    HANDLE h = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    PROCESSENTRY32 pe = { 0 };
    pe.dwSize = sizeof(PROCESSENTRY32);

    if (Process32First(h, &pe)) {
        do {
            if (pe.th32ProcessID == pid) {
                parent_pid = pe.th32ParentProcessID;
                break;
            }
        } while (Process32Next(h, &pe));
    }

    CloseHandle(h);

    return parent_pid;
}
#endif