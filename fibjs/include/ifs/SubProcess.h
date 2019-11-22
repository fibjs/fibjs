/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _SubProcess_base_H_
#define _SubProcess_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs {

class BufferedStream_base;

class SubProcess_base : public object_base {
    DECLARE_CLASS(SubProcess_base);

public:
    // SubProcess_base
    virtual result_t kill(int32_t signal) = 0;
    virtual result_t wait(int32_t& retVal, AsyncEvent* ac) = 0;
    virtual result_t findWindow(exlib::string name, v8::Local<v8::Value>& retVal) = 0;
    virtual result_t get_pid(int32_t& retVal) = 0;
    virtual result_t get_ppid(int32_t& retVal) = 0;
    virtual result_t get_stdin(obj_ptr<BufferedStream_base>& retVal) = 0;
    virtual result_t get_stdout(obj_ptr<BufferedStream_base>& retVal) = 0;
    virtual result_t get_stderr(obj_ptr<BufferedStream_base>& retVal) = 0;

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        Isolate* isolate = Isolate::current();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_kill(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_wait(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_findWindow(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_pid(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_ppid(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_stdin(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_stdout(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_stderr(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);

public:
    ASYNC_MEMBERVALUE1(SubProcess_base, wait, int32_t);
};
}

#include "ifs/BufferedStream.h"

namespace fibjs {
inline ClassInfo& SubProcess_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "kill", s_kill, false },
        { "wait", s_wait, false },
        { "waitSync", s_wait, false },
        { "findWindow", s_findWindow, false }
    };

    static ClassData::ClassProperty s_property[] = {
        { "pid", s_get_pid, block_set, false },
        { "ppid", s_get_ppid, block_set, false },
        { "stdin", s_get_stdin, block_set, false },
        { "stdout", s_get_stdout, block_set, false },
        { "stderr", s_get_stderr, block_set, false }
    };

    static ClassData s_cd = {
        "SubProcess", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &object_base::class_info()
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void SubProcess_base::s_kill(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("SubProcess.kill");
    METHOD_INSTANCE(SubProcess_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(int32_t, 0);

    hr = pInst->kill(v0);

    METHOD_VOID();
}

inline void SubProcess_base::s_wait(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_NAME("SubProcess.wait");
    METHOD_INSTANCE(SubProcess_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(0, 0);

    if (!cb.IsEmpty()) {
        pInst->acb_wait(cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = pInst->ac_wait(vr);

    METHOD_RETURN();
}

inline void SubProcess_base::s_findWindow(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Value> vr;

    METHOD_NAME("SubProcess.findWindow");
    METHOD_INSTANCE(SubProcess_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = pInst->findWindow(v0, vr);

    METHOD_RETURN();
}

inline void SubProcess_base::s_get_pid(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_NAME("SubProcess.pid");
    METHOD_INSTANCE(SubProcess_base);
    PROPERTY_ENTER();

    hr = pInst->get_pid(vr);

    METHOD_RETURN();
}

inline void SubProcess_base::s_get_ppid(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_NAME("SubProcess.ppid");
    METHOD_INSTANCE(SubProcess_base);
    PROPERTY_ENTER();

    hr = pInst->get_ppid(vr);

    METHOD_RETURN();
}

inline void SubProcess_base::s_get_stdin(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    obj_ptr<BufferedStream_base> vr;

    METHOD_NAME("SubProcess.stdin");
    METHOD_INSTANCE(SubProcess_base);
    PROPERTY_ENTER();

    hr = pInst->get_stdin(vr);

    METHOD_RETURN();
}

inline void SubProcess_base::s_get_stdout(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    obj_ptr<BufferedStream_base> vr;

    METHOD_NAME("SubProcess.stdout");
    METHOD_INSTANCE(SubProcess_base);
    PROPERTY_ENTER();

    hr = pInst->get_stdout(vr);

    METHOD_RETURN();
}

inline void SubProcess_base::s_get_stderr(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    obj_ptr<BufferedStream_base> vr;

    METHOD_NAME("SubProcess.stderr");
    METHOD_INSTANCE(SubProcess_base);
    PROPERTY_ENTER();

    hr = pInst->get_stderr(vr);

    METHOD_RETURN();
}
}

#endif
