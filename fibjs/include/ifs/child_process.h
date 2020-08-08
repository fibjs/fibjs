/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _child_process_base_H_
#define _child_process_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs {

class ChildProcess_base;

class child_process_base : public object_base {
    DECLARE_CLASS(child_process_base);

public:
    class ExecType : public NType {
    public:
        virtual void fillMembers(Isolate* isolate, v8::Local<v8::Object>& retVal)
        {
            retVal->Set(isolate->NewString("stdout"), GetReturnValue(isolate->m_isolate, stdout));
            retVal->Set(isolate->NewString("stderr"), GetReturnValue(isolate->m_isolate, stderr));
        }

        virtual void fillArguments(Isolate* isolate, std::vector<v8::Local<v8::Value>>& args)
        {
            args.push_back(GetReturnValue(isolate->m_isolate, stdout));
            args.push_back(GetReturnValue(isolate->m_isolate, stderr));
        }

    public:
        Variant stdout;
        Variant stderr;
    };
    class ExecFileType : public NType {
    public:
        virtual void fillMembers(Isolate* isolate, v8::Local<v8::Object>& retVal)
        {
            retVal->Set(isolate->NewString("stdout"), GetReturnValue(isolate->m_isolate, stdout));
            retVal->Set(isolate->NewString("stderr"), GetReturnValue(isolate->m_isolate, stderr));
        }

        virtual void fillArguments(Isolate* isolate, std::vector<v8::Local<v8::Value>>& args)
        {
            args.push_back(GetReturnValue(isolate->m_isolate, stdout));
            args.push_back(GetReturnValue(isolate->m_isolate, stderr));
        }

    public:
        Variant stdout;
        Variant stderr;
    };

public:
    // child_process_base
    static result_t spawn(exlib::string command, v8::Local<v8::Array> args, v8::Local<v8::Object> options, obj_ptr<ChildProcess_base>& retVal);
    static result_t spawn(exlib::string command, v8::Local<v8::Object> options, obj_ptr<ChildProcess_base>& retVal);
    static result_t exec(exlib::string command, v8::Local<v8::Object> options, obj_ptr<ExecType>& retVal, AsyncEvent* ac);
    static result_t execFile(exlib::string command, v8::Local<v8::Array> args, v8::Local<v8::Object> options, obj_ptr<ExecFileType>& retVal, AsyncEvent* ac);
    static result_t execFile(exlib::string command, v8::Local<v8::Object> options, obj_ptr<ExecFileType>& retVal, AsyncEvent* ac);
    static result_t run(exlib::string command, v8::Local<v8::Array> args, v8::Local<v8::Object> options, int32_t& retVal, AsyncEvent* ac);
    static result_t run(exlib::string command, v8::Local<v8::Object> options, int32_t& retVal, AsyncEvent* ac);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        Isolate* isolate = Isolate::current();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_static_spawn(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_exec(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_execFile(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_run(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
    ASYNC_STATICVALUE3(child_process_base, exec, exlib::string, v8::Local<v8::Object>, obj_ptr<ExecType>);
    ASYNC_STATICVALUE4(child_process_base, execFile, exlib::string, v8::Local<v8::Array>, v8::Local<v8::Object>, obj_ptr<ExecFileType>);
    ASYNC_STATICVALUE3(child_process_base, execFile, exlib::string, v8::Local<v8::Object>, obj_ptr<ExecFileType>);
    ASYNC_STATICVALUE4(child_process_base, run, exlib::string, v8::Local<v8::Array>, v8::Local<v8::Object>, int32_t);
    ASYNC_STATICVALUE3(child_process_base, run, exlib::string, v8::Local<v8::Object>, int32_t);
};
}

#include "ifs/ChildProcess.h"

namespace fibjs {
inline ClassInfo& child_process_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "spawn", s_static_spawn, true },
        { "exec", s_static_exec, true },
        { "execSync", s_static_exec, true },
        { "execFile", s_static_execFile, true },
        { "execFileSync", s_static_execFile, true },
        { "run", s_static_run, true },
        { "runSync", s_static_run, true }
    };

    static ClassData s_cd = {
        "child_process", true, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, 0, NULL, 0, NULL, NULL, NULL,
        &object_base::class_info()
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void child_process_base::s_static_spawn(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<ChildProcess_base> vr;

    METHOD_NAME("child_process.spawn");
    METHOD_ENTER();

    METHOD_OVER(3, 2);

    ARG(exlib::string, 0);
    ARG(v8::Local<v8::Array>, 1);
    OPT_ARG(v8::Local<v8::Object>, 2, v8::Object::New(isolate));

    hr = spawn(v0, v1, v2, vr);

    METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate));

    hr = spawn(v0, v1, vr);

    METHOD_RETURN();
}

inline void child_process_base::s_static_exec(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<ExecType> vr;

    METHOD_NAME("child_process.exec");
    METHOD_ENTER();

    ASYNC_METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate));

    if (!cb.IsEmpty())
        hr = acb_exec(v0, v1, cb, args);
    else
        hr = ac_exec(v0, v1, vr);

    METHOD_RETURN();
}

inline void child_process_base::s_static_execFile(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<ExecFileType> vr;

    METHOD_NAME("child_process.execFile");
    METHOD_ENTER();

    ASYNC_METHOD_OVER(3, 2);

    ARG(exlib::string, 0);
    ARG(v8::Local<v8::Array>, 1);
    OPT_ARG(v8::Local<v8::Object>, 2, v8::Object::New(isolate));

    if (!cb.IsEmpty())
        hr = acb_execFile(v0, v1, v2, cb, args);
    else
        hr = ac_execFile(v0, v1, v2, vr);

    ASYNC_METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate));

    if (!cb.IsEmpty())
        hr = acb_execFile(v0, v1, cb, args);
    else
        hr = ac_execFile(v0, v1, vr);

    METHOD_RETURN();
}

inline void child_process_base::s_static_run(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_NAME("child_process.run");
    METHOD_ENTER();

    ASYNC_METHOD_OVER(3, 2);

    ARG(exlib::string, 0);
    ARG(v8::Local<v8::Array>, 1);
    OPT_ARG(v8::Local<v8::Object>, 2, v8::Object::New(isolate));

    if (!cb.IsEmpty())
        hr = acb_run(v0, v1, v2, cb, args);
    else
        hr = ac_run(v0, v1, v2, vr);

    ASYNC_METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate));

    if (!cb.IsEmpty())
        hr = acb_run(v0, v1, cb, args);
    else
        hr = ac_run(v0, v1, vr);

    METHOD_RETURN();
}
}

#endif
