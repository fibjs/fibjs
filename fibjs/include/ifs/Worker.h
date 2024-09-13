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

public:
    // Worker_base
    static result_t _new(exlib::string path, v8::Local<v8::Object> opts, obj_ptr<Worker_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    virtual result_t postMessage(v8::Local<v8::Value> data) = 0;
    virtual result_t get_onload(v8::Local<v8::Function>& retVal) = 0;
    virtual result_t set_onload(v8::Local<v8::Function> newVal) = 0;
    virtual result_t get_onmessage(v8::Local<v8::Function>& retVal) = 0;
    virtual result_t set_onmessage(v8::Local<v8::Function> newVal) = 0;
    virtual result_t get_onerror(v8::Local<v8::Function>& retVal) = 0;
    virtual result_t set_onerror(v8::Local<v8::Function> newVal) = 0;

public:
    template <typename T>
    static void __new(const T& args);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_postMessage(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_onload(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_onload(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_onmessage(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_onmessage(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_onerror(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_onerror(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

namespace fibjs {
inline ClassInfo& Worker_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "postMessage", s_postMessage, false, ClassData::ASYNC_SYNC }
    };

    static ClassData::ClassProperty s_property[] = {
        { "onload", s_get_onload, s_set_onload, false },
        { "onmessage", s_get_onmessage, s_set_onmessage, false },
        { "onerror", s_get_onerror, s_set_onerror, false }
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

template <typename T>
void Worker_base::__new(const T& args)
{
    obj_ptr<Worker_base> vr;

    CONSTRUCT_ENTER();

    METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate->m_isolate));

    hr = _new(v0, v1, vr, args.This());

    CONSTRUCT_RETURN();
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

inline void Worker_base::s_get_onload(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_INSTANCE(Worker_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_onload(vr);

    METHOD_RETURN();
}

inline void Worker_base::s_set_onload(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(Worker_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Function>, 0);

    hr = pInst->set_onload(v0);

    METHOD_VOID();
}

inline void Worker_base::s_get_onmessage(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_INSTANCE(Worker_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_onmessage(vr);

    METHOD_RETURN();
}

inline void Worker_base::s_set_onmessage(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(Worker_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Function>, 0);

    hr = pInst->set_onmessage(v0);

    METHOD_VOID();
}

inline void Worker_base::s_get_onerror(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_INSTANCE(Worker_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_onerror(vr);

    METHOD_RETURN();
}

inline void Worker_base::s_set_onerror(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(Worker_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Function>, 0);

    hr = pInst->set_onerror(v0);

    METHOD_VOID();
}
}
