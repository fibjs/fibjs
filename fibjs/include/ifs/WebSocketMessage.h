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
#include "ifs/Message.h"

namespace fibjs {

class Message_base;
class ws_base;

class WebSocketMessage_base : public Message_base {
    DECLARE_CLASS(WebSocketMessage_base);

public:
    // WebSocketMessage_base
    static result_t _new(int32_t type, bool masked, bool compress, int32_t maxSize, obj_ptr<WebSocketMessage_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    virtual result_t get_masked(bool& retVal) = 0;
    virtual result_t set_masked(bool newVal) = 0;
    virtual result_t get_compress(bool& retVal) = 0;
    virtual result_t set_compress(bool newVal) = 0;
    virtual result_t get_maxSize(int32_t& retVal) = 0;
    virtual result_t set_maxSize(int32_t newVal) = 0;

public:
    template <typename T>
    static void __new(const T& args);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_masked(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_masked(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_compress(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_compress(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_maxSize(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_maxSize(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

#include "ifs/ws.h"

namespace fibjs {
inline ClassInfo& WebSocketMessage_base::class_info()
{
    static ClassData::ClassProperty s_property[] = {
        { "masked", s_get_masked, s_set_masked, false },
        { "compress", s_get_compress, s_set_compress, false },
        { "maxSize", s_get_maxSize, s_set_maxSize, false }
    };

    static ClassData s_cd = {
        "WebSocketMessage", false, s__new, NULL,
        0, NULL, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &Message_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void WebSocketMessage_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    CONSTRUCT_INIT();
    __new(args);
}

template <typename T>
void WebSocketMessage_base::__new(const T& args)
{
    obj_ptr<WebSocketMessage_base> vr;

    CONSTRUCT_ENTER();

    METHOD_OVER(4, 0);

    OPT_ARG(int32_t, 0, ws_base::C_BINARY);
    OPT_ARG(bool, 1, true);
    OPT_ARG(bool, 2, false);
    OPT_ARG(int32_t, 3, 67108864);

    hr = _new(v0, v1, v2, v3, vr, args.This());

    CONSTRUCT_RETURN();
}

inline void WebSocketMessage_base::s_get_masked(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_INSTANCE(WebSocketMessage_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_masked(vr);

    METHOD_RETURN();
}

inline void WebSocketMessage_base::s_set_masked(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(WebSocketMessage_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(bool, 0);

    hr = pInst->set_masked(v0);

    METHOD_VOID();
}

inline void WebSocketMessage_base::s_get_compress(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_INSTANCE(WebSocketMessage_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_compress(vr);

    METHOD_RETURN();
}

inline void WebSocketMessage_base::s_set_compress(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(WebSocketMessage_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(bool, 0);

    hr = pInst->set_compress(v0);

    METHOD_VOID();
}

inline void WebSocketMessage_base::s_get_maxSize(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(WebSocketMessage_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_maxSize(vr);

    METHOD_RETURN();
}

inline void WebSocketMessage_base::s_set_maxSize(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(WebSocketMessage_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(int32_t, 0);

    hr = pInst->set_maxSize(v0);

    METHOD_VOID();
}
}
