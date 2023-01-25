/*
 * process.cpp
 *
 *  Created on: Sep 19, 2012
 *      Author: lion
 */

#include "object.h"
#include "version.h"
#include "ifs/process.h"
#include "ifs/os.h"
#include "ifs/global.h"
#include "ifs/util.h"
#include "Fiber.h"
#include "EventEmitter.h"
#include "UVStream.h"
#include "BufferedStream.h"
#include "ChildProcess.h"
#include <vector>
#include "options.h"

#ifdef _WIN32
#include <psapi.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <io.h>

#include "utf8.h"
#else

#include <unistd.h>
#include "editline/include/editline.h"

#define _fileno fileno
inline int32_t _umask(int32_t m)
{
    return ::umask(m);
}

#endif

namespace fibjs {

DECLARE_MODULE_WITH_CONSTRUCTOR_EX(process, process);

RootModule_process::RootModule_process()
{
    ClassData& cd = process_base::class_info().data();

    char buffer[16];
    size_t sz = sizeof(buffer);
    if (uv_os_getenv("NODE_CHANNEL_FD", buffer, &sz)) {
        cd.mc -= 2;
        cd.pc--;
    }
}

static std::vector<char*> s_argv;
static std::vector<char*> s_start_argv;

void init_start_argv(int32_t argc, char** argv)
{
    int32_t i;

    s_start_argv.resize(argc);
    for (i = 0; i < argc; i++)
        s_start_argv[i] = argv[i];
}

void init_argv(int32_t argc, char** argv)
{
    int32_t i, j;

    s_argv.resize(argc);
    for (i = 0; i < argc; i++)
        s_argv[i] = argv[i];

    int32_t df = 0;

    for (i = 0; i < (int32_t)s_start_argv.size(); i++) {
        char* arg = s_start_argv[i];

        if (df)
            s_start_argv[i - df] = arg;

        for (j = 0; j < argc; j++)
            if (arg == argv[j]) {
                df++;
                break;
            }
    }

    if (df)
        s_start_argv.resize(s_start_argv.size() - df);
}

result_t process_base::get_argv(v8::Local<v8::Array>& retVal)
{
    Isolate* isolate = Isolate::current();
    v8::Local<v8::Context> context = isolate->context();
    v8::Local<v8::Array> args = v8::Array::New(isolate->m_isolate, (int32_t)s_argv.size());

    for (int32_t i = 0; i < (int32_t)s_argv.size(); i++)
        args->Set(context, i, isolate->NewString(s_argv[i])).Check();

    retVal = args;

    return 0;
}

result_t process_base::get_execArgv(v8::Local<v8::Array>& retVal)
{
    Isolate* isolate = Isolate::current();
    v8::Local<v8::Context> context = isolate->context();
    v8::Local<v8::Array> args = v8::Array::New(isolate->m_isolate, (int32_t)s_start_argv.size());
    int32_t i;

    for (i = 0; i < (int32_t)s_start_argv.size(); i++)
        args->Set(context, i, isolate->NewString(s_start_argv[i])).Check();

    retVal = args;

    return 0;
}

result_t process_base::get_version(exlib::string& retVal)
{
    retVal.assign(1, 'v');
    retVal.append(fibjs_version);
    return 0;
}

result_t process_base::get_versions(v8::Local<v8::Object>& retVal)
{
    return util_base::buildInfo(retVal);
}

result_t process_base::get_exitCode(int32_t& retVal)
{
    retVal = Isolate::current()->m_exitCode;
    return 0;
}

result_t process_base::set_exitCode(int32_t newVal)
{
    Isolate::current()->m_exitCode = newVal;
    return 0;
}

result_t process_base::umask(int32_t mask, int32_t& retVal)
{
    retVal = _umask(mask);
    return 0;
}

result_t process_base::umask(exlib::string mask, int32_t& retVal)
{
    int oct = 0;
    // Parse the octal string.
    for (size_t i = 0; i < mask.length(); i++) {
        char c = mask[i];

        if (c > '7' || c < '0')
            return CHECK_ERROR(Runtime::setError("process: invalid octal string"));

        oct *= 8;
        oct += c - '0';
    }

    retVal = _umask(oct);
    return 0;
}

result_t process_base::umask(int32_t& retVal)
{
    int32_t old = _umask(0);
    retVal = old;
    _umask(old);
    return 0;
}

#define NANOS_PER_SEC 1000000000LL
#define MICROS_PER_SEC 1e6

result_t process_base::hrtime(v8::Local<v8::Array> diff, v8::Local<v8::Array>& retVal)
{
    uint64_t t = uv_hrtime();

    Isolate* isolate = Isolate::current();
    v8::Local<v8::Context> context = isolate->context();

    if (diff->Length() == 2) {
        uint64_t seconds = isolate->toUint32Value(JSValue(diff->Get(context, 0)));
        uint64_t nanos = isolate->toUint32Value(JSValue(diff->Get(context, 1)));
        t -= (seconds * NANOS_PER_SEC) + nanos;
    }

    v8::Local<v8::Array> tuple = v8::Array::New(isolate->m_isolate, 2);
    tuple->Set(context, 0, v8::Integer::NewFromUnsigned(isolate->m_isolate, (uint32_t)(t / NANOS_PER_SEC))).Check();
    tuple->Set(context, 1, v8::Integer::NewFromUnsigned(isolate->m_isolate, t % NANOS_PER_SEC)).Check();

    retVal = tuple;

    return 0;
}

result_t process_base::get_execPath(exlib::string& retVal)
{
    char buf[1024] = "";
    size_t size = sizeof(buf);

    int32_t ret = uv_exepath(buf, &size);
    if (ret < 0)
        return CHECK_ERROR(ret);

    retVal = buf;

    return 0;
}

result_t process_base::get_arch(exlib::string& retVal)
{
    return os_base::arch(retVal);
}

result_t process_base::get_platform(exlib::string& retVal)
{
    return os_base::platform(retVal);
}

result_t process_base::get_pid(int32_t& retVal)
{
    retVal = uv_os_getpid();
    return 0;
}

result_t process_base::get_ppid(int32_t& retVal)
{
    retVal = uv_os_getppid();
    return 0;
}

result_t process_base::get_stdin(obj_ptr<Stream_base>& retVal)
{
    Isolate::current()->get_stdin(retVal);

    return 0;
}

result_t process_base::get_stdout(obj_ptr<Stream_base>& retVal)
{
    Isolate::current()->get_stdout(retVal);

    return 0;
}

result_t process_base::get_stderr(obj_ptr<Stream_base>& retVal)
{
    Isolate::current()->get_stderr(retVal);

    return 0;
}

extern bool g_in_readline;

result_t process_base::exit()
{
    Isolate* isolate = Isolate::current();
    int32_t code = isolate->m_exitCode;

    JSTrigger t(isolate->m_isolate, class_info().getModule(isolate));
    v8::Local<v8::Value> v = v8::Number::New(isolate->m_isolate, code);
    bool r;

    isolate->Ref();
    t._emit("exit", &v, 1, r);

    flushLog();

    if (g_cov != nullptr && isolate->m_id == 1) {
        WriteLcovData(isolate->m_isolate, g_cov);
    }

#ifndef _WIN32
    if (g_in_readline && isatty(_fileno(stdin)))
        rl_deprep_terminal();
#endif

    ::exit(code);

    return 0;
}

result_t process_base::exit(int32_t code)
{
    Isolate* isolate = Isolate::current();
    isolate->m_exitCode = code;
    return process_base::exit();
}

result_t process_base::cpuUsage(v8::Local<v8::Object> previousValue, v8::Local<v8::Object>& retVal)
{
    uv_rusage_t rusage;
    double _user = 0, _system = 0;

    int err = uv_getrusage(&rusage);
    if (err)
        return err;

    result_t hr;
    Isolate* isolate = Isolate::current();
    v8::Local<v8::Context> context = isolate->context();

    hr = GetConfigValue(isolate->m_isolate, previousValue, "user", _user, true);
    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
        return hr;

    hr = GetConfigValue(isolate->m_isolate, previousValue, "system", _system, true);
    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
        return hr;

    _user = MICROS_PER_SEC * rusage.ru_utime.tv_sec + rusage.ru_utime.tv_usec - _user;
    _system = MICROS_PER_SEC * rusage.ru_stime.tv_sec + rusage.ru_stime.tv_usec - _system;

    v8::Local<v8::Object> o = v8::Object::New(isolate->m_isolate);
    o->Set(context, isolate->NewString("user"), v8::Number::New(isolate->m_isolate, _user)).Check();
    o->Set(context, isolate->NewString("system"), v8::Number::New(isolate->m_isolate, _system)).Check();

    retVal = o;

    return 0;
}

extern exlib::atomic g_ExtStringCount;

result_t process_base::memoryUsage(v8::Local<v8::Object>& retVal)
{
    Isolate* isolate = Isolate::current();
    v8::Local<v8::Context> context = isolate->context();
    v8::Local<v8::Object> info = v8::Object::New(isolate->m_isolate);

    size_t rss;
    int32_t ret = uv_resident_set_memory(&rss);
    if (ret < 0)
        return CHECK_ERROR(ret);

    info->Set(context, isolate->NewString("rss"), v8::Number::New(isolate->m_isolate, (double)rss)).Check();

    v8::HeapStatistics v8_heap_stats;
    isolate->m_isolate->GetHeapStatistics(&v8_heap_stats);

    info->Set(context, isolate->NewString("heapTotal"),
        v8::Number::New(isolate->m_isolate, (double)v8_heap_stats.total_heap_size())).Check();
    info->Set(context, isolate->NewString("heapUsed"),
        v8::Number::New(isolate->m_isolate, (double)v8_heap_stats.used_heap_size())).Check();
    info->Set(context, isolate->NewString("external"),
        v8::Number::New(isolate->m_isolate, (double)v8_heap_stats.external_memory())).Check();

    v8::Local<v8::Object> objs;
    object_base::class_info().dump(objs);
    info->Set(context, isolate->NewString("nativeObjects"), objs).Check();
    info->Set(context, isolate->NewString("ExtStrings"),
        v8::Number::New(isolate->m_isolate, (double)g_ExtStringCount.value())).Check();

    retVal = info;

    return 0;
}

result_t process_base::uptime(double& retVal)
{
    int32_t ret = uv_uptime(&retVal);
    if (ret < 0)
        return CHECK_ERROR(ret);
    return 0;
}

result_t process_base::cwd(exlib::string& retVal)
{
    char buf[1024] = "";
    size_t size = sizeof(buf);

    if (uv_cwd(buf, &size))
        return CHECK_ERROR(LastError());

    retVal = buf;
    return 0;
}

result_t process_base::chdir(exlib::string directory)
{
    if (uv_chdir(directory.c_str()))
        return CHECK_ERROR(LastError());

    return 0;
}

result_t process_base::nextTick(v8::Local<v8::Function> func, OptArgs args)
{
    obj_ptr<Fiber_base> retVal;
    return JSFiber::New(func, args, retVal);
}

result_t process_base::binding(exlib::string name, v8::Local<v8::Value>& retVal)
{
    Isolate* isolate = Isolate::current();

    if (name == "EventEmitter")
        retVal = EventEmitter_base::class_info().getModule(isolate);
    else if (name == "Buffer")
        retVal = Buffer_base::class_info().getModule(isolate);
    else {
        RootModule* pModule = RootModule::g_root;

        while (pModule)
            if (name == pModule->name()) {
                retVal = pModule->getModule(isolate);
                break;
            } else
                pModule = pModule->m_next;
    }

    return 0;
}

result_t process_base::getgid(int32_t& retVal)
{
#ifndef _WIN32
    retVal = ::getgid();
#else
    retVal = 0;
#endif
    return 0;
}

result_t process_base::getuid(int32_t& retVal)
{
#ifndef _WIN32
    retVal = ::getuid();
#else
    retVal = 0;
#endif
    return 0;
}

result_t process_base::setgid(int32_t id)
{
#ifndef _WIN32
    ::setgid(id);
#endif
    return 0;
}

result_t process_base::setuid(int32_t id)
{
#ifndef _WIN32
    ::setuid(id);
#endif
    return 0;
}

result_t process_base::get_connected(bool& retVal)
{
    Isolate* isolate = Isolate::current();

    retVal = !!isolate->m_channel;

    return 0;
}

result_t process_base::disconnect()
{
    Isolate* isolate = Isolate::current();

    if (!isolate->m_channel)
        return CHECK_ERROR(Runtime::setError("process: IPC channel is already disconnected."));

    obj_ptr<Stream_base> _channel = isolate->m_channel;
    isolate->m_channel.Release();

    JSTrigger t(isolate->m_isolate, process_base::class_info().getModule(isolate));
    bool r;

    t._emit("disconnect", NULL, 0, r);

    _channel->ac_close();

    return 0;
}

result_t process_base::send(v8::Local<v8::Value> msg)
{
    Isolate* isolate = Isolate::current();

    if (!isolate->m_channel)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    return ChildProcess::Ipc::send(isolate->m_channel, msg);
}

}
