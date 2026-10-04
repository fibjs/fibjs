/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#pragma once

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "ifs/EventEmitter.h"

namespace fibjs {

class EventEmitter_base;
class Stream_base;

class process_base : public EventEmitter_base {
    DECLARE_CLASS(process_base);
    EVENT_SUPPORT();

public:
    using Union_umask_mask = std::variant<exlib::string, int32_t>;
    using Union_kill_signal = std::variant<exlib::string, int32_t>;

public:
    class ReleaseType : public NType {
    public:
        virtual void to_value(Isolate* isolate, v8::Local<v8::Object>& retVal)
        {
            v8::Local<v8::Context> context = isolate->context();
            retVal->Set(context, isolate->NewString("name"), GetReturnValue(isolate, name)).Check();
            retVal->Set(context, isolate->NewString("sourceUrl"), GetReturnValue(isolate, sourceUrl)).Check();
            retVal->Set(context, isolate->NewString("venderUrl"), GetReturnValue(isolate, venderUrl)).Check();
        }

        virtual void to_args(Isolate* isolate, std::vector<v8::Local<v8::Value>>& args)
        {
            args.push_back(GetReturnValue(isolate, name));
            args.push_back(GetReturnValue(isolate, sourceUrl));
            args.push_back(GetReturnValue(isolate, venderUrl));
        }

    public:
        exlib::string name;
        exlib::string sourceUrl;
        exlib::string venderUrl;
    };
    class CpuUsageType : public NType {
    public:
        virtual void to_value(Isolate* isolate, v8::Local<v8::Object>& retVal)
        {
            v8::Local<v8::Context> context = isolate->context();
            retVal->Set(context, isolate->NewString("user"), GetReturnValue(isolate, user)).Check();
            retVal->Set(context, isolate->NewString("system"), GetReturnValue(isolate, system)).Check();
        }

        virtual void to_args(Isolate* isolate, std::vector<v8::Local<v8::Value>>& args)
        {
            args.push_back(GetReturnValue(isolate, user));
            args.push_back(GetReturnValue(isolate, system));
        }

    public:
        double user;
        double system;
    };
    class ResourceUsageType : public NType {
    public:
        virtual void to_value(Isolate* isolate, v8::Local<v8::Object>& retVal)
        {
            v8::Local<v8::Context> context = isolate->context();
            retVal->Set(context, isolate->NewString("userCPUTime"), GetReturnValue(isolate, userCPUTime)).Check();
            retVal->Set(context, isolate->NewString("systemCPUTime"), GetReturnValue(isolate, systemCPUTime)).Check();
            retVal->Set(context, isolate->NewString("maxRSS"), GetReturnValue(isolate, maxRSS)).Check();
            retVal->Set(context, isolate->NewString("sharedMemorySize"), GetReturnValue(isolate, sharedMemorySize)).Check();
            retVal->Set(context, isolate->NewString("unsharedDataSize"), GetReturnValue(isolate, unsharedDataSize)).Check();
            retVal->Set(context, isolate->NewString("unsharedStackSize"), GetReturnValue(isolate, unsharedStackSize)).Check();
            retVal->Set(context, isolate->NewString("minorPageFault"), GetReturnValue(isolate, minorPageFault)).Check();
            retVal->Set(context, isolate->NewString("majorPageFault"), GetReturnValue(isolate, majorPageFault)).Check();
            retVal->Set(context, isolate->NewString("swappedOut"), GetReturnValue(isolate, swappedOut)).Check();
            retVal->Set(context, isolate->NewString("fsRead"), GetReturnValue(isolate, fsRead)).Check();
            retVal->Set(context, isolate->NewString("fsWrite"), GetReturnValue(isolate, fsWrite)).Check();
            retVal->Set(context, isolate->NewString("ipcSent"), GetReturnValue(isolate, ipcSent)).Check();
            retVal->Set(context, isolate->NewString("ipcReceived"), GetReturnValue(isolate, ipcReceived)).Check();
            retVal->Set(context, isolate->NewString("signalsCount"), GetReturnValue(isolate, signalsCount)).Check();
            retVal->Set(context, isolate->NewString("voluntaryContextSwitches"), GetReturnValue(isolate, voluntaryContextSwitches)).Check();
            retVal->Set(context, isolate->NewString("involuntaryContextSwitches"), GetReturnValue(isolate, involuntaryContextSwitches)).Check();
        }

