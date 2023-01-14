/*
 * Runtime.cpp
 *
 *  Created on: Jul 23, 2012
 *      Author: lion
 */

#include "object.h"
#include "options.h"
#include "Runtime.h"
#include "fibjs.h"
#include "ifs/os.h"
#include "ifs/process.h"
#include <locale.h>
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

static void createBasisForFiberLoop(Isolate::platform_creator get_platform)
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

#ifdef Linux
    init_sym();
#endif

    srand((unsigned int)time(0));

    v8::V8::InitializePlatform(get_platform ? get_platform() : v8::platform::NewDefaultPlatform().release());
    v8::V8::Initialize();
}

void start(int32_t argc, char** argv, result_t (*jsEntryFiber)(Isolate*), Isolate::platform_creator get_platform)
{
    class EntryThread : public exlib::OSThread {
    public:
        EntryThread(int32_t argc, char** argv, result_t (*jsFiber)(Isolate*), Isolate::platform_creator get_platform)
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

    public:
        exlib::OSSemaphore m_sem;

    private:
        int32_t m_argc;
        std::vector<char*> m_argv;
        exlib::string m_fibjsEntry;
        result_t (*m_jsFiber)(Isolate*);
        Isolate::platform_creator m_get_platform;
    };

    init_signal();
    EntryThread* entryThread = new EntryThread(argc, argv, jsEntryFiber, get_platform);
    entryThread->start();

    entryThread->m_sem.Wait();
}

static exlib::fiber_local<Runtime*> s_rt;

void Runtime::RegInThread()
{
    s_rt = this;
}

Runtime* Runtime::current()
{
    return s_rt;
}

} /* namespace fibjs */
