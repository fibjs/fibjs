
#include <v8/v8.h>
#include <locale.h>

#ifdef assert
#undef assert
#endif

#include "ifs/global.h"
#include "ifs/process.h"
#include "ifs/global.h"
#include "ifs/Function.h"
#include "SandBox.h"
#include <exlib/lockfree.h>
#include "Fiber.h"
#include "utf8.h"
#include <exlib/thread.h>

namespace fibjs
{
v8::Isolate *isolate;
v8::Persistent<v8::Context> s_context;
v8::Persistent<v8::Context> s_context_test;
v8::Persistent<v8::Object> s_global;
obj_ptr<SandBox> s_topSandbox;

void init_argv(int argc, char **argv);

void _main(const char *fname)
{
    fiber_init();

    v8::V8::Initialize();

    isolate = v8::Isolate::New();
    v8::Locker locker(isolate);
    v8::Isolate::Scope isolate_scope(isolate);

    v8::HandleScope handle_scope(isolate);

    s_context_test.Reset(isolate, v8::Context::New(isolate));

    v8::Local<v8::Context> _context = v8::Context::New(isolate);
    v8::Context::Scope context_scope(_context);

    v8::Local<v8::Object> glob = _context->Global();
    global_base::class_info().Attach(glob, NULL);

    Function_base::class_info().Attach(
        glob->Get(v8::String::NewFromUtf8(isolate, "Function"))->ToObject()->GetPrototype()->ToObject(),
        NULL);

    s_context.Reset(isolate, _context);
    s_global.Reset(isolate, glob);

    Fiber_base *fb = new JSFiber();
    exlib::Service::tlsPut(g_tlsCurrent, fb);
    fb->Ref();

    {
        JSFiber::scope s;
        s_topSandbox = new SandBox();

        s_topSandbox->initRoot();
        if (fname)
            s.m_hr = s_topSandbox->run(fname);
        else
            s.m_hr = s_topSandbox->repl();
    }

    process_base::exit(0);

    s_context.Reset();
}

}

#ifdef _WIN32
#include <DbgHelp.h>

typedef BOOL (WINAPI *MINIDUMPWRITEDUMP)(HANDLE hProcess, DWORD dwPid, HANDLE hFile, MINIDUMP_TYPE DumpType,
        CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam, CONST PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
        CONST PMINIDUMP_CALLBACK_INFORMATION CallbackParam);

static MINIDUMPWRITEDUMP s_pDump;

HANDLE CreateUniqueDumpFile()
{
    char fname[MAX_PATH];
    int l, i;
    HANDLE hFile;

    puts("core dump....");
    l = GetCurrentDirectory(MAX_PATH, fname);
    memcpy(fname + l, "\\core.", 6);
    l += 6;

    for (i = 0; i < 104; i++)
    {
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

    if (hFile != NULL && hFile != INVALID_HANDLE_VALUE)
    {
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

void enableDump()
{
    HMODULE hDll;
    if (hDll = ::LoadLibrary("DBGHELP.DLL"))
    {
        s_pDump = (MINIDUMPWRITEDUMP) ::GetProcAddress(hDll,
                  "MiniDumpWriteDump");
        if (s_pDump)
            SetUnhandledExceptionFilter (GPTUnhandledExceptionFilter);
    }
}

#else

#include <pwd.h>
#include <sys/resource.h>

void enableDump()
{
    struct rlimit corelimit =
    { RLIM_INFINITY, RLIM_INFINITY };

    setrlimit(RLIMIT_CORE, &corelimit);
}

#endif

#ifdef x64
int stack_size = 512;
#else
int stack_size = 256;
#endif

int main(int argc, char *argv[])
{
    static char s_opts[64];
    static char s_sharmony[] = " --harmony --harmony_scoping --harmony_modules"
                               " --harmony_proxies --harmony_generators"
                               " --harmony_numeric_literals --harmony_strings"
                               " --harmony_arrays --harmony_arrow_functions"
                               " --use_strict";
    enableDump();

    exlib::OSThread::Sleep(1);

    v8::V8::SetFlagsFromString(s_sharmony, sizeof(s_sharmony) - 1);
    v8::V8::SetFlagsFromString(s_opts,
                               sprintf(s_opts, "--stack_size=%d", stack_size - 16));

    v8::V8::SetFlagsFromCommandLine(&argc, argv, true);

    fibjs::init_argv(argc, argv);

    ::setlocale(LC_ALL, "");

    if (argc >= 2 && argv[1][0] != '-')
        fibjs::_main(argv[1]);
    else
        fibjs::_main(NULL);

    return 0;
}
