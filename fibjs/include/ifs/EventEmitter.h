/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _EventEmitter_base_H_
#define _EventEmitter_base_H_

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
    virtual result_t on(exlib::string ev, v8::Local<v8::Function> func, v8::Local<v8::Object>& retVal) = 0;
    virtual result_t on(v8::Local<v8::Object> map, v8::Local<v8::Object>& retVal) = 0;
    virtual result_t addListener(exlib::string ev, v8::Local<v8::Function> func, v8::Local<v8::Object>& retVal) = 0;
    virtual result_t addListener(v8::Local<v8::Object> map, v8::Local<v8::Object>& retVal) = 0;
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
    virtual result_t removeAllListeners(v8::Local<v8::Array> evs, v8::Local<v8::Object>& retVal) = 0;
    virtual result_t setMaxListeners(int32_t n) = 0;
    virtual result_t listeners(exlib::string ev, v8::Local<v8::Array>& retVal) = 0;
    virtual result_t listenerCount(exlib::string ev, int32_t& retVal) = 0;
    virtual result_t eventNames(v8::Local<v8::Array>& retVal) = 0;
    virtual result_t emit(exlib::string ev, const v8::FunctionCallbackInfo<v8::Value>& args, bool& retVal) = 0;

public:
    template <typename T>
    static void __new(const T& args);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_on(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_addListener(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_prependListener(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_once(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_prependOnceListener(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_off(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_removeListener(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_removeAllListeners(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_setMaxListeners(const v8::FunctionCallbackInfo<v8::Value>& args);
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
        { "on", s_on, false },
        { "addListener", s_addListener, false },
        { "prependListener", s_prependListener, false },
        { "once", s_once, false },
        { "prependOnceListener", s_prependOnceListener, false },
        { "off", s_off, false },
        { "removeListener", s_removeListener, false },
        { "removeAllListeners", s_removeAllListeners, false },
        { "setMaxListeners", s_setMaxListeners, false },
        { "listeners", s_listeners, false },
        { "listenerCount", s_listenerCount, false },
        { "eventNames", s_eventNames, false },
        { "emit", s_emit, false }
    };

    static ClassData s_cd = {
        "EventEmitter", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, 0, NULL, NULL, NULL,
        &object_base::class_info()
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

inline void EventEmitter_base::s_removeAllListeners(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Object> vr;

    METHOD_INSTANCE(EventEmitter_base);
    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(v8::Local<v8::Array>, 0, v8::Array::New(isolate));

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

    hr = pInst->emit(v0, args, vr);

    METHOD_RETURN();
}
}

#endif
