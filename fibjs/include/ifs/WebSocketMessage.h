/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _WebSocketMessage_base_H_
#define _WebSocketMessage_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "Message.h"

namespace fibjs
{

class Message_base;
class ws_base;

class WebSocketMessage_base : public Message_base
{
    DECLARE_CLASS(WebSocketMessage_base);

public:
    // WebSocketMessage_base
    static result_t _new(int32_t type, bool masked, int32_t maxSize, obj_ptr<WebSocketMessage_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    virtual result_t get_type(int32_t& retVal) = 0;
    virtual result_t set_type(int32_t newVal) = 0;
    virtual result_t get_data(v8::Local<v8::Value>& retVal) = 0;
    virtual result_t get_masked(bool& retVal) = 0;
    virtual result_t set_masked(bool newVal) = 0;
    virtual result_t get_maxSize(int32_t& retVal) = 0;
    virtual result_t set_maxSize(int32_t newVal) = 0;

public:
    template<typename T>
    static void __new(const T &args);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_type(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_set_type(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args);
    static void s_get_data(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get_masked(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_set_masked(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args);
    static void s_get_maxSize(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_set_maxSize(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args);
};

}

#include "ws.h"

namespace fibjs
{
    inline ClassInfo& WebSocketMessage_base::class_info()
    {
        static ClassData::ClassProperty s_property[] = 
        {
            {"type", s_get_type, s_set_type, false},
            {"data", s_get_data, block_set, false},
            {"masked", s_get_masked, s_set_masked, false},
            {"maxSize", s_get_maxSize, s_set_maxSize, false}
        };

        static ClassData s_cd = 
        { 
            "WebSocketMessage", s__new, NULL, 
            0, NULL, 0, NULL, ARRAYSIZE(s_property), s_property, NULL, NULL,
            &Message_base::class_info()
        };

        static ClassInfo s_ci(s_cd);
        return s_ci;
    }

    inline void WebSocketMessage_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();
        __new(args);
    }

    template<typename T>void WebSocketMessage_base::__new(const T& args)
    {
        obj_ptr<WebSocketMessage_base> vr;

        CONSTRUCT_ENTER();

        METHOD_OVER(3, 0);

        OPT_ARG(int32_t, 0, ws_base::_BINARY);
        OPT_ARG(bool, 1, true);
        OPT_ARG(int32_t, 2, 67108864);

        hr = _new(v0, v1, v2, vr, args.This());

        CONSTRUCT_RETURN();
    }

    inline void WebSocketMessage_base::s_get_type(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        int32_t vr;

        METHOD_INSTANCE(WebSocketMessage_base);
        PROPERTY_ENTER();

        hr = pInst->get_type(vr);

        METHOD_RETURN();
    }

    inline void WebSocketMessage_base::s_set_type(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args)
    {
        METHOD_INSTANCE(WebSocketMessage_base);
        PROPERTY_ENTER();
        PROPERTY_VAL(int32_t);

        hr = pInst->set_type(v0);

        PROPERTY_SET_LEAVE();
    }

    inline void WebSocketMessage_base::s_get_data(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        v8::Local<v8::Value> vr;

        METHOD_INSTANCE(WebSocketMessage_base);
        PROPERTY_ENTER();

        hr = pInst->get_data(vr);

        METHOD_RETURN();
    }

    inline void WebSocketMessage_base::s_get_masked(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        bool vr;

        METHOD_INSTANCE(WebSocketMessage_base);
        PROPERTY_ENTER();

        hr = pInst->get_masked(vr);

        METHOD_RETURN();
    }

    inline void WebSocketMessage_base::s_set_masked(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args)
    {
        METHOD_INSTANCE(WebSocketMessage_base);
        PROPERTY_ENTER();
        PROPERTY_VAL(bool);

        hr = pInst->set_masked(v0);

        PROPERTY_SET_LEAVE();
    }

    inline void WebSocketMessage_base::s_get_maxSize(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        int32_t vr;

        METHOD_INSTANCE(WebSocketMessage_base);
        PROPERTY_ENTER();

        hr = pInst->get_maxSize(vr);

        METHOD_RETURN();
    }

    inline void WebSocketMessage_base::s_set_maxSize(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args)
    {
        METHOD_INSTANCE(WebSocketMessage_base);
        PROPERTY_ENTER();
        PROPERTY_VAL(int32_t);

        hr = pInst->set_maxSize(v0);

        PROPERTY_SET_LEAVE();
    }

}

#endif

