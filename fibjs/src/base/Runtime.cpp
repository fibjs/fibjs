/*
 * Runtime.cpp
 *
 *  Created on: Jul 23, 2012
 *      Author: lion
 */

#include <locale.h>
#include <string.h>
#include <time.h>
#include "object.h"
#include "Runtime.h"
#include "Fiber.h"
#include "SandBox.h"
#include "HttpClient.h"
#include "X509Cert.h"
#include "console.h"
#include "LruCache.h"
#include "EventEmitter.h"
#include "ifs/global.h"
#include "ifs/os.h"
#include "ifs/process.h"
#include "ifs/coroutine.h"
#include "ifs/profiler.h"
#include "path.h"
#include "options.h"
#include "include/libplatform/libplatform.h"

namespace fibjs {

void InitializeDateUtils();
void InitializeAcPool();
void InitializeAsyncIOThread();
void initializeUVAsyncThread();
void init_signal();
void init_sym();

void init_argv(int32_t argc, char** argv);
void init_start_argv(int32_t argc, char** argv);
void options(int32_t& pos, char* argv[]);
result_t ifZipFile(exlib::string filename, bool& retVal);

exlib::string s_root;

static void createBasisForFiberLoop(v8::Platform* (*get_platform)())
{
    ::setlocale(LC_ALL, "");

    int32_t cpus = 0;

    process_base::cwd(s_root);

    os_base::cpuNumbers(cpus);
    if (cpus < 2)
        cpus = 2;

    exlib::Service::init(cpus + 1);

    InitializeDateUtils();
    InitializeAcPool();
    InitializeAsyncIOThread();
    initializeUVAsyncThread();
    init_signal();

#ifdef Linux
    init_sym();
#endif

    srand((unsigned int)time(0));

    v8::Platform* platform = get_platform != NULL ? get_platform() : v8::platform::CreateDefaultPlatform();
    v8::V8::InitializePlatform(platform);
    v8::V8::Initialize();
}

void start(int32_t argc, char** argv, result_t (*jsEntryFiber)(Isolate*), v8::Platform* (*get_platform)())
{
    class EntryThread : public exlib::OSThread {
    public:
        EntryThread(int32_t argc, char** argv, result_t (*jsFiber)(Isolate*), v8::Platform* (*get_platform)())
            : m_argc(argc)
            , m_jsFiber(jsFiber)
            , m_get_platform(get_platform)
        {
            for (int32_t i = 0; i < argc; i++)
                m_argv.push_back(argv[i]);
        }

    public:
        static void FirstFiber(void* p)
        {
            EntryThread* th = (EntryThread*)p;
            Isolate* isolate = new Isolate(th->m_fibjsEntry);
            syncCall(isolate, th->m_jsFiber, isolate);
        }

        virtual void Run()
        {
            int32_t argc = m_argc;
            char** argv = m_argv.data();

            exlib::string exePath;
            std::vector<char*> ptrArg;

            process_base::get_execPath(exePath);

            bool bZip;
            ifZipFile(exePath, bZip);
            if (bZip) {

                exePath.append(1, '$');
                ptrArg.resize(argc + 1);

                ptrArg[0] = argv[0];
                ptrArg[1] = exePath.c_buffer();

                int32_t i;
                for (i = 1; i < argc; i++)
                    ptrArg[i + 1] = argv[i];

                argv = &ptrArg[0];
                argc++;
            }

            init_start_argv(argc, argv);

            int32_t pos = argc;
            options(pos, argv);

            m_sem.Post();

            if (!g_cefprocess) {
                createBasisForFiberLoop(m_get_platform);

                if (pos < argc) {
                    m_fibjsEntry = argv[pos];

                    if (pos != 1) {
                        int32_t p = 1;
                        for (; pos < argc; pos++)
                            argv[p++] = argv[pos];
                        argc = p;
                    }
                }

                init_argv(argc, argv);

                exlib::Service::CreateFiber(FirstFiber, this, 256 * 1024, "start");
                exlib::Service::dispatch();
            }
        }

    public:
        exlib::OSSemaphore m_sem;

    private:
        int32_t m_argc;
        std::vector<char*> m_argv;
        exlib::string m_fibjsEntry;
        result_t (*m_jsFiber)(Isolate*);
        v8::Platform* (*m_get_platform)();
    };

    EntryThread* entryThread = new EntryThread(argc, argv, jsEntryFiber, get_platform);
    entryThread->start();

    entryThread->m_sem.Wait();
}

static int32_t s_tls_rt;

class rt_initer {
public:
    rt_initer()
    {
        s_tls_rt = exlib::Fiber::tlsAlloc();
    }
} s_rt_initer;

void Runtime::RegInThread()
{
    exlib::Fiber::tlsPut(s_tls_rt, this);
}

Runtime* Runtime::current()
{
    return (Runtime*)exlib::Fiber::tlsGet(s_tls_rt);
}

class ShellArrayBufferAllocator : public v8::ArrayBuffer::Allocator {
public:
    virtual void* Allocate(size_t length)
    {
        void* data = AllocateUninitialized(length);
        return data == NULL ? data : memset(data, 0, length);
    }

