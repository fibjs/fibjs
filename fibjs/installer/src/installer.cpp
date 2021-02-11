// installer.cpp : Defines the entry point for the console application.
//

#include <stdio.h>
#include <tchar.h>
#include <windows.h>

#include "js.h"

int _tmain(int argc, _TCHAR* argv[])
{
    DWORD dwRetVal = 0;
    UINT uRetVal = 0;
    WCHAR szTempFileName[MAX_PATH];
    WCHAR lpTempPathBuffer[MAX_PATH];
    WCHAR szSysDir[MAX_PATH];
    DWORD dwBytesWritten = 0;
    BOOL fSuccess = FALSE;
    HANDLE hTempFile = INVALID_HANDLE_VALUE;

    dwRetVal = GetSystemDirectoryW(szSysDir, MAX_PATH);
    if (dwRetVal > MAX_PATH || (dwRetVal == 0)) {
        puts("GetWindowsDirectory failed");
        return (1);
    }

    //  Gets the temp path env string (no guarantee it's a valid path).
    dwRetVal = GetTempPathW(MAX_PATH, // length of the buffer
        lpTempPathBuffer); // buffer for path
    if (dwRetVal > MAX_PATH || (dwRetVal == 0)) {
        puts("GetTempPath failed");
        return (2);
    }

    //  Generates a temporary file name.
    uRetVal = GetTempFileNameW(lpTempPathBuffer, // directory for tmp files
        L"CAB_", // temp file name prefix
        0, // create unique name
        szTempFileName); // buffer for name
    if (uRetVal == 0) {
        puts("GetTempFileName failed");
        return (3);
    }

    //  Creates the new file to write to for the upper-case version.
    hTempFile = CreateFileW(szTempFileName, // file name
        GENERIC_WRITE, // open for write
        0, // do not share
        NULL, // default security
        CREATE_ALWAYS, // overwrite existing
        FILE_ATTRIBUTE_NORMAL, // normal file
        NULL); // no template
    if (hTempFile == INVALID_HANDLE_VALUE) {
        puts("CreateFile failed");
        return (4);
    }

    fSuccess = WriteFile(hTempFile,
        js_data,
        sizeof(js_data),
        &dwBytesWritten,
        NULL);
    if (!fSuccess) {
        puts("WriteFile failed");
        return (5);
    }

    CloseHandle(hTempFile);

    WCHAR szParams[MAX_PATH * 3];
    size_t l;

    wcscpy_s(szParams, MAX_PATH, szTempFileName);

    l = wcslen(szParams);
    szParams[l++] = ' ';
    wcscpy_s(szParams + l, MAX_PATH, szSysDir);

    l = wcslen(szParams);
    wcscpy_s(szParams + l, MAX_PATH, L"\\fibjs.exe");

    wprintf(L"This program will install fibjs into %s.\n", szSysDir);

    SHELLEXECUTEINFOW ShExecInfo = { 0 };
    ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
    ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
    ShExecInfo.hwnd = NULL;
    ShExecInfo.lpVerb = L"runas";
    ShExecInfo.lpFile = L"expand.exe";
    ShExecInfo.lpParameters = szParams;
    ShExecInfo.lpDirectory = NULL;
    ShExecInfo.nShow = SW_HIDE;
    ShExecInfo.hInstApp = NULL;
    ShellExecuteExW(&ShExecInfo);
    WaitForSingleObject(ShExecInfo.hProcess, INFINITE);

    DeleteFileW(szTempFileName);

    return 0;
}
