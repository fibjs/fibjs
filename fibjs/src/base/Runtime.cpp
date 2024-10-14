/*
 * Runtime.cpp
 *
 *  Created on: Jul 23, 2012
 *      Author: lion
 */

#include "object.h"
#include "ifs/os.h"
#include "ifs/fs.h"
#include "ifs/process.h"
#include "options.h"
#include "Runtime.h"
#include "fibjs.h"
#include "Buffer.h"
#include <locale.h>
#include "include/libplatform/libplatform.h"

#define POSTJECT_SENTINEL_FUSE "FIBJS_FUSE_fe21d3488eb4cdf267e5ea624f2006ce"
#include "postject-api.h"
#undef POSTJECT_SENTINEL_FUSE

namespace fibjs {

void InitializeDateUtils();
void InitializeAcPool();
void InitializeAsyncIOThread();
void initializeUVAsyncThread();
void init_signal();
void init_sym();
void init_binding();

void init_argv(int32_t argc, char** argv);
void init_start_argv(int32_t argc, char** argv);
void options(int32_t& pos, char* argv[]);
result_t ifZipFile(exlib::string filename, bool& retVal);

void init_tls();

exlib::string s_root;
v8::Platform* g_default_platform;

static void createBasisForFiberLoop(Isolate::platform_creator get_platform)
{
    int32_t cpus = 0;

    process_base::cwd(s_root);

    os_base::cpuNumbers(cpus);
    if (cpus < 2)
        cpus = 2;

    init_binding();
    exlib::Service::init(cpus + 1);

    InitializeDateUtils();
    InitializeAcPool();
    InitializeAsyncIOThread();
    initializeUVAsyncThread();
    init_tls();

#ifdef Linux
    init_sym();
#endif

    srand((unsigned int)time(0));

    g_default_platform = get_platform ? get_platform() : v8::platform::NewDefaultPlatform().release();
    v8::V8::InitializePlatform(g_default_platform);
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
        virtual void Run()
        {
            int32_t argc = m_argc;
            char** argv = m_argv.data();

            const void* sea_data = nullptr;
            size_t sea_size = 0;
            if (postject_has_resource())
                sea_data = postject_find_resource("APP", &sea_size, nullptr);

            exlib::string exePath;
            std::vector<char*> ptrArg;
            if (sea_size > 0) {
                process_base::get_execPath(exePath);

                exePath.append(1, '$');
                ptrArg.resize(argc + 1);

                ptrArg[0] = argv[0];
                ptrArg[1] = exePath.data();

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

            Isolate* isolate = new Isolate(m_fibjsEntry, g_exec_code);
            isolate->sync([this, isolate, sea_data, sea_size]() -> int {
                if (sea_size > 0) {
                    exlib::string exePath;
                    process_base::get_execPath(exePath);

                    obj_ptr<Buffer> data = new Buffer((const uint8_t*)sea_data, (int32_t)sea_size);
                    fs_base::setZipFS(exePath, data);
                }

                return m_jsFiber(isolate);
            });
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

result_t setRuntimeError(result_t code, const char* err)
{
    return Runtime::setError(code, err);
}

} /* namespace fibjs */
