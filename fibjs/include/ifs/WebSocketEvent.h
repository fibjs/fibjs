/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _WebSocketEvent_base_H_
#define _WebSocketEvent_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs
{

class WebSocketEvent_base : public object_base
{
    DECLARE_CLASS(WebSocketEvent_base);

public:
    // WebSocketEvent_base
    virtual result_t get_code(int32_t& retVal) = 0;
    virtual result_t get_reason(exlib::string& retVal) = 0;
    virtual result_t get_type(exlib::string& retVal) = 0;
    virtual result_t get_target(v8::Local<v8::Object>& retVal) = 0;

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        Isolate* isolate = Isolate::current();

        isolate->m_isolate->ThrowException(
            isolate->NewFromUtf8("not a constructor"));
    }

public:
    static void s_get_code(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get_reason(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get_type(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get_target(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
};

}

namespace fibjs
{
    inline ClassInfo& WebSocketEvent_base::class_info()
    {
        static ClassData::ClassProperty s_property[] = 
        {
            {"code", s_get_code, block_set, false},
            {"reason", s_get_reason, block_set, false},
            {"type", s_get_type, block_set, false},
            {"target", s_get_target, block_set, false}
        };

        static ClassData s_cd = 
        { 
            "WebSocketEvent", s__new, NULL, 
            0, NULL, 0, NULL, ARRAYSIZE(s_property), s_property, NULL, NULL,
            &object_base::class_info()
        };

        static ClassInfo s_ci(s_cd);
        return s_ci;
    }

    inline void WebSocketEvent_base::s_get_code(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        int32_t vr;

        METHOD_INSTANCE(WebSocketEvent_base);
        PROPERTY_ENTER();

        hr = pInst->get_code(vr);

        METHOD_RETURN();
    }

    inline void WebSocketEvent_base::s_get_reason(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        exlib::string vr;

        METHOD_INSTANCE(WebSocketEvent_base);
        PROPERTY_ENTER();

        hr = pInst->get_reason(vr);

        METHOD_RETURN();
    }

    inline void WebSocketEvent_base::s_get_type(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        exlib::string vr;

        METHOD_INSTANCE(WebSocketEvent_base);
        PROPERTY_ENTER();

        hr = pInst->get_type(vr);

        METHOD_RETURN();
    }

    inline void WebSocketEvent_base::s_get_target(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        v8::Local<v8::Object> vr;

        METHOD_INSTANCE(WebSocketEvent_base);
        PROPERTY_ENTER();

        hr = pInst->get_target(vr);

        METHOD_RETURN();
    }

}

#endif

