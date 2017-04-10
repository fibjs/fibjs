/*
 * profile.cpp
 *
 *  Created on: Jul 21, 2015
 *      Author: lion
 */

#include "console.h"
#include "Fiber.h"

#ifdef _WIN32
#include <DbgHelp.h>
#else
#include <pwd.h>
#include <sys/resource.h>
#include <signal.h>
#endif

namespace fibjs {

extern exlib::LockedList<Isolate> s_isolates;
extern bool g_perf;

static exlib::string traceFiber()
{
    exlib::string msg;

    exlib::linkitem* p = Isolate::current()->m_fibers.head();

    char buf[128];
    int32_t n = 0;

    while (p) {
        JSFiber* fb = (JSFiber*)p;

        sprintf(buf, "\nFiber %d:", n++);
        msg.append(buf);
        msg.append(fb->m_traceInfo);
        p = p->m_next;
    }

    return msg;
}

void dump_memory();
void dump_stack();

static void dumpFibers()
{
    exlib::string msg;

    msg.append(COLOR_LIGHTRED "User interrupt.");

    if (Isolate::rt::g_trace)
        msg.append(traceFiber());
    else if (JSFiber::current())
        msg.append(traceInfo(300));

    msg.append(COLOR_RESET "\n");

    std_logger::out(msg);

#ifdef DEBUG
    dump_memory();
    dump_stack();
#endif

    _exit(1);
}

static void cb_interrupt(v8::Isolate* isolate, void* data)
{
    dumpFibers();
}

void Isolate::InterruptCallback()
{
    dumpFibers();
}

void on_break(int32_t s)
{
    puts("");

    static bool s_double = false;
    if (s_double) {
        puts("User interrupt.");

#ifdef DEBUG
        dump_memory();
        dump_stack();
#endif

        _exit(1);
    }
    s_double = true;

#ifdef WIN32
    exlib::OSThread _thread;
    _thread.bindCurrent();
#endif

    Isolate* p = s_isolates.head();
    while (p != 0) {
        p->m_isolate->RequestInterrupt(cb_interrupt, NULL);
        p->m_interrupt = true;
        // p->RequestInterrupt(InterruptCallbackEx);

        p = s_isolates.next(p);
    }
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
    exit(pExceptionInfo->ExceptionRecord->ExceptionCode);
    return EXCEPTION_EXECUTE_HANDLER;
}

BOOL WINAPI win_breakEvent(DWORD dwCtrlType)
{
    on_break(0);
    return TRUE;
}

void init_prof()
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

    SetConsoleCtrlHandler(win_breakEvent, TRUE);
}

#else

void init_prof()
{
    struct rlimit corelimit = { RLIM_INFINITY, RLIM_INFINITY };
    struct sigaction sigIntHandler;

    setrlimit(RLIMIT_CORE, &corelimit);

    sigIntHandler.sa_handler = on_break;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;

    sigaction(SIGINT, &sigIntHandler, NULL);
    sigaction(SIGTERM, &sigIntHandler, NULL);
}

#endif
}
