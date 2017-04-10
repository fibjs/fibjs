/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _WebSocketHandler_base_H_
#define _WebSocketHandler_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "HandlerEx.h"

namespace fibjs {

class HandlerEx_base;

class WebSocketHandler_base : public HandlerEx_base {
    DECLARE_CLASS(WebSocketHandler_base);

public:
    // WebSocketHandler_base
    static result_t _new(v8::Local<v8::Value> hdlr, obj_ptr<WebSocketHandler_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    virtual result_t get_maxSize(int32_t& retVal) = 0;
    virtual result_t set_maxSize(int32_t newVal) = 0;

public:
    template <typename T>
    static void __new(const T& args);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_maxSize(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_set_maxSize(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
};
}

namespace fibjs {
inline ClassInfo& WebSocketHandler_base::class_info()
{
    static ClassData::ClassProperty s_property[] = {
        { "maxSize", s_get_maxSize, s_set_maxSize, false }
    };

    static ClassData s_cd = {
        "WebSocketHandler", false, s__new, NULL,
        0, NULL, 0, NULL, ARRAYSIZE(s_property), s_property, NULL, NULL,
        &HandlerEx_base::class_info()
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void WebSocketHandler_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    CONSTRUCT_INIT();
    __new(args);
}

template <typename T>
void WebSocketHandler_base::__new(const T& args)
{
    obj_ptr<WebSocketHandler_base> vr;

    CONSTRUCT_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = _new(v0, vr, args.This());

    CONSTRUCT_RETURN();
}

inline void WebSocketHandler_base::s_get_maxSize(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(WebSocketHandler_base);
    PROPERTY_ENTER();

    hr = pInst->get_maxSize(vr);

    METHOD_RETURN();
}

inline void WebSocketHandler_base::s_set_maxSize(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_INSTANCE(WebSocketHandler_base);
    PROPERTY_ENTER();
    PROPERTY_VAL(int32_t);

    hr = pInst->set_maxSize(v0);

    PROPERTY_SET_LEAVE();
}
}

#endif
