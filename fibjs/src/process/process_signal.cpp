/*
 * process_signal.cpp
 *
 *  Created on: Oct 16, 2017
 *      Author: lion
 */

#include "object.h"
#include "ifs/process.h"
#include "Fiber.h"
#include "Trigger.h"
#include <signal.h>

#ifdef _WIN32
#include <DbgHelp.h>
#else
#include <pwd.h>
#include <sys/resource.h>
#endif

namespace fibjs {

extern exlib::LockedList<Isolate> s_isolates;
exlib::atomic s_check_callback;

static void _InterruptCallback(v8::Isolate* v8_isolate, void* data)
{
    s_check_callback = 0;
    Isolate* isolate = Isolate::current();
    JSTrigger t(isolate->m_isolate, process_base::class_info().getModule(isolate));
    bool r = false;

    t._emit((const char*)data, NULL, 0, r);
    if (!r)
        _exit(1);
}

result_t async_signal(const char* name)
{
    s_isolates.head()->RequestInterrupt(_InterruptCallback, (void*)name);
    return 0;
}

void on_signal(int32_t s)
{
    const char* name = NULL;

    switch (s) {
    case SIGINT:
        name = "SIGINT";
        break;
    case SIGTERM:
        name = "SIGTERM";
        break;
#ifdef SIGBREAK
    case SIGBREAK:
        name = "SIGINT";
        break;
#endif
    default:
        _exit(1);
    }

    if (s_check_callback.CompareAndSwap(0, 1) != 0)
        _exit(1);
    asyncCall(async_signal, name);
}

#ifdef _WIN32

typedef BOOL(WINAPI* MINIDUMPWRITEDUMP)(HANDLE hProcess, DWORD dwPid, HANDLE hFile, MINIDUMP_TYPE DumpType,
    CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam, CONST PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
    CONST PMINIDUMP_CALLBACK_INFORMATION CallbackParam);

static MINIDUMPWRITEDUMP s_pDump;

HANDLE CreateUniqueDumpFile()
{
    char fname[MAX_PATH];
    int32_t l, i;
    HANDLE hFile;

    puts("core dump....");
    l = GetCurrentDirectory(MAX_PATH, fname);
    memcpy(fname + l, "\\core.", 6);
    l += 6;

    for (i = 0; i < 104; i++) {
        _itoa_s(i, fname + l, 10, 10);
        memcpy(fname + l + (i > 999 ? 4 : (i > 99 ? 3 : (i > 9 ? 2 : 1))),
            ".dmp", 5);

        hFile = CreateFile(fname, GENERIC_READ | GENERIC_WRITE, 0, NULL,
            CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
        if (hFile != INVALID_HANDLE_VALUE)
            return hFile;

        if (GetLastError() != ERROR_FILE_EXISTS)
            return INVALID_HANDLE_VALUE;
    };

    return INVALID_HANDLE_VALUE;
}

static void CreateMiniDump(LPEXCEPTION_POINTERS lpExceptionInfo)
{
    HANDLE hFile = CreateUniqueDumpFile();

    if (hFile != NULL && hFile != INVALID_HANDLE_VALUE) {
        MINIDUMP_EXCEPTION_INFORMATION mdei;

        mdei.ThreadId = GetCurrentThreadId();
        mdei.ExceptionPointers = lpExceptionInfo;
        mdei.ClientPointers = FALSE;

        MINIDUMP_TYPE mdt = MiniDumpNormal;
        BOOL retv = s_pDump(GetCurrentProcess(), GetCurrentProcessId(), hFile,
            mdt, (lpExceptionInfo != 0) ? &mdei : 0, 0, 0);

        CloseHandle(hFile);
    }
}

static LONG WINAPI GPTUnhandledExceptionFilter(PEXCEPTION_POINTERS pExceptionInfo)
{
    CreateMiniDump(pExceptionInfo);
    _exit(pExceptionInfo->ExceptionRecord->ExceptionCode);
    return EXCEPTION_EXECUTE_HANDLER;
}

void init_signal()
{
#ifndef DEBUG
    HMODULE hDll;
    if (hDll = ::LoadLibrary("DBGHELP.DLL")) {
        s_pDump = (MINIDUMPWRITEDUMP)::GetProcAddress(hDll,
            "MiniDumpWriteDump");
        if (s_pDump)
            SetUnhandledExceptionFilter(GPTUnhandledExceptionFilter);
    }
#endif

    signal(SIGINT, on_signal);
    signal(SIGTERM, on_signal);
    signal(SIGBREAK, on_signal);
}

#else

void init_signal()
{
    struct rlimit corelimit = { RLIM_INFINITY, RLIM_INFINITY };
    setrlimit(RLIMIT_CORE, &corelimit);

    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = on_signal;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;

    sigaction(SIGINT, &sigIntHandler, NULL);
    sigaction(SIGTERM, &sigIntHandler, NULL);
}

#endif
}
