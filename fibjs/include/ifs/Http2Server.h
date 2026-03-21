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
#include "ifs/TcpServer.h"

namespace fibjs {

class TcpServer_base;
class SecureContext_base;
class Handler_base;

class Http2Server_base : public TcpServer_base {
    DECLARE_CLASS(Http2Server_base);

public:
    // Http2Server_base
    static result_t _new(SecureContext_base* context, Handler_base* hdlr, obj_ptr<Http2Server_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    static result_t _new(SecureContext_base* context, int32_t port, Handler_base* hdlr, obj_ptr<Http2Server_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    static result_t _new(SecureContext_base* context, exlib::string addr, int32_t port, Handler_base* hdlr, obj_ptr<Http2Server_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    static result_t _new(v8::Local<v8::Object> options, Handler_base* hdlr, obj_ptr<Http2Server_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    virtual result_t get_secureContext(obj_ptr<SecureContext_base>& retVal) = 0;
    virtual result_t setSecureContext(SecureContext_base* context) = 0;
    virtual result_t setSecureContext(v8::Local<v8::Object> options) = 0;

public:
    static void __new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static result_t load(v8::Local<v8::Value> v, obj_ptr<Http2Server_base>& retVal);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_secureContext(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_setSecureContext(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

#include "ifs/SecureContext.h"
#include "ifs/Handler.h"

namespace fibjs {
inline ClassInfo& Http2Server_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "setSecureContext", s_setSecureContext, false, ClassData::ASYNC_SYNC }
    };

    static ClassData::ClassProperty s_property[] = {
        { "secureContext", s_get_secureContext, block_set, false }
    };

    static ClassData s_cd = {
        "Http2Server", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &TcpServer_base::class_info(),
        true
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void Http2Server_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    CONSTRUCT_INIT();
    __new(args);
}

inline void Http2Server_base::__new(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Http2Server_base> vr;

    CONSTRUCT_ENTER();

    METHOD_OVER(2, 2);

    ARG(obj_ptr<SecureContext_base>, 0);
    ARG(obj_ptr<Handler_base>, 1);

    hr = _new(v0.get(), v1.get(), vr, args.This());

    METHOD_OVER(3, 3);

    ARG(obj_ptr<SecureContext_base>, 0);
    ARG(int32_t, 1);
    ARG(obj_ptr<Handler_base>, 2);

    hr = _new(v0.get(), v1, v2.get(), vr, args.This());

    METHOD_OVER(4, 4);

    ARG(obj_ptr<SecureContext_base>, 0);
    ARG(exlib::string, 1);
    ARG(int32_t, 2);
    ARG(obj_ptr<Handler_base>, 3);

    hr = _new(v0.get(), v1, v2, v3.get(), vr, args.This());

    METHOD_OVER(2, 2);

    ARG(v8::Local<v8::Object>, 0);
    ARG(obj_ptr<Handler_base>, 1);

    hr = _new(v0, v1.get(), vr, args.This());

    CONSTRUCT_RETURN();
}

inline result_t Http2Server_base::load(v8::Local<v8::Value> v, obj_ptr<Http2Server_base>& retVal)
{
    obj_ptr<Http2Server_base> vr;

    LOAD_ENTER();

    LOAD_RETURN();
}

inline void Http2Server_base::s_get_secureContext(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<SecureContext_base> vr;

    METHOD_INSTANCE(Http2Server_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_secureContext(vr);

    METHOD_RETURN();
}

inline void Http2Server_base::s_setSecureContext(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(Http2Server_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(obj_ptr<SecureContext_base>, 0);

    hr = pInst->setSecureContext(v0.get());

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Object>, 0);

    hr = pInst->setSecureContext(v0);

    METHOD_VOID();
}
}
