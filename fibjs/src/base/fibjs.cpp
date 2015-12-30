
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

void init_argv(int32_t argc, char **argv);
void init_prof();
void init_acThread();
void init_logger();
void init_net();
void init_fiber();
bool options(int32_t* argc, char *argv[]);

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

void _main(const char *fname)
{
    exlib::Service::init();
    init_acThread();
    init_logger();
    init_net();

    v8::Platform *platform = v8::platform::CreateDefaultPlatform();
    v8::V8::InitializePlatform(platform);

    v8::V8::Initialize();

    Isolate is;
    Isolate::reg(&is);
    s_isolates.putTail(&is);

    Isolate* isolate = Isolate::now();

    v8::Isolate::CreateParams create_params;
    ShellArrayBufferAllocator array_buffer_allocator;
    create_params.array_buffer_allocator = &array_buffer_allocator;

    isolate->m_service = exlib::Service::current();
    isolate->m_isolate = v8::Isolate::New(create_params);
    v8::Locker locker(isolate->m_isolate);
    v8::Isolate::Scope isolate_scope(isolate->m_isolate);

    v8::HandleScope handle_scope(isolate->m_isolate);

    v8::Local<v8::Context> _context = v8::Context::New(isolate->m_isolate);
    v8::Context::Scope context_scope(_context);

    v8::Local<v8::Object> glob = _context->Global();
    static const char* skips[] = {"repl", "argv", NULL};
    global_base::class_info().Attach(isolate, glob, skips);

    isolate->m_context.Reset(isolate->m_isolate, _context);
    isolate->m_global.Reset(isolate->m_isolate, glob);

    init_fiber();

    result_t hr;

    v8::Local<v8::Value> replFunc;

    replFunc = global_base::class_info().getFunction(isolate)->Get(
                   v8::String::NewFromUtf8(isolate->m_isolate, "repl"));

    JSFiber *fb = new JSFiber();
    {
        JSFiber::scope s(fb);
        isolate->m_topSandbox = new SandBox();

        isolate->m_topSandbox->initRoot();
        if (fname)
        {
            v8::Local<v8::Array> argv;

            global_base::get_argv(argv);
            hr = s.m_hr = isolate->m_topSandbox->run(fname, argv, replFunc);
        }
        else
        {
            v8::Local<v8::Array> cmds = v8::Array::New(isolate->m_isolate);
            hr = s.m_hr = isolate->m_topSandbox->repl(cmds);
        }
    }

    process_base::exit(hr);

    isolate->m_isolate->Dispose();

    v8::V8::ShutdownPlatform();
    delete platform;

    isolate->m_context.Reset();
}

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

    if (i < argc)
        fibjs::_main(argv[i]);
    else
        fibjs::_main(NULL);

    return 0;
}
