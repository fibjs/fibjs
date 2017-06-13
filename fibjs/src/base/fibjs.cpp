
#include <locale.h>

#include <string.h>
#include <time.h>
#include "object.h"
#include "ifs/os.h"
#include "ifs/global.h"
#include "SandBox.h"
#include "Fiber.h"
#include "include/libplatform/libplatform.h"
#include "Stat.h"
#include "utf8.h"

namespace fibjs {

void init_date();
void init_rt();
void init_argv(int32_t argc, char** argv);
void init_start_argv(int32_t argc, char** argv);
void init_prof();
void init_cipher();
void init_acThread();
void init_gui();
void init_logger();
void init_aio();
void init_fs();
void init_fiber();
void init_sandbox();
bool options(int32_t* argc, char* argv[]);

void init(int32_t& argc, char* argv[])
{
    ::setlocale(LC_ALL, "");

    int32_t cpus = 0;

    os_base::cpuNumbers(cpus);
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
    init_gui();
    init_logger();
    init_aio();
    init_fs();
    init_fiber();

    srand((unsigned int)time(0));

    v8::Platform* platform = v8::platform::CreateDefaultPlatform();
    v8::V8::InitializePlatform(platform);

    v8::V8::Initialize();
}

static result_t main_fiber(Isolate* isolate)
{
    JSFiber::scope s;

    if (!isolate->m_fname.empty()) {
        v8::Local<v8::Array> argv;

        global_base::get_argv(argv);
        s.m_hr = isolate->m_topSandbox->run_main(isolate->m_fname, argv);
    } else {
        v8::Local<v8::Array> cmds = v8::Array::New(isolate->m_isolate);
        s.m_hr = isolate->m_topSandbox->repl(cmds);
    }

    return s.m_hr;
}

void main(int32_t argc, char* argv[])
{
    init(argc, argv);

    int32_t i;

    for (i = 1; (i < argc) && (argv[i][0] == '-'); i++)
        ;

    exlib::string fname;
    if (i < argc)
        fname = argv[i];

    Isolate* isolate = new Isolate(fname);
    syncCall(isolate, main_fiber, isolate);
    exlib::Service::dispatch();
}
}

#ifdef _WIN32

#ifdef _CONSOLE
int32_t main()
#else
int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#endif
{
    int32_t argc;
    char** argv;

    LPWSTR* szArglist = CommandLineToArgvW(GetCommandLineW(), &argc);
    std::vector<exlib::string> strArgList;
    std::vector<char*> ptrArgList;
    int32_t i;

    strArgList.resize(argc);
    ptrArgList.resize(argc);
    for (i = 0; i < argc; i++) {
        strArgList[i] = fibjs::utf16to8String(szArglist[i]);
        ptrArgList[i] = strArgList[i].c_buffer();
    }

    argv = ptrArgList.data();

    fibjs::main(argc, argv);
    return 0;
}

#else

int32_t main(int32_t argc, char* argv[])
{
    fibjs::main(argc, argv);
    return 0;
}
#endif
