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
#include "SandBox.h"
#include <vector>
#include <signal.h>
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

extern exlib::string s_root;

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

std::vector<char*> s_argv;
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

    if (isolate->m_argv.IsEmpty()) {
        v8::Local<v8::Context> context = isolate->context();
        v8::Local<v8::Array> args = v8::Array::New(isolate->m_isolate, (int32_t)s_argv.size());

        for (int32_t i = 0; i < (int32_t)s_argv.size(); i++)
            args->Set(context, i, isolate->NewString(s_argv[i])).IsJust();

        isolate->m_argv.Reset(isolate->m_isolate, args);
    }

    retVal = isolate->m_argv.Get(isolate->m_isolate);

    return 0;
}

result_t process_base::set_argv(v8::Local<v8::Array> newVal)
{
    Isolate* isolate = Isolate::current();
    isolate->m_argv.Reset(isolate->m_isolate, newVal);
    return 0;
}

result_t process_base::get_execArgv(v8::Local<v8::Array>& retVal)
{
    Isolate* isolate = Isolate::current();
    v8::Local<v8::Context> context = isolate->context();
    v8::Local<v8::Array> args = v8::Array::New(isolate->m_isolate, (int32_t)s_start_argv.size());
    int32_t i;

    for (i = 0; i < (int32_t)s_start_argv.size(); i++)
        args->Set(context, i, isolate->NewString(s_start_argv[i])).IsJust();

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
    tuple->Set(context, 0, v8::Integer::NewFromUnsigned(isolate->m_isolate, (uint32_t)(t / NANOS_PER_SEC))).IsJust();
    tuple->Set(context, 1, v8::Integer::NewFromUnsigned(isolate->m_isolate, t % NANOS_PER_SEC)).IsJust();

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

result_t process_base::get_release(v8::Local<v8::Object>& retVal)
{
    Isolate* isolate = Isolate::current();
    v8::Local<v8::Context> context = isolate->context();
    v8::Local<v8::Object> obj = v8::Object::New(isolate->m_isolate);

    obj->Set(context, isolate->NewString("name"), isolate->NewString("node")).IsJust();
    obj->Set(context, isolate->NewString("sourceUrl"), isolate->NewString("https://github.com/fibjs/fibjs")).IsJust();
    obj->Set(context, isolate->NewString("venderUrl"), isolate->NewString("https://github.com/fibjs/fibjs_vender")).IsJust();

    retVal = obj;
    return 0;
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

#ifdef _WIN32
    TerminateProcess(GetCurrentProcess(), code);
#else
    if (g_in_readline && isatty(_fileno(stdin)))
        rl_deprep_terminal();
    ::_exit(code);
#endif

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

    hr = GetConfigValue(previousValue, "user", _user, true);
    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
        return hr;

    hr = GetConfigValue(previousValue, "system", _system, true);
    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
        return hr;

    _user = MICROS_PER_SEC * rusage.ru_utime.tv_sec + rusage.ru_utime.tv_usec - _user;
    _system = MICROS_PER_SEC * rusage.ru_stime.tv_sec + rusage.ru_stime.tv_usec - _system;

    v8::Local<v8::Object> o = v8::Object::New(isolate->m_isolate);
    o->Set(context, isolate->NewString("user"), v8::Number::New(isolate->m_isolate, _user)).IsJust();
    o->Set(context, isolate->NewString("system"), v8::Number::New(isolate->m_isolate, _system)).IsJust();

    retVal = o;

    return 0;
}

result_t process_base::resourceUsage(v8::Local<v8::Object>& retVal)
{
    uv_rusage_t rusage;
    double _user = 0, _system = 0;

    int err = uv_getrusage(&rusage);
    if (err)
        return err;

    Isolate* isolate = Isolate::current();
    v8::Local<v8::Context> context = isolate->context();

    v8::Local<v8::Object> o = v8::Object::New(isolate->m_isolate);
    o->Set(context, isolate->NewString("userCPUTime"),
         v8::Number::New(isolate->m_isolate, (double)(MICROS_PER_SEC * rusage.ru_utime.tv_sec + rusage.ru_utime.tv_usec)))
        .IsJust();
    o->Set(context, isolate->NewString("systemCPUTime"),
         v8::Number::New(isolate->m_isolate, (double)(MICROS_PER_SEC * rusage.ru_stime.tv_sec + rusage.ru_stime.tv_usec)))
        .IsJust();
    o->Set(context, isolate->NewString("maxRSS"), v8::Number::New(isolate->m_isolate, (double)rusage.ru_maxrss)).IsJust();
    o->Set(context, isolate->NewString("sharedMemorySize"), v8::Number::New(isolate->m_isolate, (double)rusage.ru_ixrss)).IsJust();
    o->Set(context, isolate->NewString("unsharedDataSize"), v8::Number::New(isolate->m_isolate, (double)rusage.ru_idrss)).IsJust();
    o->Set(context, isolate->NewString("unsharedStackSize"), v8::Number::New(isolate->m_isolate, (double)rusage.ru_isrss)).IsJust();
    o->Set(context, isolate->NewString("minorPageFault"), v8::Number::New(isolate->m_isolate, (double)rusage.ru_minflt)).IsJust();
    o->Set(context, isolate->NewString("majorPageFault"), v8::Number::New(isolate->m_isolate, (double)rusage.ru_majflt)).IsJust();
    o->Set(context, isolate->NewString("swappedOut"), v8::Number::New(isolate->m_isolate, (double)rusage.ru_nswap)).IsJust();
    o->Set(context, isolate->NewString("fsRead"), v8::Number::New(isolate->m_isolate, (double)rusage.ru_inblock)).IsJust();
    o->Set(context, isolate->NewString("fsWrite"), v8::Number::New(isolate->m_isolate, (double)rusage.ru_oublock)).IsJust();
    o->Set(context, isolate->NewString("ipcSent"), v8::Number::New(isolate->m_isolate, (double)rusage.ru_msgsnd)).IsJust();
    o->Set(context, isolate->NewString("ipcReceived"), v8::Number::New(isolate->m_isolate, (double)rusage.ru_msgrcv)).IsJust();
    o->Set(context, isolate->NewString("signalsCount"), v8::Number::New(isolate->m_isolate, (double)rusage.ru_nsignals)).IsJust();
    o->Set(context, isolate->NewString("voluntaryContextSwitches"), v8::Number::New(isolate->m_isolate, (double)rusage.ru_nvcsw)).IsJust();
    o->Set(context, isolate->NewString("involuntaryContextSwitches"), v8::Number::New(isolate->m_isolate, (double)rusage.ru_nivcsw)).IsJust();
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

    info->Set(context, isolate->NewString("rss"), v8::Number::New(isolate->m_isolate, (double)rss)).IsJust();

    v8::HeapStatistics v8_heap_stats;
    isolate->m_isolate->GetHeapStatistics(&v8_heap_stats);

    info->Set(context, isolate->NewString("heapTotal"),
            v8::Number::New(isolate->m_isolate, (double)v8_heap_stats.total_heap_size()))
        .IsJust();
    info->Set(context, isolate->NewString("heapUsed"),
            v8::Number::New(isolate->m_isolate, (double)v8_heap_stats.used_heap_size()))
        .IsJust();
    info->Set(context, isolate->NewString("external"),
            v8::Number::New(isolate->m_isolate, (double)v8_heap_stats.external_memory()))
        .IsJust();

    v8::Local<v8::Object> objs;
    object_base::class_info().dump(objs);
    if (objs.IsEmpty())
        objs = v8::Object::New(isolate->m_isolate);
    info->Set(context, isolate->NewString("nativeObjects"), objs).IsJust();
    info->Set(context, isolate->NewString("ExtStrings"),
            v8::Number::New(isolate->m_isolate, (double)g_ExtStringCount.value()))
        .IsJust();

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

static exlib::string s_cwd;
static exlib::spinlock s_cwd_lock;

result_t process_base::cwd(exlib::string& retVal)
{
    s_cwd_lock.lock();
    if (s_cwd.empty()) {
        char buf[1024] = "";
        size_t size = sizeof(buf);

        if (uv_cwd(buf, &size)) {
            s_cwd_lock.unlock();
            return CHECK_ERROR(LastError());
        }

        s_cwd = buf;
    }
    retVal = s_cwd;
    s_cwd_lock.unlock();
    return 0;
}

result_t process_base::chdir(exlib::string directory)
{
    if (uv_chdir(directory.c_str()))
        return CHECK_ERROR(LastError());

    s_cwd_lock.lock();
    s_cwd.clear();
    s_cwd_lock.unlock();

    return 0;
}

result_t process_base::nextTick(v8::Local<v8::Function> func, OptArgs args)
{
    obj_ptr<Fiber_base> retVal;
    Isolate* isolate = Isolate::current(func);
    return JSFiber::New(func, args, retVal, isolate->m_microtaskDepth == 0);
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

static RootModule* findNativeModule(exlib::string name)
{
    RootModule* pModule = RootModule::g_root;

    while (pModule) {
        if (name == pModule->name())
            return pModule;
        pModule = pModule->m_next;
    }

    return NULL;
}

static bool isBuiltinModuleName(Isolate* isolate, exlib::string name)
{
    const char* q = name.c_str();
    size_t len = name.length();

    // Native builtin modules such as "path" or "fs", plus the "buffer" module
    if (findNativeModule(name) || name == "buffer")
        return true;

    // Fixed sub-path builtin modules
    if (name == "assert/strict" || name == "util/types" || name == "path/posix" ||
        name == "path/win32")
        return true;

    // "<native>/promises" sub-path modules. A native module exposing a promise
    // flavored module object, such as "fs/promises" or "dns/promises", qualifies.
    if (len > 9 && !qstrcmp(q + len - 9, "/promises")) {
        RootModule* pModule = findNativeModule(exlib::string(q, len - 9));
        if (pModule) {
            v8::Local<v8::Object> mod = pModule->getModule(isolate);
            v8::Local<v8::Context> context = isolate->context();
            v8::Local<v8::Value> promises = mod->Get(context, isolate->NewString("promises"))
                                                .FromMaybe(v8::Local<v8::Value>());
            if (!promises.IsEmpty() && promises->IsObject())
                return true;
        }
    }

    // Embedded JS builtin modules such as "stream", "readline/promises" or
    // "timers/promises", except internal ones
    for (intptr_t i = 0; opt_tools[i].name; i++)
        if (!qstrcmp(opt_tools[i].name, q)) {
            const char* n = opt_tools[i].name;

            return qstrcmp(n, "internal/", 9) && qstrcmp(n, "opt_tools/", 10) &&
                qstrcmp(n, "_stream_", 8);
        }

    return false;
}

result_t process_base::getBuiltinModule(exlib::string id, v8::Local<v8::Value>& retVal)
{
    Isolate* isolate = Isolate::current();

    // Builtin module ids may be given with or without the "node:" prefix.
    // NOTE: must not assign from a pointer into the string itself (aliasing),
    // use substr() to build the stripped copy.
    exlib::string name = id;
    if (!qstrcmp(name.c_str(), "node:", 5))
        name = name.substr(5);

    // Not a builtin module, return undefined instead of throwing
    if (!isBuiltinModuleName(isolate, name))
        return 0;

    // Resolve through the top-level sandbox so that the returned module object
    // is identical to the one returned by require(id)
    return isolate->m_topSandbox->require(id, s_root, retVal, true);
}

result_t process_base::emitWarning(v8::Local<v8::Value> warning, v8::Local<v8::Object> options)
{
    Isolate* isolate = Isolate::current();
    v8::Local<v8::Context> context = isolate->context();

    if (warning->IsString() || warning->IsStringObject()) {
        warning = v8::Exception::Error(warning.As<v8::String>());
    } else if (!warning->IsNativeError())
        return CALL_E_BADVARTYPE;
    v8::Local<v8::Object> opts = warning.As<v8::Object>();

    exlib::string type("Warning");
    GetConfigValue(options, "type", type, true);
    opts->Set(context, isolate->NewString("name"), isolate->NewString(type)).IsJust();

    opts->Set(context, isolate->NewString("code"), options->Get(context, isolate->NewString("code")).FromMaybe(v8::Local<v8::Value>())).IsJust();
    opts->Set(context, isolate->NewString("detail"), options->Get(context, isolate->NewString("detail")).FromMaybe(v8::Local<v8::Value>())).IsJust();

    v8::Local<v8::Value> v = opts;
    (new JSTrigger::AsyncEmitter(isolate, class_info().getModule(isolate)))->emit("warning", &v, 1);

    return 0;
}

result_t process_base::emitWarning(v8::Local<v8::Value> warning, exlib::string type, exlib::string code)
{
    Isolate* isolate = Isolate::current();
    v8::Local<v8::Context> context = isolate->context();

    if (warning->IsString() || warning->IsStringObject()) {
        warning = v8::Exception::Error(warning.As<v8::String>());
    } else if (!warning->IsNativeError())
        return CALL_E_BADVARTYPE;
    v8::Local<v8::Object> opts = warning.As<v8::Object>();

    opts->Set(context, isolate->NewString("name"), isolate->NewString(type)).IsJust();
    opts->Set(context, isolate->NewString("code"), isolate->NewString(code)).IsJust();

    v8::Local<v8::Value> v = opts;
    (new JSTrigger::AsyncEmitter(isolate, class_info().getModule(isolate)))->emit("warning", &v, 1);

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
        return CHECK_ERROR(Runtime::setError(CALL_E_INVALID_CALL, "process.send: IPC channel is not available."));

    return ChildProcess::Ipc::send(isolate->m_channel, msg);
}

static int32_t sig_name_to_number(exlib::string signal)
{
#ifdef SIGHUP
    if (signal == "SIGHUP")
        return SIGHUP;
#endif
#ifdef SIGINT
    if (signal == "SIGINT")
        return SIGINT;
#endif
#ifdef SIGQUIT
    if (signal == "SIGQUIT")
        return SIGQUIT;
#endif
#ifdef SIGILL
    if (signal == "SIGILL")
        return SIGILL;
#endif
#ifdef SIGTRAP
    if (signal == "SIGTRAP")
        return SIGTRAP;
#endif
#ifdef SIGABRT
    if (signal == "SIGABRT")
        return SIGABRT;
#endif
#ifdef SIGIOT
    if (signal == "SIGIOT")
        return SIGIOT;
#endif
#ifdef SIGFPE
    if (signal == "SIGFPE")
        return SIGFPE;
#endif
#ifdef SIGKILL
    if (signal == "SIGKILL")
        return SIGKILL;
#endif
#ifdef SIGBUS
    if (signal == "SIGBUS")
        return SIGBUS;
#endif
#ifdef SIGSEGV
    if (signal == "SIGSEGV")
        return SIGSEGV;
#endif
#ifdef SIGSYS
    if (signal == "SIGSYS")
        return SIGSYS;
#endif
#ifdef SIGPIPE
    if (signal == "SIGPIPE")
        return SIGPIPE;
#endif
#ifdef SIGALRM
    if (signal == "SIGALRM")
        return SIGALRM;
#endif
#ifdef SIGTERM
    if (signal == "SIGTERM")
        return SIGTERM;
#endif
#ifdef SIGURG
    if (signal == "SIGURG")
        return SIGURG;
#endif
#ifdef SIGSTOP
    if (signal == "SIGSTOP")
        return SIGSTOP;
#endif
#ifdef SIGTSTP
    if (signal == "SIGTSTP")
        return SIGTSTP;
#endif
#ifdef SIGCONT
    if (signal == "SIGCONT")
        return SIGCONT;
#endif
#ifdef SIGCHLD
    if (signal == "SIGCHLD")
        return SIGCHLD;
#endif
#ifdef SIGTTIN
    if (signal == "SIGTTIN")
        return SIGTTIN;
#endif
#ifdef SIGTTOU
    if (signal == "SIGTTOU")
        return SIGTTOU;
#endif
#ifdef SIGIO
    if (signal == "SIGIO")
        return SIGIO;
#endif
#ifdef SIGXCPU
    if (signal == "SIGXCPU")
        return SIGXCPU;
#endif
#ifdef SIGXFSZ
    if (signal == "SIGXFSZ")
        return SIGXFSZ;
#endif
#ifdef SIGVTALRM
    if (signal == "SIGVTALRM")
        return SIGVTALRM;
#endif
#ifdef SIGPROF
    if (signal == "SIGPROF")
        return SIGPROF;
#endif
#ifdef SIGWINCH
    if (signal == "SIGWINCH")
        return SIGWINCH;
#endif
#ifdef SIGINFO
    if (signal == "SIGINFO")
        return SIGINFO;
#endif
#ifdef SIGUSR1
    if (signal == "SIGUSR1")
        return SIGUSR1;
#endif
#ifdef SIGUSR2
    if (signal == "SIGUSR2")
        return SIGUSR2;
#endif
    return -1;
}

result_t process_base::kill(int32_t pid, int32_t signal)
{
    int err = uv_kill(pid, signal);
    if (err)
        return CHECK_ERROR(Runtime::setError("process: kill failed with error: " + exlib::string(uv_strerror(err))));

    return 0;
}

result_t process_base::kill(int32_t pid, exlib::string signal)
{
    int32_t signo = sig_name_to_number(signal);
    if (signo < 0)
        return CHECK_ERROR(Runtime::setError("process: Unknown signal: " + signal));

    return kill(pid, signo);
}

}
