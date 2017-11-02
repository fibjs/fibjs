/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _EventInfo_base_H_
#define _EventInfo_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs {

class EventInfo_base : public object_base {
    DECLARE_CLASS(EventInfo_base);

public:
    // EventInfo_base
    virtual result_t get_code(int32_t& retVal) = 0;
    virtual result_t get_reason(exlib::string& retVal) = 0;
    virtual result_t get_type(exlib::string& retVal) = 0;
    virtual result_t get_target(v8::Local<v8::Object>& retVal) = 0;
    virtual result_t _named_getter(exlib::string property, int32_t& retVal) = 0;
    virtual result_t _named_enumerator(v8::Local<v8::Array>& retVal) = 0;

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        Isolate* isolate = Isolate::current();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_get_code(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_reason(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_type(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_target(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void i_NamedGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void i_NamedEnumerator(const v8::PropertyCallbackInfo<v8::Array>& args);
};
}

namespace fibjs {
inline ClassInfo& EventInfo_base::class_info()
{
    static ClassData::ClassProperty s_property[] = {
        { "code", s_get_code, block_set, false },
        { "reason", s_get_reason, block_set, false },
        { "type", s_get_type, block_set, false },
        { "target", s_get_target, block_set, false }
    };

    static ClassData::ClassNamed s_named = {
        i_NamedGetter, i_NamedSetter, i_NamedDeleter, i_NamedEnumerator
    };

    static ClassData s_cd = {
        "EventInfo", false, s__new, NULL,
        0, NULL, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, &s_named,
        &object_base::class_info()
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void EventInfo_base::s_get_code(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_NAME("EventInfo.code");
    METHOD_INSTANCE(EventInfo_base);
    PROPERTY_ENTER();

    hr = pInst->get_code(vr);

    METHOD_RETURN();
}

inline void EventInfo_base::s_get_reason(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_NAME("EventInfo.reason");
    METHOD_INSTANCE(EventInfo_base);
    PROPERTY_ENTER();

    hr = pInst->get_reason(vr);

    METHOD_RETURN();
}

inline void EventInfo_base::s_get_type(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_NAME("EventInfo.type");
    METHOD_INSTANCE(EventInfo_base);
    PROPERTY_ENTER();

    hr = pInst->get_type(vr);

    METHOD_RETURN();
}

inline void EventInfo_base::s_get_target(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Object> vr;

    METHOD_NAME("EventInfo.target");
    METHOD_INSTANCE(EventInfo_base);
    PROPERTY_ENTER();

    hr = pInst->get_target(vr);

    METHOD_RETURN();
}

inline void EventInfo_base::i_NamedGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_NAME("EventInfo.undefined[]");
    METHOD_INSTANCE(EventInfo_base);
    PROPERTY_ENTER();

    exlib::string k;
    GetArgumentValue(isolate, property, k);
    if (class_info().has(k.c_str()))
        return;

    hr = pInst->_named_getter(k, vr);
    if (hr == CALL_RETURN_NULL)
        return;

    METHOD_RETURN();
}

inline void EventInfo_base::i_NamedEnumerator(const v8::PropertyCallbackInfo<v8::Array>& args)
{
    v8::Local<v8::Array> vr;

    METHOD_NAME("EventInfo.undefined");
    METHOD_INSTANCE(EventInfo_base);
    PROPERTY_ENTER();

    hr = pInst->_named_enumerator(vr);

    METHOD_RETURN1();
}
}

#endif
