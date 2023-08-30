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

public:
    // process_base
    static result_t get_argv(v8::Local<v8::Array>& retVal);
    static result_t get_execArgv(v8::Local<v8::Array>& retVal);
    static result_t get_version(exlib::string& retVal);
    static result_t get_versions(v8::Local<v8::Object>& retVal);
    static result_t get_execPath(exlib::string& retVal);
    static result_t get_env(v8::Local<v8::Object>& retVal);
    static result_t get_arch(exlib::string& retVal);
    static result_t get_platform(exlib::string& retVal);
    static result_t get_pid(int32_t& retVal);
    static result_t get_ppid(int32_t& retVal);
    static result_t get_stdin(obj_ptr<Stream_base>& retVal);
    static result_t get_stdout(obj_ptr<Stream_base>& retVal);
    static result_t get_stderr(obj_ptr<Stream_base>& retVal);
    static result_t get_exitCode(int32_t& retVal);
    static result_t set_exitCode(int32_t newVal);
    static result_t umask(int32_t mask, int32_t& retVal);
    static result_t umask(exlib::string mask, int32_t& retVal);
    static result_t umask(int32_t& retVal);
    static result_t hrtime(v8::Local<v8::Array> diff, v8::Local<v8::Array>& retVal);
    static result_t exit();
    static result_t exit(int32_t code);
    static result_t cwd(exlib::string& retVal);
    static result_t chdir(exlib::string directory);
    static result_t uptime(double& retVal);
    static result_t cpuUsage(v8::Local<v8::Object> previousValue, v8::Local<v8::Object>& retVal);
    static result_t memoryUsage(v8::Local<v8::Object>& retVal);
    static result_t nextTick(v8::Local<v8::Function> func, OptArgs args);
    static result_t binding(exlib::string name, v8::Local<v8::Value>& retVal);
    static result_t getgid(int32_t& retVal);
    static result_t getuid(int32_t& retVal);
    static result_t setgid(int32_t id);
    static result_t setuid(int32_t id);
    static result_t emitWarning(v8::Local<v8::Value> warning, v8::Local<v8::Object> options);
    static result_t emitWarning(v8::Local<v8::Value> warning, exlib::string type, exlib::string code);
    static result_t get_connected(bool& retVal);
    static result_t disconnect();
    static result_t send(v8::Local<v8::Value> msg);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_static_get_argv(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_static_get_execArgv(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_static_get_version(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_static_get_versions(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_static_get_execPath(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_static_get_env(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_static_get_arch(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_static_get_platform(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_static_get_pid(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_static_get_ppid(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_static_get_stdin(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_static_get_stdout(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_static_get_stderr(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_static_get_exitCode(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_static_set_exitCode(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_static_umask(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_hrtime(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_exit(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_cwd(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_chdir(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_uptime(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_cpuUsage(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_memoryUsage(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_nextTick(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_binding(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_getgid(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_getuid(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_setgid(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_setuid(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_emitWarning(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_get_connected(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_static_disconnect(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_send(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

#include "ifs/Stream.h"

namespace fibjs {
inline ClassInfo& process_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "umask", s_static_umask, true, false },
        { "hrtime", s_static_hrtime, true, false },
        { "exit", s_static_exit, true, false },
        { "cwd", s_static_cwd, true, false },
        { "chdir", s_static_chdir, true, false },
        { "uptime", s_static_uptime, true, false },
        { "cpuUsage", s_static_cpuUsage, true, false },
        { "memoryUsage", s_static_memoryUsage, true, false },
        { "nextTick", s_static_nextTick, true, false },
        { "binding", s_static_binding, true, false },
        { "getgid", s_static_getgid, true, false },
        { "getuid", s_static_getuid, true, false },
        { "setgid", s_static_setgid, true, false },
        { "setuid", s_static_setuid, true, false },
        { "emitWarning", s_static_emitWarning, true, false },
        { "disconnect", s_static_disconnect, true, false },
        { "send", s_static_send, true, false }
    };

    static ClassData::ClassProperty s_property[] = {
        { "argv", s_static_get_argv, block_set, true },
        { "execArgv", s_static_get_execArgv, block_set, true },
        { "version", s_static_get_version, block_set, true },
        { "versions", s_static_get_versions, block_set, true },
        { "execPath", s_static_get_execPath, block_set, true },
        { "env", s_static_get_env, block_set, true },
        { "arch", s_static_get_arch, block_set, true },
        { "platform", s_static_get_platform, block_set, true },
        { "pid", s_static_get_pid, block_set, true },
        { "ppid", s_static_get_ppid, block_set, true },
        { "stdin", s_static_get_stdin, block_set, true },
        { "stdout", s_static_get_stdout, block_set, true },
        { "stderr", s_static_get_stderr, block_set, true },
        { "exitCode", s_static_get_exitCode, s_static_set_exitCode, true },
        { "connected", s_static_get_connected, block_set, true }
    };

    static ClassData s_cd = {
        "process", true, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &EventEmitter_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void process_base::s_static_get_argv(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Array> vr;

    PROPERTY_ENTER();

    hr = get_argv(vr);

    METHOD_RETURN();
}

inline void process_base::s_static_get_execArgv(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Array> vr;

    PROPERTY_ENTER();

    hr = get_execArgv(vr);

    METHOD_RETURN();
}

inline void process_base::s_static_get_version(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    PROPERTY_ENTER();

    hr = get_version(vr);

    METHOD_RETURN();
}

inline void process_base::s_static_get_versions(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Object> vr;

    PROPERTY_ENTER();

    hr = get_versions(vr);

    METHOD_RETURN();
}

inline void process_base::s_static_get_execPath(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    PROPERTY_ENTER();

    hr = get_execPath(vr);

    METHOD_RETURN();
}

inline void process_base::s_static_get_env(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Object> vr;

    PROPERTY_ENTER();

    hr = get_env(vr);

    METHOD_RETURN();
}

inline void process_base::s_static_get_arch(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    PROPERTY_ENTER();

    hr = get_arch(vr);

    METHOD_RETURN();
}

inline void process_base::s_static_get_platform(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    PROPERTY_ENTER();

    hr = get_platform(vr);

    METHOD_RETURN();
}

inline void process_base::s_static_get_pid(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    PROPERTY_ENTER();

    hr = get_pid(vr);

    METHOD_RETURN();
}

inline void process_base::s_static_get_ppid(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    PROPERTY_ENTER();

    hr = get_ppid(vr);

    METHOD_RETURN();
}

inline void process_base::s_static_get_stdin(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    obj_ptr<Stream_base> vr;

    PROPERTY_ENTER();

    hr = get_stdin(vr);

    METHOD_RETURN();
}

inline void process_base::s_static_get_stdout(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    obj_ptr<Stream_base> vr;

    PROPERTY_ENTER();

    hr = get_stdout(vr);

    METHOD_RETURN();
}

inline void process_base::s_static_get_stderr(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    obj_ptr<Stream_base> vr;

    PROPERTY_ENTER();

    hr = get_stderr(vr);

    METHOD_RETURN();
}

inline void process_base::s_static_get_exitCode(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    PROPERTY_ENTER();

    hr = get_exitCode(vr);

    METHOD_RETURN();
}

inline void process_base::s_static_set_exitCode(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    PROPERTY_ENTER();
    PROPERTY_VAL(int32_t);

    hr = set_exitCode(v0);

    PROPERTY_SET_LEAVE();
}

inline void process_base::s_static_umask(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(int32_t, 0);

    hr = umask(v0, vr);

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

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

inline void process_base::s_static_chdir(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = chdir(v0);

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
    v8::Local<v8::Object> vr;

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

inline void process_base::s_static_get_connected(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    bool vr;

    PROPERTY_ENTER();

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