        virtual void to_args(Isolate* isolate, std::vector<v8::Local<v8::Value>>& args)
        {
            args.push_back(GetReturnValue(isolate, userCPUTime));
            args.push_back(GetReturnValue(isolate, systemCPUTime));
            args.push_back(GetReturnValue(isolate, maxRSS));
            args.push_back(GetReturnValue(isolate, sharedMemorySize));
            args.push_back(GetReturnValue(isolate, unsharedDataSize));
            args.push_back(GetReturnValue(isolate, unsharedStackSize));
            args.push_back(GetReturnValue(isolate, minorPageFault));
            args.push_back(GetReturnValue(isolate, majorPageFault));
            args.push_back(GetReturnValue(isolate, swappedOut));
            args.push_back(GetReturnValue(isolate, fsRead));
            args.push_back(GetReturnValue(isolate, fsWrite));
            args.push_back(GetReturnValue(isolate, ipcSent));
            args.push_back(GetReturnValue(isolate, ipcReceived));
            args.push_back(GetReturnValue(isolate, signalsCount));
            args.push_back(GetReturnValue(isolate, voluntaryContextSwitches));
            args.push_back(GetReturnValue(isolate, involuntaryContextSwitches));
        }

    public:
        double userCPUTime;
        double systemCPUTime;
        double maxRSS;
        double sharedMemorySize;
        double unsharedDataSize;
        double unsharedStackSize;
        double minorPageFault;
        double majorPageFault;
        double swappedOut;
        double fsRead;
        double fsWrite;
        double ipcSent;
        double ipcReceived;
        double signalsCount;
        double voluntaryContextSwitches;
        double involuntaryContextSwitches;
    };

public:
    // process_base
    static result_t get_argv(v8::Local<v8::Array>& retVal);
    static result_t set_argv(v8::Local<v8::Array> newVal);
    static result_t get_execArgv(std::vector<exlib::string>& retVal);
    static result_t get_version(exlib::string& retVal);
    static result_t get_versions(v8::Local<v8::Object>& retVal);
    static result_t get_execPath(exlib::string& retVal);
    static result_t get_env(v8::Local<v8::Object>& retVal);
    static result_t get_arch(exlib::string& retVal);
    static result_t get_platform(exlib::string& retVal);
    static result_t get_release(obj_ptr<ReleaseType>& retVal);
    static result_t get_pid(int32_t& retVal);
    static result_t get_ppid(int32_t& retVal);
    static result_t get_stdin(obj_ptr<Stream_base>& retVal);
    static result_t get_stdout(obj_ptr<Stream_base>& retVal);
    static result_t get_stderr(obj_ptr<Stream_base>& retVal);
    static result_t get_exitCode(int32_t& retVal);
    static result_t set_exitCode(int32_t newVal);
    static result_t umask(Union_umask_mask mask, int32_t& retVal);
    static result_t umask(int32_t& retVal);
    static result_t hrtime(v8::Local<v8::Array> diff, v8::Local<v8::Array>& retVal);
    static result_t exit();
    static result_t exit(int32_t code);
    static result_t cwd(exlib::string& retVal);
    static result_t dlopen(v8::Local<v8::Object> module, exlib::string filename, int32_t flags);
    static result_t chdir(exlib::string directory);
    static result_t loadEnvFile(exlib::string path);
    static result_t uptime(double& retVal);
    static result_t cpuUsage(v8::Local<v8::Object> previousValue, obj_ptr<CpuUsageType>& retVal);
    static result_t memoryUsage(v8::Local<v8::Object>& retVal);
    static result_t resourceUsage(obj_ptr<ResourceUsageType>& retVal);
    static result_t nextTick(v8::Local<v8::Function> func, OptArgs args);
    static result_t binding(exlib::string name, v8::Local<v8::Value>& retVal);
    static result_t getBuiltinModule(exlib::string id, v8::Local<v8::Value>& retVal);
    static result_t getgid(int32_t& retVal);
    static result_t getuid(int32_t& retVal);
    static result_t setgid(int32_t id);
    static result_t setuid(int32_t id);
    static result_t emitWarning(v8::Local<v8::Value> warning, v8::Local<v8::Object> options);
    static result_t emitWarning(v8::Local<v8::Value> warning, exlib::string type, exlib::string code);
    static result_t kill(int32_t pid, Union_kill_signal signal);
    static result_t get_connected(bool& retVal);
    static result_t disconnect();
    static result_t send(v8::Local<v8::Value> msg);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        ThrowTypeError("not a constructor");
    }

