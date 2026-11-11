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

class MessageEvent_base : public object_base {
    DECLARE_CLASS(MessageEvent_base);

public:
    // MessageEvent_base
    static result_t _new(exlib::string type, v8::Local<v8::Object> eventInitDict, obj_ptr<MessageEvent_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    virtual result_t get_type(exlib::string& retVal) = 0;
    virtual result_t get_origin(exlib::string& retVal) = 0;
    virtual result_t get_lastEventId(exlib::string& retVal) = 0;
    virtual result_t get_source(v8::Local<v8::Value>& retVal) = 0;
    virtual result_t get_ports(v8::Local<v8::Array>& retVal) = 0;
    virtual result_t get_data(v8::Local<v8::Value>& retVal) = 0;

public:
    static void __new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static result_t load(v8::Local<v8::Value> v, obj_ptr<MessageEvent_base>& retVal);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_type(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_origin(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_lastEventId(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_source(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_ports(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_data(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

namespace fibjs {
inline ClassInfo& MessageEvent_base::class_info()
{
    static ClassData::ClassProperty s_property[] = {
        { "type", s_get_type, block_set, false },
        { "origin", s_get_origin, block_set, false },
        { "lastEventId", s_get_lastEventId, block_set, false },
        { "source", s_get_source, block_set, false },
        { "ports", s_get_ports, block_set, false },
        { "data", s_get_data, block_set, false }
    };

    static ClassData s_cd = {
        "MessageEvent", false, s__new, NULL,
        0, NULL, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &object_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void MessageEvent_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    CONSTRUCT_INIT();
    __new(args);
}

inline void MessageEvent_base::__new(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<MessageEvent_base> vr;

    CONSTRUCT_ENTER();

    METHOD_OVER_NAMED(2, 1, "type");

    ARG_NAMED(exlib::string, 0, "type");
    OPT_ARG_NAMED(v8::Local<v8::Object>, 1, v8::Object::New(isolate->m_isolate), "eventInitDict");

    hr = _new(v0, v1, vr, args.This());

    CONSTRUCT_RETURN();
}

inline result_t MessageEvent_base::load(v8::Local<v8::Value> v, obj_ptr<MessageEvent_base>& retVal)
{
    obj_ptr<MessageEvent_base> vr;

    LOAD_ENTER();

    METHOD_OVER_NAMED(2, 1, "type");

    ARG_NAMED(exlib::string, 0, "type");
    OPT_ARG_NAMED(v8::Local<v8::Object>, 1, v8::Object::New(isolate->m_isolate), "eventInitDict");

    hr = _new(v0, v1, vr, args.This());

    LOAD_RETURN();
}

inline void MessageEvent_base::s_get_type(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(MessageEvent_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_type(vr);

    METHOD_RETURN();
}

inline void MessageEvent_base::s_get_origin(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(MessageEvent_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_origin(vr);

    METHOD_RETURN();
}

inline void MessageEvent_base::s_get_lastEventId(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(MessageEvent_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_lastEventId(vr);

    METHOD_RETURN();
}

inline void MessageEvent_base::s_get_source(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Value> vr;

    METHOD_INSTANCE(MessageEvent_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_source(vr);

    METHOD_RETURN();
}

inline void MessageEvent_base::s_get_ports(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Array> vr;

    METHOD_INSTANCE(MessageEvent_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_ports(vr);

    METHOD_RETURN();
}

inline void MessageEvent_base::s_get_data(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Value> vr;

    METHOD_INSTANCE(MessageEvent_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_data(vr);

    METHOD_RETURN();
}
}
