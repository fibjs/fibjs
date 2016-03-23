
#include <locale.h>

#include <string.h>
#include <time.h>
#include "ifs/os.h"
#include "ifs/global.h"
#include "SandBox.h"
#include "Fiber.h"
#include "include/libplatform/libplatform.h"

namespace fibjs
{

void init_date();
void init_rt();
void init_argv(int32_t argc, char **argv);
void init_start_argv(int32_t argc, char **argv);
void init_prof();
void init_cipher();
void init_acThread();
void init_logger();
void init_aio();
void init_fiber();
void init_sandbox();
void init_Task(int32_t vms);
bool options(int32_t* argc, char *argv[]);

void init(int32_t argc, char *argv[])
{
    ::setlocale(LC_ALL, "");

    int32_t cpus = 0;

    os_base::CPUs(cpus);
    if (cpus < 2)
        cpus = 2;

    exlib::Service::init(cpus + 1);

    init_start_argv(argc, argv);

    if (options(&argc, argv))
        _exit(0);

    init_prof();
    init_argv(argc, argv);
    init_date();
    init_rt();
    init_sandbox();
    init_cipher();
    init_acThread();
    init_logger();
    init_aio();
    init_fiber();

    srand((unsigned int)time(0));

    v8::Platform *platform = v8::platform::CreateDefaultPlatform();
    v8::V8::InitializePlatform(platform);

    v8::V8::Initialize();

    init_Task(cpus);
}

static void main_fiber(Isolate* isolate)
{
    result_t hr;

    if (!isolate->m_fname.empty())
    {
        JSFiber::scope s;
        v8::Local<v8::Array> argv;

        global_base::get_argv(argv);

        hr = s.m_hr = isolate->m_topSandbox->run(
                          isolate->m_fname.c_str(), argv, true);
    }
    else
    {
        JSFiber::scope s;
        v8::Local<v8::Array> cmds = v8::Array::New(isolate->m_isolate);
        hr = s.m_hr = isolate->m_topSandbox->repl(cmds);
    }

    process_base::exit(hr);
}

void main(int32_t argc, char *argv[])
{
    init(argc, argv);

    int32_t i;

    for (i = 1; (i < argc) && (argv[i][0] == '-'); i ++);

    const char *fname = NULL;
    if (i < argc)
        fname = argv[i];

    Isolate* isolate = new Isolate(fname);
    syncCall(isolate, main_fiber, isolate);
    exlib::Service::dispatch();
}

}

int32_t main(int32_t argc, char *argv[])
{
    fibjs::main(argc, argv);
    return 0;
}
