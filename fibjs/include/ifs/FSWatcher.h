/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _FSWatcher_base_H_
#define _FSWatcher_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "ifs/EventEmitter.h"

namespace fibjs {

class EventEmitter_base;

class FSWatcher_base : public EventEmitter_base {
    DECLARE_CLASS(FSWatcher_base);

public:
    // FSWatcher_base
    virtual result_t get_onchange(v8::Local<v8::Function>& retVal) = 0;
    virtual result_t set_onchange(v8::Local<v8::Function> newVal) = 0;
    virtual result_t get_onclose(v8::Local<v8::Function>& retVal) = 0;
    virtual result_t set_onclose(v8::Local<v8::Function> newVal) = 0;
    virtual result_t get_onerror(v8::Local<v8::Function>& retVal) = 0;
    virtual result_t set_onerror(v8::Local<v8::Function> newVal) = 0;
    virtual result_t close() = 0;

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        Isolate* isolate = Isolate::current();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_get_onchange(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_set_onchange(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_get_onclose(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_set_onclose(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_get_onerror(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_set_onerror(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_close(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

namespace fibjs {
inline ClassInfo& FSWatcher_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "close", s_close, false }
    };

    static ClassData::ClassProperty s_property[] = {
        { "onchange", s_get_onchange, s_set_onchange, false },
        { "onclose", s_get_onclose, s_set_onclose, false },
        { "onerror", s_get_onerror, s_set_onerror, false }
    };

    static ClassData s_cd = {
        "FSWatcher", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &EventEmitter_base::class_info()
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void FSWatcher_base::s_get_onchange(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_NAME("FSWatcher.onchange");
    METHOD_INSTANCE(FSWatcher_base);
    PROPERTY_ENTER();

    hr = pInst->get_onchange(vr);

    METHOD_RETURN();
}

inline void FSWatcher_base::s_set_onchange(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_NAME("FSWatcher.onchange");
    METHOD_INSTANCE(FSWatcher_base);
    PROPERTY_ENTER();
    PROPERTY_VAL(v8::Local<v8::Function>);

    hr = pInst->set_onchange(v0);

    PROPERTY_SET_LEAVE();
}

inline void FSWatcher_base::s_get_onclose(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_NAME("FSWatcher.onclose");
    METHOD_INSTANCE(FSWatcher_base);
    PROPERTY_ENTER();

    hr = pInst->get_onclose(vr);

    METHOD_RETURN();
}

inline void FSWatcher_base::s_set_onclose(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_NAME("FSWatcher.onclose");
    METHOD_INSTANCE(FSWatcher_base);
    PROPERTY_ENTER();
    PROPERTY_VAL(v8::Local<v8::Function>);

    hr = pInst->set_onclose(v0);

    PROPERTY_SET_LEAVE();
}

inline void FSWatcher_base::s_get_onerror(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_NAME("FSWatcher.onerror");
    METHOD_INSTANCE(FSWatcher_base);
    PROPERTY_ENTER();

    hr = pInst->get_onerror(vr);

    METHOD_RETURN();
}

inline void FSWatcher_base::s_set_onerror(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_NAME("FSWatcher.onerror");
    METHOD_INSTANCE(FSWatcher_base);
    PROPERTY_ENTER();
    PROPERTY_VAL(v8::Local<v8::Function>);

    hr = pInst->set_onerror(v0);

    PROPERTY_SET_LEAVE();
}

inline void FSWatcher_base::s_close(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("FSWatcher.close");
    METHOD_INSTANCE(FSWatcher_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->close();

    METHOD_VOID();
}
}

#endif
