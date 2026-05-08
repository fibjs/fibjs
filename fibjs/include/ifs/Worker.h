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

class Worker_base : public EventEmitter_base {
    DECLARE_CLASS(Worker_base);
    EVENT_SUPPORT();

public:
    // Worker_base
    static result_t _new(exlib::string path, v8::Local<v8::Object> opts, obj_ptr<Worker_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    virtual result_t get_threadId(int32_t& retVal) = 0;
    virtual result_t postMessage(v8::Local<v8::Value> data) = 0;
    virtual result_t terminate() = 0;
    virtual result_t ref() = 0;
    virtual result_t unref() = 0;

public:
    static void __new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static result_t load(v8::Local<v8::Value> v, obj_ptr<Worker_base>& retVal);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_threadId(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_postMessage(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_terminate(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_ref(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_unref(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_ononline(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_ononline(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_onmessage(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_onmessage(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_onerror(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_onerror(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_onexit(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_onexit(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

namespace fibjs {
inline ClassInfo& Worker_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "postMessage", s_postMessage, false, ClassData::ASYNC_SYNC },
        { "terminate", s_terminate, false, ClassData::ASYNC_SYNC },
        { "ref", s_ref, false, ClassData::ASYNC_SYNC },
        { "unref", s_unref, false, ClassData::ASYNC_SYNC }
    };

    static ClassData::ClassProperty s_property[] = {
        { "threadId", s_get_threadId, block_set, false },
        { "ononline", s_get_ononline, s_set_ononline, false },
        { "onmessage", s_get_onmessage, s_set_onmessage, false },
        { "onerror", s_get_onerror, s_set_onerror, false },
        { "onexit", s_get_onexit, s_set_onexit, false }
    };

    static ClassData s_cd = {
        "Worker", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &EventEmitter_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void Worker_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    CONSTRUCT_INIT();
    __new(args);
}

inline void Worker_base::__new(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Worker_base> vr;

    CONSTRUCT_ENTER();

    METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate->m_isolate));

    hr = _new(v0, v1, vr, args.This());

    CONSTRUCT_RETURN();
}

inline result_t Worker_base::load(v8::Local<v8::Value> v, obj_ptr<Worker_base>& retVal)
{
    obj_ptr<Worker_base> vr;

    LOAD_ENTER();

    METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate->m_isolate));

    hr = _new(v0, v1, vr, args.This());

    LOAD_RETURN();
}

inline void Worker_base::s_get_threadId(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(Worker_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_threadId(vr);

    METHOD_RETURN();
}

inline void Worker_base::s_postMessage(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(Worker_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = pInst->postMessage(v0);

    METHOD_VOID();
}

inline void Worker_base::s_terminate(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(Worker_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->terminate();

    METHOD_VOID();
}

inline void Worker_base::s_ref(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(Worker_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->ref();

    METHOD_VOID();
}

inline void Worker_base::s_unref(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(Worker_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->unref();

    METHOD_VOID();
}

inline void Worker_base::s_get_ononline(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_INSTANCE(Worker_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->getListener("online", vr);

    METHOD_RETURN();
}

inline void Worker_base::s_set_ononline(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(Worker_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Function>, 0);

    hr = pInst->setListener("online", v0);

    METHOD_VOID();
}

inline void Worker_base::s_get_onmessage(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_INSTANCE(Worker_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->getListener("message", vr);

    METHOD_RETURN();
}

inline void Worker_base::s_set_onmessage(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(Worker_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Function>, 0);

    hr = pInst->setListener("message", v0);

    METHOD_VOID();
}

inline void Worker_base::s_get_onerror(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_INSTANCE(Worker_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->getListener("error", vr);

    METHOD_RETURN();
}

inline void Worker_base::s_set_onerror(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(Worker_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Function>, 0);

    hr = pInst->setListener("error", v0);

    METHOD_VOID();
}

inline void Worker_base::s_get_onexit(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_INSTANCE(Worker_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->getListener("exit", vr);

    METHOD_RETURN();
}

inline void Worker_base::s_set_onexit(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(Worker_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Function>, 0);

    hr = pInst->setListener("exit", v0);

    METHOD_VOID();
}
}
