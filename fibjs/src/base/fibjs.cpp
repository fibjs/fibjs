
#include <locale.h>

#include <string.h>
#include "ifs/console.h"
#include "ifs/global.h"
#include "ifs/process.h"
#include "ifs/global.h"
#include "ifs/Function.h"
#include "SandBox.h"
#include "Fiber.h"
#include "utf8.h"
#include "include/libplatform/libplatform.h"

namespace fibjs
{

void init_argv(int argc, char **argv);

class ShellArrayBufferAllocator : public v8::ArrayBuffer::Allocator
{
public:
    virtual void* Allocate(size_t length)
    {
        void* data = AllocateUninitialized(length);
        return data == NULL ? data : memset(data, 0, length);
    }

    virtual void* AllocateUninitialized(size_t length)
    {
        return malloc(length);
    }

    virtual void Free(void* data, size_t)
    {
        free(data);
    }
};

static exlib::lockfree<Isolate> s_isolates;

void _main(const char *fname)
{
    v8::Platform *platform = v8::platform::CreateDefaultPlatform();
    v8::V8::InitializePlatform(platform);

    v8::V8::Initialize();

    Isolate is;
    Isolate::reg(&is);
    s_isolates.put(&is);

    Isolate& isolate = Isolate::now();

    v8::Isolate::CreateParams create_params;
    ShellArrayBufferAllocator array_buffer_allocator;
    create_params.array_buffer_allocator = &array_buffer_allocator;

    isolate.isolate = v8::Isolate::New(create_params);
    v8::Locker locker(isolate.isolate);
    v8::Isolate::Scope isolate_scope(isolate.isolate);

    v8::HandleScope handle_scope(isolate.isolate);

    v8::Local<v8::Context> _context = v8::Context::New(isolate.isolate);
    v8::Context::Scope context_scope(_context);

    v8::Local<v8::Object> glob = _context->Global();
    global_base::class_info().Attach(glob, NULL);

    obj_ptr<console_base> s_console = new console_base();
    glob->ForceSet(_context, v8::String::NewFromUtf8(isolate.isolate, "console"), s_console->wrap());

    Function_base::class_info().Attach(
        glob->Get(v8::String::NewFromUtf8(isolate.isolate, "Function"))->ToObject()->GetPrototype()->ToObject(),
        NULL);

    isolate.s_context.Reset(isolate.isolate, _context);
    isolate.s_global.Reset(isolate.isolate, glob);

    JSFiber *fb = new JSFiber();
    {
        JSFiber::scope s(fb);
        isolate.s_topSandbox = new SandBox();

        isolate.s_topSandbox->initRoot();
        if (fname)
            s.m_hr = isolate.s_topSandbox->run(fname);
        else
            s.m_hr = isolate.s_topSandbox->repl();
    }

    process_base::exit(0);

    isolate.isolate->Dispose();

    v8::V8::ShutdownPlatform();
    delete platform;

    isolate.s_context.Reset();
}

}

void InterruptCallback(v8::Isolate *isolate, void *data)
{
    std::string msg;

    msg.append("User interrupt.", 15);
    msg.append(fibjs::traceInfo());

    fibjs::asyncLog(fibjs::console_base::_ERROR, msg);
    fibjs::process_base::exit(1);
}

void on_break(int s) {
    class delay_exit: public exlib::OSThread
    {
    public:
        delay_exit()
        {
            start();
        }

        virtual void Run()
        {
            Sleep(10);
            exit(1);
        }
    };

    puts("");

    fibjs::Isolate *p;
    while ((p = fibjs::s_isolates.get()) != 0)
        p->isolate->RequestInterrupt(InterruptCallback, NULL);

    new delay_exit();
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

BOOL WINAPI win_breakEvent(DWORD dwCtrlType)
{
    on_break(0);
    return TRUE;
}

void catchBreak()
{
    SetConsoleCtrlHandler(win_breakEvent, TRUE);
}

#else

#include <pwd.h>
#include <sys/resource.h>
#include <signal.h>

void enableDump()
{
    struct rlimit corelimit =
    { RLIM_INFINITY, RLIM_INFINITY };

    setrlimit(RLIMIT_CORE, &corelimit);
}

void catchBreak()
{
    struct sigaction sigIntHandler;

    sigIntHandler.sa_handler = on_break;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;

    sigaction(SIGINT, &sigIntHandler, NULL);
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
    static char s_sharmony[] = " --harmony --harmony_proxies"
                               " --use_strict"
                               " --nologfile_per_isolate";
    enableDump();
    catchBreak();

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
