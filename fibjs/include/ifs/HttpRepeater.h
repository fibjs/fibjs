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
#include "ifs/Handler.h"

namespace fibjs {

class Handler_base;
class HttpClient_base;

class HttpRepeater_base : public Handler_base {
    DECLARE_CLASS(HttpRepeater_base);

public:
    // HttpRepeater_base
    static result_t _new(exlib::string url, obj_ptr<HttpRepeater_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    static result_t _new(v8::Local<v8::Array> urls, obj_ptr<HttpRepeater_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    virtual result_t load(v8::Local<v8::Array> urls) = 0;
    virtual result_t get_urls(obj_ptr<NArray>& retVal) = 0;
    virtual result_t get_client(obj_ptr<HttpClient_base>& retVal) = 0;

public:
    template <typename T>
    static void __new(const T& args);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_load(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_urls(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_client(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
};
}

#include "ifs/HttpClient.h"

namespace fibjs {
inline ClassInfo& HttpRepeater_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "load", s_load, false, ClassData::ASYNC_SYNC }
    };

    static ClassData::ClassProperty s_property[] = {
        { "urls", s_get_urls, block_set, false },
        { "client", s_get_client, block_set, false }
    };

    static ClassData s_cd = {
        "HttpRepeater", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &Handler_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void HttpRepeater_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    CONSTRUCT_INIT();
    __new(args);
}

template <typename T>
void HttpRepeater_base::__new(const T& args)
{
    obj_ptr<HttpRepeater_base> vr;

    CONSTRUCT_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = _new(v0, vr, args.This());

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Array>, 0);

    hr = _new(v0, vr, args.This());

    CONSTRUCT_RETURN();
}

inline void HttpRepeater_base::s_load(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(HttpRepeater_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Array>, 0);

    hr = pInst->load(v0);

    METHOD_VOID();
}

inline void HttpRepeater_base::s_get_urls(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    obj_ptr<NArray> vr;

    METHOD_INSTANCE(HttpRepeater_base);
    PROPERTY_ENTER();

    hr = pInst->get_urls(vr);

    METHOD_RETURN();
}

inline void HttpRepeater_base::s_get_client(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    obj_ptr<HttpClient_base> vr;

    METHOD_INSTANCE(HttpRepeater_base);
    PROPERTY_ENTER();

    hr = pInst->get_client(vr);

    METHOD_RETURN();
}
}
