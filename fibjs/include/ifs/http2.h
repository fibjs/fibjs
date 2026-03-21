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

class Http2Server_base;
class Handler_base;
class SecureContext_base;
class Http2Session_base;
class Http2Stream_base;
class http2_constants_base;

class http2_base : public object_base {
    DECLARE_CLASS(http2_base);

public:
    // http2_base
    static result_t createServer(v8::Local<v8::Object> options, Handler_base* hdlr, obj_ptr<Http2Server_base>& retVal);
    static result_t createServer(SecureContext_base* context, Handler_base* hdlr, obj_ptr<Http2Server_base>& retVal);
    static result_t connect(exlib::string authority, v8::Local<v8::Object> options, obj_ptr<Http2Session_base>& retVal, AsyncEvent* ac);
    static result_t getDefaultSettings(v8::Local<v8::Object>& retVal);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        ThrowTypeError("not a constructor");
    }

    static result_t load(v8::Local<v8::Value> v, obj_ptr<http2_base>& retVal)
    { return CALL_E_TYPEMISMATCH; }

public:
    static void s_static_createServer(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_connect(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_getDefaultSettings(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
    ASYNC_STATICVALUE3(http2_base, connect, exlib::string, v8::Local<v8::Object>, obj_ptr<Http2Session_base>);
};
}

#include "ifs/Http2Server.h"
#include "ifs/Handler.h"
#include "ifs/SecureContext.h"
#include "ifs/Http2Session.h"
#include "ifs/Http2Stream.h"
#include "ifs/http2_constants.h"

namespace fibjs {
inline ClassInfo& http2_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "createServer", s_static_createServer, true, ClassData::ASYNC_SYNC },
        { "connect", s_static_connect, true, ClassData::ASYNC_ASYNC },
        { "getDefaultSettings", s_static_getDefaultSettings, true, ClassData::ASYNC_SYNC }
    };

    static ClassData::ClassObject s_object[] = {
        { "Server", Http2Server_base::class_info },
        { "Http2Stream", Http2Stream_base::class_info },
        { "Http2Session", Http2Session_base::class_info },
        { "constants", http2_constants_base::class_info }
    };

    static ClassData s_cd = {
        "http2", true, s__new, NULL,
        ARRAYSIZE(s_method), s_method, ARRAYSIZE(s_object), s_object, 0, NULL, 0, NULL, NULL, NULL,
        &object_base::class_info(),
        true
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void http2_base::s_static_createServer(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Http2Server_base> vr;

    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(v8::Local<v8::Object>, 0);
    ARG(obj_ptr<Handler_base>, 1);

    hr = createServer(v0, v1.get(), vr);

    METHOD_OVER(2, 2);

    ARG(obj_ptr<SecureContext_base>, 0);
    ARG(obj_ptr<Handler_base>, 1);

    hr = createServer(v0.get(), v1.get(), vr);

    METHOD_RETURN();
}

inline void http2_base::s_static_connect(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Http2Session_base> vr;

    ASYNC_METHOD_ENTER("http2.connect");

    METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate->m_isolate));

    if (!cb.IsEmpty())
        hr = acb_connect(v0, v1, cb, args);
    else
        hr = ac_connect(v0, v1, vr);

    METHOD_RETURN();
}

inline void http2_base::s_static_getDefaultSettings(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Object> vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = getDefaultSettings(vr);

    METHOD_RETURN();
}
}
