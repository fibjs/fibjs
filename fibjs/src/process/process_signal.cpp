/*
 * process_signal.cpp
 *
 *  Created on: Oct 16, 2017
 *      Author: lion
 */

#include "object.h"
#include "ifs/process.h"
#include "process_signal.h"
#include "Fiber.h"
#include "EventEmitter.h"
#include <signal.h>
#include <string.h>

#ifdef _WIN32
#include <DbgHelp.h>
#else
#include <pwd.h>
#include <sys/resource.h>
#endif

namespace fibjs {

static exlib::atomic s_check_callback;
// Signal that arrived but was not consumed by a JS listener yet: written by the
// signal handler, read by _InterruptCallback() and by the natural exit path of
// the main script (a signal must win over an ordinary exit(0)).
static exlib::atomic s_pending_signal;

// Set while the command runner waits for its child process.
static exlib::atomic s_forward_children;

bool signal_reraise(int32_t signum)
{
#ifdef _WIN32
    // Windows has no signal deaths: the caller falls back to exit(128 + signum).
    (void)signum;
    return false;
#else
    if (signum <= 0)
        return false;

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = SIG_DFL;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction(signum, &sa, NULL) < 0)
        return false;

    raise(signum);
    return true;
#endif
}

void process_signal_clear()
{
    s_pending_signal = 0;
}

void process_signal_note(int32_t signum)
{
    s_pending_signal = signum;
}

void process_signal_forward_children(bool on)
{
    s_forward_children = on ? 1 : 0;
}

// End the process the way the signal says it ended: by re-raising it, or with
// 128 + signum when the platform cannot re-raise it.
static void exit_by_signal(int32_t signum)
{
    if (signum > 0 && signal_reraise(signum))
        return;

    process_base::exit(signum > 0 ? 128 + signum : 1);
}

void process_signal_exit_pending()
{
    exit_by_signal((int32_t)s_pending_signal.xchg(0));
}

void process_signal_reraise_pending()
{
    int32_t signum = (int32_t)s_pending_signal.xchg(0);

    if (signum > 0)
        exit_by_signal(signum);
}

// Emit <name> on the process module and decide what the signal means for the
// process. Must only be called with a live JS scope.
static void emit_signal(Isolate* isolate, const char* name)
{
    bool r = false;
    result_t hr = 0;

    {
        JSFiber::EnterJsScope s;
        JSTrigger t(isolate->m_isolate, process_base::class_info().getModule(isolate));

        hr = t._emit(name, NULL, 0, r);
    }

    if (!r || hr < 0)
        // no listener: leave like the signal says instead of exit(1)
        process_signal_exit_pending();
    else
        process_signal_clear();
}

// Runs inside a V8 interrupt and emits the listener right away.
static void _InterruptCallback(v8::Isolate* v8_isolate, void* data)
{
    s_check_callback = 0;

    Isolate* isolate = Isolate::current(v8_isolate);

    if (!isolate)
        process_signal_exit_pending();
    else
        emit_signal(isolate, (const char*)data);
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
        _exit(128 + s);
    }

    // Remember it: even when the interrupt cannot reach the JS thread before
    // the program ends, the exit path still reports 128 + signum.
    s_pending_signal = s;

    if (s_forward_children) {
        // The command runner owns the terminal here: hand the signal to the
        // children and let their exit status decide the exit code.
        child_process_signal_alive(s);
        return;
    }

    if (s_check_callback.CompareAndSwap(0, 1) != 0)
        _exit(128 + s);
    async([name]() {
        Isolate* isolate = Isolate::main();

        if (isolate)
            isolate->RequestInterrupt(_InterruptCallback, (void*)name);
        else
            process_signal_exit_pending();
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
    exlib::string cwd;

    puts("core dump....");
    process_base::cwd(cwd);
    l = (int32_t)cwd.length();
    memcpy(fname, cwd.c_str(), l);
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
