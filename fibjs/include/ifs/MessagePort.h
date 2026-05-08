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

class MessagePort_base : public EventEmitter_base {
    DECLARE_CLASS(MessagePort_base);
    EVENT_SUPPORT();

public:
    // MessagePort_base
    virtual result_t postMessage(v8::Local<v8::Value> data) = 0;
    virtual result_t postMessage(v8::Local<v8::Value> data, v8::Local<v8::Array> transfer) = 0;
    virtual result_t start() = 0;
    virtual result_t close() = 0;
    virtual result_t ref() = 0;
    virtual result_t unref() = 0;

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        ThrowTypeError("not a constructor");
    }

    static result_t load(v8::Local<v8::Value> v, obj_ptr<MessagePort_base>& retVal)
    { return CALL_E_TYPEMISMATCH; }

public:
    static void s_postMessage(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_start(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_close(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_ref(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_unref(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_onmessage(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_onmessage(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_onmessageerror(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_onmessageerror(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

namespace fibjs {
inline ClassInfo& MessagePort_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "postMessage", s_postMessage, false, ClassData::ASYNC_SYNC },
        { "start", s_start, false, ClassData::ASYNC_SYNC },
        { "close", s_close, false, ClassData::ASYNC_SYNC },
        { "ref", s_ref, false, ClassData::ASYNC_SYNC },
        { "unref", s_unref, false, ClassData::ASYNC_SYNC }
    };

    static ClassData::ClassProperty s_property[] = {
        { "onmessage", s_get_onmessage, s_set_onmessage, false },
        { "onmessageerror", s_get_onmessageerror, s_set_onmessageerror, false }
    };

    static ClassData s_cd = {
        "MessagePort", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &EventEmitter_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void MessagePort_base::s_postMessage(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(MessagePort_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = pInst->postMessage(v0);

    METHOD_OVER(2, 2);

    ARG(v8::Local<v8::Value>, 0);
    ARG(v8::Local<v8::Array>, 1);

    hr = pInst->postMessage(v0, v1);

    METHOD_VOID();
}

inline void MessagePort_base::s_start(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(MessagePort_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->start();

    METHOD_VOID();
}

inline void MessagePort_base::s_close(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(MessagePort_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->close();

    METHOD_VOID();
}

inline void MessagePort_base::s_ref(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(MessagePort_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->ref();

    METHOD_VOID();
}

inline void MessagePort_base::s_unref(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(MessagePort_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->unref();

    METHOD_VOID();
}

inline void MessagePort_base::s_get_onmessage(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_INSTANCE(MessagePort_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->getListener("message", vr);

    METHOD_RETURN();
}

inline void MessagePort_base::s_set_onmessage(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(MessagePort_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Function>, 0);

    hr = pInst->setListener("message", v0);

    METHOD_VOID();
}

inline void MessagePort_base::s_get_onmessageerror(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_INSTANCE(MessagePort_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->getListener("messageerror", vr);

    METHOD_RETURN();
}

inline void MessagePort_base::s_set_onmessageerror(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(MessagePort_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Function>, 0);

    hr = pInst->setListener("messageerror", v0);

    METHOD_VOID();
}
}
