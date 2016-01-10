
#include <locale.h>

#include <string.h>
#include <time.h>
#include "ifs/global.h"
#include "ifs/process.h"
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

Isolate::Isolate(const char *fname) :
    m_id((int32_t)s_iso_id.inc()), m_test_setup_bbd(false), m_test_setup_tdd(false),
    m_currentFibers(0), m_idleFibers(0), m_loglevel(console_base::_NOTSET), m_interrupt(false)
{
    if (fname)
        m_fname = fname;

    m_currentFibers++;
    m_idleFibers ++;
    Create(FiberBase::fiber_proc, this, stack_size * 1024);
}

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

void Isolate::Run()
{
    s_isolates.putTail(this);

    Runtime rt;
    rt.m_pDateCache = &m_dc;
    Runtime::reg(&rt);

    v8::Isolate::CreateParams create_params;
    ShellArrayBufferAllocator array_buffer_allocator;
    create_params.array_buffer_allocator = &array_buffer_allocator;

    m_isolate = v8::Isolate::New(create_params);
    v8::Locker locker(m_isolate);
    v8::Isolate::Scope isolate_scope(m_isolate);

    v8::HandleScope handle_scope(m_isolate);

    v8::Local<v8::Context> _context = v8::Context::New(m_isolate);
    v8::Context::Scope context_scope(_context);

    v8::Local<v8::Object> glob = _context->Global();
    static const char* skips[] = {"repl", "argv", NULL};
    global_base::class_info().Attach(this, glob, skips);

    m_context.Reset(m_isolate, _context);
    m_global.Reset(m_isolate, glob);

    result_t hr;

    {
        JSFiber::scope s;
        m_topSandbox = new SandBox();

        m_topSandbox->initRoot();
        if (!m_fname.empty())
        {
            v8::Local<v8::Array> argv;
            v8::Local<v8::Value> replFunc;

            global_base::get_argv(argv);
            replFunc = global_base::class_info().getFunction(this)->Get(
                           v8::String::NewFromUtf8(m_isolate, "repl"));

            hr = s.m_hr = m_topSandbox->run(m_fname.c_str(), argv, replFunc);
        }
        else
        {
            v8::Local<v8::Array> cmds = v8::Array::New(m_isolate);
            hr = s.m_hr = m_topSandbox->repl(cmds);
        }
    }

    process_base::exit(hr);
}

void init_rt();
void init_argv(int32_t argc, char **argv);
void init_prof();
void init_acThread();
void init_logger();
void init_net();
void init_fiber();
void init_sandbox();
bool options(int32_t* argc, char *argv[]);

}

int32_t main(int32_t argc, char *argv[])
{
    ::setlocale(LC_ALL, "");

    if ( fibjs::options(&argc, argv) )
        return 0;

    fibjs::init_prof();

    fibjs::init_argv(argc, argv);

    srand((unsigned int)time(0));

    int32_t i;

    for (i = 1; (i < argc) && (argv[i][0] == '-'); i ++);

    const char *fname = NULL;
    if (i < argc)
        fname = argv[i];

    fibjs::Isolate* isolate = new fibjs::Isolate(fname);
    isolate->bindCurrent();

    fibjs::init_rt();

    fibjs::init_sandbox();
    fibjs::init_acThread();
    fibjs::init_logger();
    fibjs::init_net();
    fibjs::init_fiber();

    v8::Platform *platform = v8::platform::CreateDefaultPlatform();
    v8::V8::InitializePlatform(platform);

    v8::V8::Initialize();

    isolate->Run();

    return 0;
}
