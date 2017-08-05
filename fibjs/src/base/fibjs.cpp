
#include <locale.h>

#include <string.h>
#include <time.h>
#include "object.h"
#include "ifs/os.h"
#include "ifs/global.h"
#include "ifs/zip.h"
#include "SandBox.h"
#include "Fiber.h"
#include "include/libplatform/libplatform.h"
#include "Stat.h"
#include "utf8.h"
#include "path.h"
#include "unzip/include/unzip.h"

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
void options(int32_t argc, char* argv[]);

exlib::string s_root;
static exlib::string s_start;

void init()
{
    ::setlocale(LC_ALL, "");

    int32_t cpus = 0;

    process_base::cwd(s_root);

    os_base::cpuNumbers(cpus);
    if (cpus < 2)
        cpus = 2;

    exlib::Service::init(cpus + 1);

    // init_prof();
    init_date();
    init_rt();
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

result_t start_fiber(int32_t n)
{
    Isolate* isolate = new Isolate(s_start);
    syncCall(isolate, main_fiber, isolate);
    return 0;
}

void main(int32_t argc, char* argv[])
{
    exlib::string exePath;
    std::vector<char*> ptrArg;
    int32_t i;

    process_base::get_execPath(exePath);

    unzFile unz;
    if ((unz = unzOpen64(exePath.c_str())) != NULL) {
        unzClose(unz);

        exePath.append(1, '$');
        ptrArg.resize(argc + 1);

        ptrArg[0] = argv[0];
        ptrArg[1] = exePath.c_buffer();
        for (i = 1; i < argc; i++)
            ptrArg[i + 1] = argv[i];

        argv = &ptrArg[0];
        argc++;
    }

    init_start_argv(argc, argv);
    for (i = 1; (i < argc) && (argv[i][0] == '-'); i++)
        ;
    options(i, argv);

    init();

    if (i < argc) {
        s_start = s_root;
        resolvePath(s_start, argv[i]);

        if (i != 1) {
            int32_t p = 1;
            for (; i < argc; i++)
                argv[p++] = argv[i];
            argc = p;
        }
    }

    init_argv(argc, argv);

    asyncCall(start_fiber, (int32_t)0);
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
