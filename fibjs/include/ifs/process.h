/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _process_base_H_
#define _process_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "ifs/EventEmitter.h"

namespace fibjs {

class EventEmitter_base;
class File_base;
class SubProcess_base;

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
    static result_t get_stdin(obj_ptr<File_base>& retVal);
    static result_t get_stdout(obj_ptr<File_base>& retVal);
    static result_t get_stderr(obj_ptr<File_base>& retVal);
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
    static result_t memoryUsage(v8::Local<v8::Object>& retVal);
    static result_t nextTick(v8::Local<v8::Function> func, OptArgs args);
    static result_t binding(exlib::string name, v8::Local<v8::Value>& retVal);
    static result_t open(exlib::string command, v8::Local<v8::Array> args, v8::Local<v8::Object> opts, obj_ptr<SubProcess_base>& retVal);
    static result_t open(exlib::string command, v8::Local<v8::Object> opts, obj_ptr<SubProcess_base>& retVal);
    static result_t start(exlib::string command, v8::Local<v8::Array> args, v8::Local<v8::Object> opts, obj_ptr<SubProcess_base>& retVal);
    static result_t start(exlib::string command, v8::Local<v8::Object> opts, obj_ptr<SubProcess_base>& retVal);
    static result_t run(exlib::string command, v8::Local<v8::Array> args, v8::Local<v8::Object> opts, int32_t& retVal, AsyncEvent* ac);
    static result_t run(exlib::string command, v8::Local<v8::Object> opts, int32_t& retVal, AsyncEvent* ac);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        Isolate* isolate = Isolate::current();

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
    static void s_static_memoryUsage(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_nextTick(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_binding(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_open(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_start(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_run(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
    ASYNC_STATICVALUE4(process_base, run, exlib::string, v8::Local<v8::Array>, v8::Local<v8::Object>, int32_t);
    ASYNC_STATICVALUE3(process_base, run, exlib::string, v8::Local<v8::Object>, int32_t);
};
}

#include "ifs/File.h"
#include "ifs/SubProcess.h"

namespace fibjs {
inline ClassInfo& process_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "umask", s_static_umask, true },
        { "hrtime", s_static_hrtime, true },
        { "exit", s_static_exit, true },
        { "cwd", s_static_cwd, true },
        { "chdir", s_static_chdir, true },
        { "uptime", s_static_uptime, true },
        { "memoryUsage", s_static_memoryUsage, true },
        { "nextTick", s_static_nextTick, true },
        { "binding", s_static_binding, true },
        { "open", s_static_open, true },
        { "start", s_static_start, true },
        { "run", s_static_run, true },
        { "runSync", s_static_run, true }
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
        { "exitCode", s_static_get_exitCode, s_static_set_exitCode, true }
    };

    static ClassData s_cd = {
        "process", true, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &EventEmitter_base::class_info()
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void process_base::s_static_get_argv(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Array> vr;

    METHOD_NAME("process.argv");
    PROPERTY_ENTER();

    hr = get_argv(vr);

    METHOD_RETURN();
}

inline void process_base::s_static_get_execArgv(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Array> vr;

    METHOD_NAME("process.execArgv");
    PROPERTY_ENTER();

    hr = get_execArgv(vr);

    METHOD_RETURN();
}

inline void process_base::s_static_get_version(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_NAME("process.version");
    PROPERTY_ENTER();

    hr = get_version(vr);

    METHOD_RETURN();
}

inline void process_base::s_static_get_versions(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Object> vr;

    METHOD_NAME("process.versions");
    PROPERTY_ENTER();

    hr = get_versions(vr);

    METHOD_RETURN();
}

inline void process_base::s_static_get_execPath(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_NAME("process.execPath");
    PROPERTY_ENTER();

    hr = get_execPath(vr);

    METHOD_RETURN();
}

inline void process_base::s_static_get_env(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Object> vr;

    METHOD_NAME("process.env");
    PROPERTY_ENTER();

    hr = get_env(vr);

    METHOD_RETURN();
}

inline void process_base::s_static_get_arch(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_NAME("process.arch");
    PROPERTY_ENTER();

    hr = get_arch(vr);

    METHOD_RETURN();
}

inline void process_base::s_static_get_platform(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_NAME("process.platform");
    PROPERTY_ENTER();

    hr = get_platform(vr);

    METHOD_RETURN();
}

inline void process_base::s_static_get_pid(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_NAME("process.pid");
    PROPERTY_ENTER();

    hr = get_pid(vr);

    METHOD_RETURN();
}

inline void process_base::s_static_get_ppid(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_NAME("process.ppid");
    PROPERTY_ENTER();

    hr = get_ppid(vr);

    METHOD_RETURN();
}

inline void process_base::s_static_get_stdin(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    obj_ptr<File_base> vr;

    METHOD_NAME("process.stdin");
    PROPERTY_ENTER();

    hr = get_stdin(vr);

    METHOD_RETURN();
}

inline void process_base::s_static_get_stdout(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    obj_ptr<File_base> vr;

    METHOD_NAME("process.stdout");
    PROPERTY_ENTER();

    hr = get_stdout(vr);

    METHOD_RETURN();
}

inline void process_base::s_static_get_stderr(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    obj_ptr<File_base> vr;

    METHOD_NAME("process.stderr");
    PROPERTY_ENTER();

    hr = get_stderr(vr);

    METHOD_RETURN();
}

inline void process_base::s_static_get_exitCode(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_NAME("process.exitCode");
    PROPERTY_ENTER();

    hr = get_exitCode(vr);

    METHOD_RETURN();
}

inline void process_base::s_static_set_exitCode(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_NAME("process.exitCode");
    PROPERTY_ENTER();
    PROPERTY_VAL(int32_t);

    hr = set_exitCode(v0);

    PROPERTY_SET_LEAVE();
}

inline void process_base::s_static_umask(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_NAME("process.umask");
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

    METHOD_NAME("process.hrtime");
    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(v8::Local<v8::Array>, 0, v8::Array::New(isolate));

    hr = hrtime(v0, vr);

    METHOD_RETURN();
}

inline void process_base::s_static_exit(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("process.exit");
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

    METHOD_NAME("process.cwd");
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = cwd(vr);

    METHOD_RETURN();
}

inline void process_base::s_static_chdir(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("process.chdir");
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = chdir(v0);

    METHOD_VOID();
}

inline void process_base::s_static_uptime(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    double vr;

    METHOD_NAME("process.uptime");
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = uptime(vr);

    METHOD_RETURN();
}

inline void process_base::s_static_memoryUsage(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Object> vr;

    METHOD_NAME("process.memoryUsage");
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = memoryUsage(vr);

    METHOD_RETURN();
}

inline void process_base::s_static_nextTick(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("process.nextTick");
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

    METHOD_NAME("process.binding");
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = binding(v0, vr);

    METHOD_RETURN();
}

inline void process_base::s_static_open(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<SubProcess_base> vr;

    METHOD_NAME("process.open");
    METHOD_ENTER();

    METHOD_OVER(3, 2);

    ARG(exlib::string, 0);
    ARG(v8::Local<v8::Array>, 1);
    OPT_ARG(v8::Local<v8::Object>, 2, v8::Object::New(isolate));

    hr = open(v0, v1, v2, vr);

    METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate));

    hr = open(v0, v1, vr);

    METHOD_RETURN();
}

inline void process_base::s_static_start(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<SubProcess_base> vr;

    METHOD_NAME("process.start");
    METHOD_ENTER();

    METHOD_OVER(3, 2);

    ARG(exlib::string, 0);
    ARG(v8::Local<v8::Array>, 1);
    OPT_ARG(v8::Local<v8::Object>, 2, v8::Object::New(isolate));

    hr = start(v0, v1, v2, vr);

    METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate));

    hr = start(v0, v1, vr);

    METHOD_RETURN();
}

inline void process_base::s_static_run(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_NAME("process.run");
    METHOD_ENTER();

    ASYNC_METHOD_OVER(3, 2);

    ARG(exlib::string, 0);
    ARG(v8::Local<v8::Array>, 1);
    OPT_ARG(v8::Local<v8::Object>, 2, v8::Object::New(isolate));

    if (!cb.IsEmpty()) {
        acb_run(v0, v1, v2, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = ac_run(v0, v1, v2, vr);

    ASYNC_METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate));

    if (!cb.IsEmpty()) {
        acb_run(v0, v1, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = ac_run(v0, v1, vr);

    METHOD_RETURN();
}
}

#endif
