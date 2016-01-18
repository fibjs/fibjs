
#include <locale.h>

#include <string.h>
#include <time.h>
#include "ifs/global.h"
#include "ifs/process.h"
#include "ifs/os.h"
#include "console.h"
#include "SandBox.h"
#include "Fiber.h"
#include "utf8.h"
#include "include/libplatform/libplatform.h"

namespace fibjs
{

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

exlib::LockedList<Isolate> s_isolates;
exlib::atomic s_iso_id;
extern int32_t stack_size;

bool Isolate::rt::g_trace = false;

inline JSFiber* saveTrace()
{
    JSFiber* fiber = JSFiber::current();
    assert(fiber != 0);
    fiber->m_traceInfo = traceInfo(300);
    return fiber;
}

Isolate::rt::rt() :
    m_fiber(g_trace ? saveTrace() : NULL),
    unlocker(m_isolate->m_isolate)
{
}

Isolate::rt::~rt()
{
    if (m_fiber)
        m_fiber->m_traceInfo.resize(0);
}

static void fb_GCCallback(v8::Isolate* js_isolate, v8::GCType type, v8::GCCallbackFlags flags)
{
    Isolate *isolate = Isolate::current();
    exlib::linkitem* p;

    isolate->m_weakLock.lock();
    while ((p = isolate->m_weak.getHead()) != 0)
        object_base::gc_callback(p);
    isolate->m_weakLock.unlock();
}

void *init_proc(void *p)
{
    Isolate* isolate = (Isolate*)p;
    Runtime rt(isolate);

    isolate->init();
    return FiberBase::fiber_proc(p);
}

Isolate::Isolate(const char *fname) :
    m_id((int32_t)s_iso_id.inc()),
    m_test_setup_bbd(false), m_test_setup_tdd(false), m_test(NULL),
    m_currentFibers(0), m_idleFibers(0),
    m_loglevel(console_base::_NOTSET), m_interrupt(false)
{
    if (fname)
        m_fname = fname;

    m_currentFibers++;
    m_idleFibers ++;

    exlib::Service::Create(init_proc, this, stack_size * 1024);
}

Isolate* Isolate::current()
{
    return Runtime::current().isolate();
}

static void main_fiber(Isolate* isolate)
{
    result_t hr;

    if (!isolate->m_fname.empty())
    {
        JSFiber::scope s;
        v8::Local<v8::Array> argv;
        v8::Local<v8::Value> replFunc;

        global_base::get_argv(argv);
        replFunc = global_base::class_info().getFunction(isolate)->Get(
                       v8::String::NewFromUtf8(isolate->m_isolate, "repl"));

        hr = s.m_hr = isolate->m_topSandbox->run(
                          isolate->m_fname.c_str(), argv, replFunc);
    }
    else
    {
        JSFiber::scope s;
        v8::Local<v8::Array> cmds = v8::Array::New(isolate->m_isolate);
        hr = s.m_hr = isolate->m_topSandbox->repl(cmds);
    }

    process_base::exit(hr);
}

void Isolate::init()
{
    static v8::Isolate::CreateParams create_params;
    static ShellArrayBufferAllocator array_buffer_allocator;
    create_params.array_buffer_allocator = &array_buffer_allocator;

    s_isolates.putTail(this);

    m_isolate = v8::Isolate::New(create_params);
    m_isolate->AddGCPrologueCallback(fb_GCCallback, v8::kGCTypeMarkSweepCompact);

    v8::Locker locker(m_isolate);
    v8::Isolate::Scope isolate_scope(m_isolate);
    v8::HandleScope handle_scope(m_isolate);

    v8::Local<v8::Context> _context = v8::Context::New(m_isolate);
    m_context.Reset(m_isolate, _context);

    v8::Local<v8::Object> glob = _context->Global();
    m_global.Reset(m_isolate, glob);

    v8::Context::Scope context_scope(_context);

    m_topSandbox = new SandBox();
    m_topSandbox->initRoot();

    static const char* skips[] = {"repl", "argv", NULL};
    global_base::class_info().Attach(this, glob, skips);

    syncCall(this, main_fiber, this);
}

void init_date();
void init_rt();
void init_argv(int32_t argc, char **argv);
void init_prof();
void init_acThread();
void init_logger();
void init_net();
void init_fiber();
void init_sandbox();
bool options(int32_t* argc, char *argv[]);

void init(int32_t argc, char *argv[])
{
    ::setlocale(LC_ALL, "");

    if (options(&argc, argv))
        _exit(0);

    int32_t cpus = 0;

    os_base::CPUs(cpus);
    if (cpus < 2)
        cpus = 2;

    exlib::Service::init(cpus + 1);

    init_prof();
    init_argv(argc, argv);
    init_date();
    init_rt();
    init_sandbox();
    init_acThread();
    init_logger();
    init_net();
    init_fiber();

    srand((unsigned int)time(0));
}

}

int32_t main(int32_t argc, char *argv[])
{
    fibjs::init(argc, argv);

    int32_t i;

    for (i = 1; (i < argc) && (argv[i][0] == '-'); i ++);

    const char *fname = NULL;
    if (i < argc)
        fname = argv[i];

    v8::Platform *platform = v8::platform::CreateDefaultPlatform();
    v8::V8::InitializePlatform(platform);

    v8::V8::Initialize();

    new fibjs::Isolate(fname);
    exlib::Service::dispatch();

    return 0;
}
