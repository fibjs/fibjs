/*
 * process_signal.cpp
 *
 *  Created on: Oct 16, 2017
 *      Author: lion
 */

#include "object.h"
#include "ifs/process.h"
#include "Fiber.h"
#include "EventEmitter.h"
#include <signal.h>

#ifdef _WIN32
#include <DbgHelp.h>
#else
#include <pwd.h>
#include <sys/resource.h>
#endif

namespace fibjs {

static exlib::atomic s_check_callback;

static void _InterruptCallback(v8::Isolate* v8_isolate, void* data)
{
    s_check_callback = 0;
    Isolate* isolate = Isolate::current(v8_isolate);
    bool r = false;
    result_t hr;

    {
        JSFiber::EnterJsScope s;
        JSTrigger t(isolate->m_isolate, process_base::class_info().getModule(isolate));

        hr = t._emit((const char*)data, NULL, 0, r);
    }

    if (!r || hr < 0)
        process_base::exit(1);
}

static void on_signal(int32_t s)
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
#ifdef SIGPIPE
    case SIGPIPE:
        return;
#endif
    default:
        _exit(1);
    }

    if (s_check_callback.CompareAndSwap(0, 1) != 0)
        _exit(1);
    async([name]() {
        Isolate* isolate = Isolate::main();

        if (isolate)
            isolate->RequestInterrupt(_InterruptCallback, (void*)name);
        else
            _exit(1);
    });
}

#ifdef _WIN32

typedef BOOL(WINAPI* MINIDUMPWRITEDUMP)(HANDLE hProcess, DWORD dwPid, HANDLE hFile, MINIDUMP_TYPE DumpType,
    CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam, CONST PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
    CONST PMINIDUMP_CALLBACK_INFORMATION CallbackParam);

static MINIDUMPWRITEDUMP s_pDump;

static HANDLE CreateUniqueDumpFile()
{
    char fname[MAX_PATH];
    int32_t l, i;
    HANDLE hFile;

    puts("core dump....");
    l = GetCurrentDirectoryA(MAX_PATH, fname);
    memcpy(fname + l, "\\core.", 6);
    l += 6;

    for (i = 0; i < 104; i++) {
        _itoa_s(i, fname + l, 10, 10);
        memcpy(fname + l + (i > 999 ? 4 : (i > 99 ? 3 : (i > 9 ? 2 : 1))),
            ".dmp", 5);

        hFile = CreateFileA(fname, GENERIC_READ | GENERIC_WRITE, 0, NULL,
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

#ifdef DEBUG
        DWORD mdt = MiniDumpWithFullMemory
            | MiniDumpWithFullMemoryInfo
            | MiniDumpWithHandleData
            | MiniDumpWithUnloadedModules
            | MiniDumpWithThreadInfo;
#else
        DWORD mdt = MiniDumpNormal;
#endif

        BOOL retv = s_pDump(GetCurrentProcess(), GetCurrentProcessId(), hFile,
            (MINIDUMP_TYPE)mdt, (lpExceptionInfo != 0) ? &mdei : 0, 0, 0);

        CloseHandle(hFile);
    }
}

static LONG WINAPI GPTUnhandledExceptionFilter(PEXCEPTION_POINTERS pExceptionInfo)
{
    CreateMiniDump(pExceptionInfo);
    _exit(pExceptionInfo->ExceptionRecord->ExceptionCode);
    return EXCEPTION_EXECUTE_HANDLER;
}

static BOOL WINAPI ConsoleCtrlHandler(DWORD ctrlType)
{
    switch (ctrlType) {
    case CTRL_C_EVENT:
    case CTRL_BREAK_EVENT:
    case CTRL_CLOSE_EVENT:
    case CTRL_LOGOFF_EVENT:
    case CTRL_SHUTDOWN_EVENT:
        on_signal(SIGBREAK);
        return TRUE;
    default:
        return FALSE;
    }
}

void init_signal()
{
    HMODULE hDll;
    if (hDll = ::LoadLibraryA("DBGHELP.DLL")) {
        s_pDump = (MINIDUMPWRITEDUMP)::GetProcAddress(hDll,
            "MiniDumpWriteDump");
        if (s_pDump)
            SetUnhandledExceptionFilter(GPTUnhandledExceptionFilter);
    }

    SetConsoleCtrlHandler(ConsoleCtrlHandler, TRUE);

    signal(SIGINT, on_signal);
    signal(SIGTERM, on_signal);
    signal(SIGBREAK, on_signal);
}

#else

void init_signal()
{
    struct rlimit corelimit = { RLIM_INFINITY, RLIM_INFINITY };
    setrlimit(RLIMIT_CORE, &corelimit);

    sigset_t blocked_signals;
    sigemptyset(&blocked_signals);
    pthread_sigmask(SIG_SETMASK, &blocked_signals, nullptr);

    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = on_signal;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;

    sigaction(SIGINT, &sigIntHandler, NULL);
    sigaction(SIGTERM, &sigIntHandler, NULL);
    sigaction(SIGPIPE, &sigIntHandler, NULL);
}

#endif
}
