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

namespace fibjs {

class EventEmitter_base : public object_base {
    DECLARE_CLASS(EventEmitter_base);

public:
    // EventEmitter_base
    static result_t _new(obj_ptr<EventEmitter_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    static result_t get_defaultMaxListeners(int32_t& retVal);
    static result_t set_defaultMaxListeners(int32_t newVal);
    virtual result_t on(exlib::string ev, v8::Local<v8::Function> func, v8::Local<v8::Object>& retVal) = 0;
    virtual result_t on(v8::Local<v8::Object> map, v8::Local<v8::Object>& retVal) = 0;
    virtual result_t addListener(exlib::string ev, v8::Local<v8::Function> func, v8::Local<v8::Object>& retVal) = 0;
    virtual result_t addListener(v8::Local<v8::Object> map, v8::Local<v8::Object>& retVal) = 0;
    virtual result_t addEventListener(exlib::string ev, v8::Local<v8::Function> func, v8::Local<v8::Object> options, v8::Local<v8::Object>& retVal) = 0;
    virtual result_t prependListener(exlib::string ev, v8::Local<v8::Function> func, v8::Local<v8::Object>& retVal) = 0;
    virtual result_t prependListener(v8::Local<v8::Object> map, v8::Local<v8::Object>& retVal) = 0;
    virtual result_t once(exlib::string ev, v8::Local<v8::Function> func, v8::Local<v8::Object>& retVal) = 0;
    virtual result_t once(v8::Local<v8::Object> map, v8::Local<v8::Object>& retVal) = 0;
    virtual result_t prependOnceListener(exlib::string ev, v8::Local<v8::Function> func, v8::Local<v8::Object>& retVal) = 0;
    virtual result_t prependOnceListener(v8::Local<v8::Object> map, v8::Local<v8::Object>& retVal) = 0;
    virtual result_t off(exlib::string ev, v8::Local<v8::Function> func, v8::Local<v8::Object>& retVal) = 0;
    virtual result_t off(exlib::string ev, v8::Local<v8::Object>& retVal) = 0;
    virtual result_t off(v8::Local<v8::Object> map, v8::Local<v8::Object>& retVal) = 0;
    virtual result_t removeListener(exlib::string ev, v8::Local<v8::Function> func, v8::Local<v8::Object>& retVal) = 0;
    virtual result_t removeListener(exlib::string ev, v8::Local<v8::Object>& retVal) = 0;
    virtual result_t removeListener(v8::Local<v8::Object> map, v8::Local<v8::Object>& retVal) = 0;
    virtual result_t removeEventListener(exlib::string ev, v8::Local<v8::Function> func, v8::Local<v8::Object> options, v8::Local<v8::Object>& retVal) = 0;
    virtual result_t removeAllListeners(exlib::string ev, v8::Local<v8::Object>& retVal) = 0;
    virtual result_t removeAllListeners(v8::Local<v8::Array> evs, v8::Local<v8::Object>& retVal) = 0;
    virtual result_t setMaxListeners(int32_t n) = 0;
    virtual result_t getMaxListeners(int32_t& retVal) = 0;
    virtual result_t listeners(exlib::string ev, v8::Local<v8::Array>& retVal) = 0;
    virtual result_t listenerCount(exlib::string ev, int32_t& retVal) = 0;
    virtual result_t listenerCount(v8::Local<v8::Value> o, exlib::string ev, int32_t& retVal) = 0;
    virtual result_t eventNames(v8::Local<v8::Array>& retVal) = 0;
    virtual result_t emit(exlib::string ev, OptArgs args, bool& retVal) = 0;

public:
    template <typename T>
    static void __new(const T& args);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_get_defaultMaxListeners(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_static_set_defaultMaxListeners(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_on(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_addListener(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_addEventListener(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_prependListener(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_once(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_prependOnceListener(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_off(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_removeListener(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_removeEventListener(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_removeAllListeners(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_setMaxListeners(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_getMaxListeners(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_listeners(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_listenerCount(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_eventNames(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_emit(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

namespace fibjs {
inline ClassInfo& EventEmitter_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "on", s_on, false, ClassData::ASYNC_SYNC },
        { "addListener", s_addListener, false, ClassData::ASYNC_SYNC },
        { "addEventListener", s_addEventListener, false, ClassData::ASYNC_SYNC },
        { "prependListener", s_prependListener, false, ClassData::ASYNC_SYNC },
        { "once", s_once, false, ClassData::ASYNC_SYNC },
        { "prependOnceListener", s_prependOnceListener, false, ClassData::ASYNC_SYNC },
        { "off", s_off, false, ClassData::ASYNC_SYNC },
        { "removeListener", s_removeListener, false, ClassData::ASYNC_SYNC },
        { "removeEventListener", s_removeEventListener, false, ClassData::ASYNC_SYNC },
        { "removeAllListeners", s_removeAllListeners, false, ClassData::ASYNC_SYNC },
        { "setMaxListeners", s_setMaxListeners, false, ClassData::ASYNC_SYNC },
        { "getMaxListeners", s_getMaxListeners, false, ClassData::ASYNC_SYNC },
        { "listeners", s_listeners, false, ClassData::ASYNC_SYNC },
        { "listenerCount", s_listenerCount, false, ClassData::ASYNC_SYNC },
        { "eventNames", s_eventNames, false, ClassData::ASYNC_SYNC },
        { "emit", s_emit, false, ClassData::ASYNC_SYNC }
    };

    static ClassData::ClassObject s_object[] = {
        { "EventEmitter", EventEmitter_base::class_info }
    };

    static ClassData::ClassProperty s_property[] = {
        { "defaultMaxListeners", s_static_get_defaultMaxListeners, s_static_set_defaultMaxListeners, true }
    };

    static ClassData s_cd = {
        "EventEmitter", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, ARRAYSIZE(s_object), s_object, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &object_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void EventEmitter_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    CONSTRUCT_INIT();
    __new(args);
}

template <typename T>
void EventEmitter_base::__new(const T& args)
{
    obj_ptr<EventEmitter_base> vr;

    CONSTRUCT_ENTER();

    METHOD_OVER(0, 0);

    hr = _new(vr, args.This());

    CONSTRUCT_RETURN();
}

inline void EventEmitter_base::s_static_get_defaultMaxListeners(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    PROPERTY_ENTER();

    hr = get_defaultMaxListeners(vr);

    METHOD_RETURN();
}

inline void EventEmitter_base::s_static_set_defaultMaxListeners(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    PROPERTY_ENTER();
    PROPERTY_VAL(int32_t);

    hr = set_defaultMaxListeners(v0);

    PROPERTY_SET_LEAVE();
}

inline void EventEmitter_base::s_on(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Object> vr;

    METHOD_INSTANCE(EventEmitter_base);
    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(v8::Local<v8::Function>, 1);

    hr = pInst->on(v0, v1, vr);

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Object>, 0);

    hr = pInst->on(v0, vr);

    METHOD_RETURN();
}

inline void EventEmitter_base::s_addListener(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Object> vr;

    METHOD_INSTANCE(EventEmitter_base);
    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(v8::Local<v8::Function>, 1);

    hr = pInst->addListener(v0, v1, vr);

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Object>, 0);

    hr = pInst->addListener(v0, vr);

    METHOD_RETURN();
}

inline void EventEmitter_base::s_addEventListener(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Object> vr;

    METHOD_INSTANCE(EventEmitter_base);
    METHOD_ENTER();

    METHOD_OVER(3, 2);

    ARG(exlib::string, 0);
    ARG(v8::Local<v8::Function>, 1);
    OPT_ARG(v8::Local<v8::Object>, 2, v8::Object::New(isolate->m_isolate));

    hr = pInst->addEventListener(v0, v1, v2, vr);

    METHOD_RETURN();
}

inline void EventEmitter_base::s_prependListener(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Object> vr;

    METHOD_INSTANCE(EventEmitter_base);
    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(v8::Local<v8::Function>, 1);

    hr = pInst->prependListener(v0, v1, vr);

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Object>, 0);

    hr = pInst->prependListener(v0, vr);

    METHOD_RETURN();
}

inline void EventEmitter_base::s_once(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Object> vr;

    METHOD_INSTANCE(EventEmitter_base);
    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(v8::Local<v8::Function>, 1);

    hr = pInst->once(v0, v1, vr);

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Object>, 0);

    hr = pInst->once(v0, vr);

    METHOD_RETURN();
}

inline void EventEmitter_base::s_prependOnceListener(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Object> vr;

    METHOD_INSTANCE(EventEmitter_base);
    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(v8::Local<v8::Function>, 1);

    hr = pInst->prependOnceListener(v0, v1, vr);

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Object>, 0);

    hr = pInst->prependOnceListener(v0, vr);

    METHOD_RETURN();
}

inline void EventEmitter_base::s_off(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Object> vr;

    METHOD_INSTANCE(EventEmitter_base);
    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(v8::Local<v8::Function>, 1);

    hr = pInst->off(v0, v1, vr);

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = pInst->off(v0, vr);

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Object>, 0);

    hr = pInst->off(v0, vr);

    METHOD_RETURN();
}

inline void EventEmitter_base::s_removeListener(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Object> vr;

    METHOD_INSTANCE(EventEmitter_base);
    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(v8::Local<v8::Function>, 1);

    hr = pInst->removeListener(v0, v1, vr);

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = pInst->removeListener(v0, vr);

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Object>, 0);

    hr = pInst->removeListener(v0, vr);

    METHOD_RETURN();
}

inline void EventEmitter_base::s_removeEventListener(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Object> vr;

    METHOD_INSTANCE(EventEmitter_base);
    METHOD_ENTER();

    METHOD_OVER(3, 2);

    ARG(exlib::string, 0);
    ARG(v8::Local<v8::Function>, 1);
    OPT_ARG(v8::Local<v8::Object>, 2, v8::Object::New(isolate->m_isolate));

    hr = pInst->removeEventListener(v0, v1, v2, vr);

    METHOD_RETURN();
}

inline void EventEmitter_base::s_removeAllListeners(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Object> vr;

    METHOD_INSTANCE(EventEmitter_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = pInst->removeAllListeners(v0, vr);

    METHOD_OVER(1, 0);

    OPT_ARG(v8::Local<v8::Array>, 0, v8::Array::New(isolate->m_isolate));

    hr = pInst->removeAllListeners(v0, vr);

    METHOD_RETURN();
}

inline void EventEmitter_base::s_setMaxListeners(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(EventEmitter_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(int32_t, 0);

    hr = pInst->setMaxListeners(v0);

    METHOD_VOID();
}

inline void EventEmitter_base::s_getMaxListeners(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(EventEmitter_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->getMaxListeners(vr);

    METHOD_RETURN();
}

inline void EventEmitter_base::s_listeners(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Array> vr;

    METHOD_INSTANCE(EventEmitter_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = pInst->listeners(v0, vr);

    METHOD_RETURN();
}

inline void EventEmitter_base::s_listenerCount(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(EventEmitter_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = pInst->listenerCount(v0, vr);

    METHOD_OVER(2, 2);

    ARG(v8::Local<v8::Value>, 0);
    ARG(exlib::string, 1);

    hr = pInst->listenerCount(v0, v1, vr);

    METHOD_RETURN();
}

inline void EventEmitter_base::s_eventNames(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Array> vr;

    METHOD_INSTANCE(EventEmitter_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->eventNames(vr);

    METHOD_RETURN();
}

inline void EventEmitter_base::s_emit(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_INSTANCE(EventEmitter_base);
    METHOD_ENTER();

    METHOD_OVER(-1, 1);

    ARG(exlib::string, 0);
    ARG_LIST(1);

    hr = pInst->emit(v0, v1, vr);

    METHOD_RETURN();
}
}