    static result_t load(v8::Local<v8::Value> v, obj_ptr<process_base>& retVal)
    { return CALL_E_TYPEMISMATCH; }

public:
    static void s_static_get_argv(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_set_argv(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_get_execArgv(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_get_version(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_get_versions(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_get_execPath(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_get_env(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_get_arch(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_get_platform(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_get_release(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_get_pid(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_get_ppid(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_get_stdin(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_get_stdout(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_get_stderr(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_get_exitCode(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_set_exitCode(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_umask(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_hrtime(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_exit(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_cwd(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_dlopen(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_chdir(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_loadEnvFile(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_uptime(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_cpuUsage(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_memoryUsage(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_resourceUsage(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_nextTick(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_binding(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_getBuiltinModule(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_getgid(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_getuid(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_setgid(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_setuid(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_emitWarning(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_kill(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_get_connected(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_disconnect(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_send(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

#include "ifs/Stream.h"

namespace fibjs {
inline ClassInfo& process_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "umask", s_static_umask, true, ClassData::ASYNC_SYNC },
        { "hrtime", s_static_hrtime, true, ClassData::ASYNC_SYNC },
        { "exit", s_static_exit, true, ClassData::ASYNC_SYNC },
        { "cwd", s_static_cwd, true, ClassData::ASYNC_SYNC },
        { "dlopen", s_static_dlopen, true, ClassData::ASYNC_SYNC },
        { "chdir", s_static_chdir, true, ClassData::ASYNC_SYNC },
        { "loadEnvFile", s_static_loadEnvFile, true, ClassData::ASYNC_SYNC },
        { "uptime", s_static_uptime, true, ClassData::ASYNC_SYNC },
        { "cpuUsage", s_static_cpuUsage, true, ClassData::ASYNC_SYNC },
        { "memoryUsage", s_static_memoryUsage, true, ClassData::ASYNC_SYNC },
        { "resourceUsage", s_static_resourceUsage, true, ClassData::ASYNC_SYNC },
        { "nextTick", s_static_nextTick, true, ClassData::ASYNC_SYNC },
        { "binding", s_static_binding, true, ClassData::ASYNC_SYNC },
        { "getBuiltinModule", s_static_getBuiltinModule, true, ClassData::ASYNC_SYNC },
        { "getgid", s_static_getgid, true, ClassData::ASYNC_SYNC },
        { "getuid", s_static_getuid, true, ClassData::ASYNC_SYNC },
        { "setgid", s_static_setgid, true, ClassData::ASYNC_SYNC },
        { "setuid", s_static_setuid, true, ClassData::ASYNC_SYNC },
        { "emitWarning", s_static_emitWarning, true, ClassData::ASYNC_SYNC },
        { "kill", s_static_kill, true, ClassData::ASYNC_SYNC },
        { "disconnect", s_static_disconnect, true, ClassData::ASYNC_SYNC },
        { "send", s_static_send, true, ClassData::ASYNC_SYNC }
    };

    static ClassData::ClassProperty s_property[] = {
        { "argv", s_static_get_argv, s_static_set_argv, true },
        { "execArgv", s_static_get_execArgv, block_set, true },
        { "version", s_static_get_version, block_set, true },
        { "versions", s_static_get_versions, block_set, true },
        { "execPath", s_static_get_execPath, block_set, true },
        { "env", s_static_get_env, block_set, true },
        { "arch", s_static_get_arch, block_set, true },
        { "platform", s_static_get_platform, block_set, true },
        { "release", s_static_get_release, block_set, true },
        { "pid", s_static_get_pid, block_set, true },
        { "ppid", s_static_get_ppid, block_set, true },
        { "stdin", s_static_get_stdin, block_set, true },
        { "stdout", s_static_get_stdout, block_set, true },
        { "stderr", s_static_get_stderr, block_set, true },
        { "exitCode", s_static_get_exitCode, s_static_set_exitCode, true },
        { "connected", s_static_get_connected, block_set, true }
    };

    static ClassData::ClassConst s_const[] = {
        { "title", ClassData::CONST_String, { .stringValue = "fibjs" } }
    };

    static ClassData s_cd = {
        "process", true, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, ARRAYSIZE(s_const), s_const, NULL, NULL,
        &EventEmitter_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void process_base::s_static_get_argv(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Array> vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = get_argv(vr);

    METHOD_RETURN();
}

inline void process_base::s_static_set_argv(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Array>, 0);

    hr = set_argv(v0);

    METHOD_VOID();
}

inline void process_base::s_static_get_execArgv(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    std::vector<exlib::string> vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = get_execArgv(vr);

    METHOD_RETURN();
}

inline void process_base::s_static_get_version(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = get_version(vr);

    METHOD_RETURN();
}

inline void process_base::s_static_get_versions(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Object> vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = get_versions(vr);

    METHOD_RETURN();
}

inline void process_base::s_static_get_execPath(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = get_execPath(vr);

    METHOD_RETURN();
}

inline void process_base::s_static_get_env(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Object> vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = get_env(vr);

    METHOD_RETURN();
}

inline void process_base::s_static_get_arch(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = get_arch(vr);

    METHOD_RETURN();
}

inline void process_base::s_static_get_platform(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = get_platform(vr);

    METHOD_RETURN();
}

inline void process_base::s_static_get_release(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<ReleaseType> vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = get_release(vr);

    METHOD_RETURN();
}

inline void process_base::s_static_get_pid(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = get_pid(vr);

    METHOD_RETURN();
}

inline void process_base::s_static_get_ppid(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = get_ppid(vr);

    METHOD_RETURN();
}

inline void process_base::s_static_get_stdin(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Stream_base> vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = get_stdin(vr);

    METHOD_RETURN();
}

inline void process_base::s_static_get_stdout(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Stream_base> vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = get_stdout(vr);

    METHOD_RETURN();
}

inline void process_base::s_static_get_stderr(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Stream_base> vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = get_stderr(vr);

    METHOD_RETURN();
}

inline void process_base::s_static_get_exitCode(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = get_exitCode(vr);

    METHOD_RETURN();
}

inline void process_base::s_static_set_exitCode(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(int32_t, 0);

    hr = set_exitCode(v0);

    METHOD_VOID();
}

inline void process_base::s_static_umask(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(Union_umask_mask, 0);

    hr = umask(v0, vr);

    METHOD_OVER(0, 0);

    hr = umask(vr);

    METHOD_RETURN();
}

inline void process_base::s_static_hrtime(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Array> vr;

    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(v8::Local<v8::Array>, 0, v8::Array::New(isolate->m_isolate));

    hr = hrtime(v0, vr);

    METHOD_RETURN();
}

inline void process_base::s_static_exit(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = exit();

    METHOD_OVER(1, 1);

    ARG(int32_t, 0);

    hr = exit(v0);

    METHOD_VOID();
}

inline void process_base::s_static_cwd(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = cwd(vr);

    METHOD_RETURN();
}

inline void process_base::s_static_dlopen(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(3, 2);

    ARG(v8::Local<v8::Object>, 0);
    ARG(exlib::string, 1);
    OPT_ARG(int32_t, 2, 1);

    hr = dlopen(v0, v1, v2);

    METHOD_VOID();
}

inline void process_base::s_static_chdir(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = chdir(v0);

    METHOD_VOID();
}

inline void process_base::s_static_loadEnvFile(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(exlib::string, 0, "");

    hr = loadEnvFile(v0);

    METHOD_VOID();
}

inline void process_base::s_static_uptime(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    double vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = uptime(vr);

    METHOD_RETURN();
}

inline void process_base::s_static_cpuUsage(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<CpuUsageType> vr;

    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(v8::Local<v8::Object>, 0, v8::Object::New(isolate->m_isolate));

    hr = cpuUsage(v0, vr);

    METHOD_RETURN();
}

inline void process_base::s_static_memoryUsage(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Object> vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = memoryUsage(vr);

    METHOD_RETURN();
}

inline void process_base::s_static_resourceUsage(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<ResourceUsageType> vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = resourceUsage(vr);

    METHOD_RETURN();
}

inline void process_base::s_static_nextTick(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(-1, 1);

    ARG(v8::Local<v8::Function>, 0);
    ARG_LIST(1);

    hr = nextTick(v0, v1);

    METHOD_VOID();
}

inline void process_base::s_static_binding(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Value> vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = binding(v0, vr);

    METHOD_RETURN();
}

inline void process_base::s_static_getBuiltinModule(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Value> vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = getBuiltinModule(v0, vr);

    METHOD_RETURN();
}

inline void process_base::s_static_getgid(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = getgid(vr);

    METHOD_RETURN();
}

inline void process_base::s_static_getuid(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = getuid(vr);

    METHOD_RETURN();
}

inline void process_base::s_static_setgid(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(int32_t, 0);

    hr = setgid(v0);

    METHOD_VOID();
}

inline void process_base::s_static_setuid(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(int32_t, 0);

    hr = setuid(v0);

    METHOD_VOID();
}

inline void process_base::s_static_emitWarning(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(v8::Local<v8::Value>, 0);
    ARG(v8::Local<v8::Object>, 1);

    hr = emitWarning(v0, v1);

    METHOD_OVER(3, 1);

    ARG(v8::Local<v8::Value>, 0);
    OPT_ARG(exlib::string, 1, "Warning");
    OPT_ARG(exlib::string, 2, "");

    hr = emitWarning(v0, v1, v2);

    METHOD_VOID();
}

inline void process_base::s_static_kill(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(int32_t, 0);
    OPT_ARG(Union_kill_signal, 1, exlib::string("SIGTERM"));

    hr = kill(v0, v1);

    METHOD_VOID();
}

inline void process_base::s_static_get_connected(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = get_connected(vr);

    METHOD_RETURN();
}

inline void process_base::s_static_disconnect(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = disconnect();

    METHOD_VOID();
}

inline void process_base::s_static_send(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = send(v0);

    METHOD_VOID();
}
}