    virtual void* AllocateUninitialized(size_t length)
    {
        return exlib::string::Buffer::New(length)->data();
    }

    virtual void Free(void* data, size_t)
    {
        exlib::string::Buffer::fromData((char*)data)->unref();
    }
};

exlib::LockedList<Isolate> s_isolates;
exlib::atomic s_iso_id;
exlib::atomic s_iso_ref;

Isolate::SnapshotJsScope::SnapshotJsScope(Isolate* cur)
    : m_isolate((cur ? cur : Isolate::current()))
{
    JSFiber* fb = JSFiber::current();
    V8FrameInfo _fi = save_fi(m_isolate->m_isolate);

    fb->m_c_entry_fp_ = _fi.entry_fp;
    fb->m_handler_ = _fi.handle;

    m_isolate->m_isolate->RunMicrotasks();
}

Isolate::SnapshotJsScope::~SnapshotJsScope()
{
}

static void fb_GCCallback(v8::Isolate* js_isolate, v8::GCType type, v8::GCCallbackFlags flags)
{
    Isolate* isolate = Isolate::current();
    exlib::linkitem* p;
    exlib::List<exlib::linkitem> freelist;

    while (true) {
        isolate->m_weakLock.lock();
        isolate->m_weak.getList(freelist);
        isolate->m_weakLock.unlock();

        if (freelist.empty())
            break;

        while ((p = freelist.getHead()) != 0)
            object_base::gc_delete(p);
    }
}

static void FiberProcIsolate(void* p)
{
    Isolate* isolate = (Isolate*)p;
    Runtime rtForThread(isolate);

    isolate->init();
    JSFiber::FiberProcRunJavascript(p);
}

Isolate::Isolate(exlib::string jsFilename)
    : m_id((int32_t)s_iso_id.inc())
    , m_hr(0)
    , m_test(NULL)
    , m_currentFibers(0)
    , m_idleFibers(0)
    , m_fid(1)
    , m_flake_tm(0)
    , m_flake_host(0)
    , m_flake_count(0)
    , m_console_colored(true)
    , m_loglevel(console_base::C_NOTSET)
    , m_defaultMaxListeners(10)
    , m_exitCode(0)
    , m_enable_FileSystem(true)
    , m_safe_buffer(false)
    , m_max_buffer_size(-1)
    , m_ca(new X509Cert())
{
    m_fname = jsFilename;

    static v8::Isolate::CreateParams create_params;
    static ShellArrayBufferAllocator array_buffer_allocator;
    static v8::StartupData blob;

    if (blob.data == NULL) {
        v8::SnapshotCreator creator;
        v8::Isolate* isolate = creator.GetIsolate();
        {
            v8::HandleScope handle_scope(isolate);
            v8::Local<v8::Context> context = v8::Context::New(isolate);
            creator.SetDefaultContext(context);
        }
        blob = creator.CreateBlob(v8::SnapshotCreator::FunctionCodeHandling::kClear);
    }

    create_params.snapshot_blob = &blob;
    create_params.array_buffer_allocator = &array_buffer_allocator;

    m_isolate = v8::Isolate::New(create_params);
    m_isolate->AddGCEpilogueCallback(fb_GCCallback, v8::kGCTypeMarkSweepCompact);

    m_currentFibers++;
    m_idleFibers++;

    exlib::Service::CreateFiber(FiberProcIsolate, this, stack_size * 1024, "JSFiber");
}

Isolate* Isolate::current()
{
    Runtime* rt = Runtime::current();
    if (rt == NULL)
        return NULL;

    return rt->isolate();
}

void Isolate::init()
{
    s_isolates.putTail(this);

    v8::Locker locker(m_isolate);
    v8::Isolate::Scope isolate_scope(m_isolate);
    v8::HandleScope handle_scope(m_isolate);

    v8::Local<v8::Context> _context = v8::Context::New(m_isolate);
    m_context.Reset(m_isolate, _context);

    v8::Context::Scope context_scope(_context);

    Fiber_base::class_info().getModule(this);

    JSFiber::EnterJsScope s;

    if (g_cov && m_id == 1)
        beginCoverage(m_isolate);

    _context->SetEmbedderData(1, v8::Object::New(m_isolate)->GetPrototype());

    static const char* skips[] = { "Master", "argv", "__filename", "__dirname", NULL };
    global_base::class_info().Attach(this, _context->Global(), skips);

    m_topSandbox = new SandBox();
    m_topSandbox->initRoot();

    auto assertion_error = "class AssertionError extends Error {"
                           "   constructor(options) {"
                           "       var { actual, expected, message, operator } = options;"
                           "       if (message) {"
                           "           super(message);"
                           "       } else {"
                           "           if (actual && actual.stack && actual instanceof Error)"
                           "               actual = `${actual.name}: ${actual.message}`;"
                           "           if (expected && expected.stack && expected instanceof Error)"
                           "               expected = `${expected.name}: ${expected.message}`;"
                           "           super(`${JSON.stringify(actual).slice(0, 128)} ` +"
                           "               `${operator} ${JSON.stringify(expected).slice(0, 128)}`);"
                           "       }"
                           "       this.generatedMessage = !message;"
                           "       this.name = 'AssertionError [ERR_ASSERTION]';"
                           "       this.code = 'ERR_ASSERTION';"
                           "       this.actual = actual;"
                           "       this.expected = expected;"
                           "       this.operator = operator;"
                           "   }"
                           "}"
                           "AssertionError;";

    v8::Local<v8::Script> script = v8::Script::Compile(_context, NewString(assertion_error)).ToLocalChecked();
    v8::MaybeLocal<v8::Value> result = script->Run(_context);
    v8::Local<v8::Object> AssertionError = result.ToLocalChecked().As<v8::Object>();
    m_AssertionError.Reset(m_isolate, AssertionError);
}

static result_t syncExit(Isolate* isolate)
{
    v8::HandleScope handle_scope(isolate->m_isolate);
    JSFiber::EnterJsScope s;
    JSTrigger t(isolate->m_isolate, process_base::class_info().getModule(isolate));
    v8::Local<v8::Value> code = v8::Number::New(isolate->m_isolate, isolate->m_exitCode);
    bool r;

    t._emit("beforeExit", &code, 1, r);
    if (s_iso_ref == 1) {
        if (isolate->m_hr >= 0)
            process_base::exit();
        else
            process_base::exit(isolate->m_hr);
    }

    return 0;
}

void Isolate::Ref()
{
    s_iso_ref.inc();
}

void Isolate::Unref(int32_t hr)
{
    if (s_iso_ref.dec() == 0) {
        Isolate* isolate = s_isolates.head();
        isolate->m_hr = hr;
        syncCall(isolate, syncExit, isolate);
    }
}

void Isolate::start_profiler()
{
    if (g_prof) {
        char name[32];
        obj_ptr<Timer_base> tm;
        sprintf(name, "fibjs-%08x.log", (uint32_t)(intptr_t)this);
        profiler_base::start(name, -1, g_prof_interval, tm);
    }
}
void InvokeApiInterruptCallbacks(v8::Isolate* isolate);
static result_t js_timer(Isolate* isolate)
{
    JSFiber::EnterJsScope s;
    isolate->m_has_timer = 0;
    InvokeApiInterruptCallbacks(isolate->m_isolate);
    return 0;
}

void Isolate::RequestInterrupt(v8::InterruptCallback callback, void* data)
{
    m_isolate->RequestInterrupt(callback, data);
    if (m_has_timer.CompareAndSwap(0, 1) == 0)
        syncCall(this, js_timer, this);
}

} /* namespace fibjs */
