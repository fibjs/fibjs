// installer.cpp : Defines the entry point for the console application.
//

#include <stdio.h>
#include <tchar.h>
#include <windows.h>

#if defined(_M_X64)
#include "..\bin\Windows_Release\js.h"
#elif defined(_M_IX86)
#include "..\bin\Windows_Release32\js.h"
#endif

int _tmain(int argc, _TCHAR *argv[])
{
    DWORD dwRetVal = 0;
    UINT uRetVal = 0;
    WCHAR szTempFileName[MAX_PATH];
    WCHAR lpTempPathBuffer[MAX_PATH];
    WCHAR szSysDir[MAX_PATH];
    DWORD dwBytesWritten = 0;
    BOOL fSuccess  = FALSE;
    HANDLE hTempFile = INVALID_HANDLE_VALUE;

    dwRetVal = GetSystemDirectory(szSysDir, MAX_PATH);
    if (dwRetVal > MAX_PATH || (dwRetVal == 0))
    {
        puts("GetWindowsDirectory failed");
        return (1);
    }

    //  Gets the temp path env string (no guarantee it's a valid path).
    dwRetVal = GetTempPath(MAX_PATH,          // length of the buffer
                           lpTempPathBuffer); // buffer for path
    if (dwRetVal > MAX_PATH || (dwRetVal == 0))
    {
        puts("GetTempPath failed");
        return (2);
    }

    //  Generates a temporary file name.
    uRetVal = GetTempFileName(lpTempPathBuffer, // directory for tmp files
                              L"CAB_",     // temp file name prefix
                              0,                // create unique name
                              szTempFileName);  // buffer for name
    if (uRetVal == 0)
    {
        puts("GetTempFileName failed");
        return (3);
    }

    //  Creates the new file to write to for the upper-case version.
    hTempFile = CreateFile((LPTSTR) szTempFileName, // file name
                           GENERIC_WRITE,        // open for write
                           0,                    // do not share
                           NULL,                 // default security
                           CREATE_ALWAYS,        // overwrite existing
                           FILE_ATTRIBUTE_NORMAL,// normal file
                           NULL);                // no template
    if (hTempFile == INVALID_HANDLE_VALUE)
    {
        puts("Second CreateFile failed");
        return (4);
    }

    fSuccess = WriteFile(hTempFile,
                         js_data,
                         sizeof(js_data),
                         &dwBytesWritten,
                         NULL);
    if (!fSuccess)
    {
        puts("WriteFile failed");
        return (5);
    }

    CloseHandle(hTempFile);

    WCHAR szCommmand[MAX_PATH * 3];
    size_t l;

    wcscpy_s(szCommmand, MAX_PATH * 3, L"expand ");

    l = wcslen(szCommmand);
    wcscpy_s(szCommmand + l, MAX_PATH * 3 - l, szTempFileName);

    l = wcslen(szCommmand);
    szCommmand[l ++] = ' ';
    wcscpy_s(szCommmand + l, MAX_PATH * 3 - l, szSysDir);

    l = wcslen(szCommmand);
    wcscpy_s(szCommmand + l, MAX_PATH * 3 - l, L"\\fibjs.exe");

    _putws(szCommmand);
    ::_wsystem(szCommmand);

    DeleteFile(szTempFileName);

    return 0;
}

