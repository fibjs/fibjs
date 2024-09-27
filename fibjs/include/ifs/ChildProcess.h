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

class ChildProcess_base : public EventEmitter_base {
    DECLARE_CLASS(ChildProcess_base);

public:
    // ChildProcess_base
    virtual result_t kill(int32_t signal) = 0;
    virtual result_t kill(exlib::string signal) = 0;
    virtual result_t join(int32_t& retVal, AsyncEvent* ac) = 0;
    virtual result_t get_connected(bool& retVal) = 0;
    virtual result_t disconnect() = 0;
    virtual result_t send(v8::Local<v8::Value> msg) = 0;
    virtual result_t usage(v8::Local<v8::Object>& retVal) = 0;
    virtual result_t get_pid(int32_t& retVal) = 0;
    virtual result_t get_exitCode(int32_t& retVal) = 0;
    virtual result_t get_stdin(obj_ptr<Stream_base>& retVal) = 0;
    virtual result_t get_stdout(obj_ptr<Stream_base>& retVal) = 0;
    virtual result_t get_stderr(obj_ptr<Stream_base>& retVal) = 0;
    virtual result_t get_onexit(v8::Local<v8::Function>& retVal) = 0;
    virtual result_t set_onexit(v8::Local<v8::Function> newVal) = 0;
    virtual result_t get_onmessage(v8::Local<v8::Function>& retVal) = 0;
    virtual result_t set_onmessage(v8::Local<v8::Function> newVal) = 0;

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_kill(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_join(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_connected(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_disconnect(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_send(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_usage(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_pid(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_exitCode(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_stdin(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_stdout(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_stderr(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_onexit(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_onexit(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_onmessage(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_onmessage(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
    ASYNC_MEMBERVALUE1(ChildProcess_base, join, int32_t);
};
}

#include "ifs/Stream.h"

namespace fibjs {
inline ClassInfo& ChildProcess_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "kill", s_kill, false, ClassData::ASYNC_SYNC },
        { "join", s_join, false, ClassData::ASYNC_ASYNC },
        { "disconnect", s_disconnect, false, ClassData::ASYNC_SYNC },
        { "send", s_send, false, ClassData::ASYNC_SYNC },
        { "usage", s_usage, false, ClassData::ASYNC_SYNC }
    };

    static ClassData::ClassProperty s_property[] = {
        { "connected", s_get_connected, block_set, false },
        { "pid", s_get_pid, block_set, false },
        { "exitCode", s_get_exitCode, block_set, false },
        { "stdin", s_get_stdin, block_set, false },
        { "stdout", s_get_stdout, block_set, false },
        { "stderr", s_get_stderr, block_set, false },
        { "onexit", s_get_onexit, s_set_onexit, false },
        { "onmessage", s_get_onmessage, s_set_onmessage, false }
    };

    static ClassData s_cd = {
        "ChildProcess", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &EventEmitter_base::class_info(),
        true
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void ChildProcess_base::s_kill(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(ChildProcess_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(int32_t, 0);

    hr = pInst->kill(v0);

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = pInst->kill(v0);

    METHOD_VOID();
}

inline void ChildProcess_base::s_join(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    ASYNC_METHOD_INSTANCE(ChildProcess_base);
    ASYNC_METHOD_ENTER();

    METHOD_OVER(0, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_join(cb, args);
    else
        hr = pInst->ac_join(vr);

    METHOD_RETURN();
}

inline void ChildProcess_base::s_get_connected(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_INSTANCE(ChildProcess_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_connected(vr);

    METHOD_RETURN();
}

inline void ChildProcess_base::s_disconnect(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(ChildProcess_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->disconnect();

    METHOD_VOID();
}

inline void ChildProcess_base::s_send(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(ChildProcess_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = pInst->send(v0);

    METHOD_VOID();
}

inline void ChildProcess_base::s_usage(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Object> vr;

    METHOD_INSTANCE(ChildProcess_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->usage(vr);

    METHOD_RETURN();
}

inline void ChildProcess_base::s_get_pid(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(ChildProcess_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_pid(vr);

    METHOD_RETURN();
}

inline void ChildProcess_base::s_get_exitCode(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(ChildProcess_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_exitCode(vr);

    METHOD_RETURN();
}

inline void ChildProcess_base::s_get_stdin(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Stream_base> vr;

    METHOD_INSTANCE(ChildProcess_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_stdin(vr);

    METHOD_RETURN();
}

inline void ChildProcess_base::s_get_stdout(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Stream_base> vr;

    METHOD_INSTANCE(ChildProcess_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_stdout(vr);

    METHOD_RETURN();
}

inline void ChildProcess_base::s_get_stderr(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Stream_base> vr;

    METHOD_INSTANCE(ChildProcess_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_stderr(vr);

    METHOD_RETURN();
}

inline void ChildProcess_base::s_get_onexit(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_INSTANCE(ChildProcess_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_onexit(vr);

    METHOD_RETURN();
}

inline void ChildProcess_base::s_set_onexit(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(ChildProcess_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Function>, 0);

    hr = pInst->set_onexit(v0);

    METHOD_VOID();
}

inline void ChildProcess_base::s_get_onmessage(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_INSTANCE(ChildProcess_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_onmessage(vr);

    METHOD_RETURN();
}

inline void ChildProcess_base::s_set_onmessage(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(ChildProcess_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Function>, 0);

    hr = pInst->set_onmessage(v0);

    METHOD_VOID();
}
}
