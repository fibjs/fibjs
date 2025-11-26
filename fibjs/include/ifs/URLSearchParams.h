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
#include "ifs/HttpCollection.h"

namespace fibjs {

class HttpCollection_base;

class URLSearchParams_base : public HttpCollection_base {
    DECLARE_CLASS(URLSearchParams_base);

public:
    // URLSearchParams_base
    static result_t _new(obj_ptr<URLSearchParams_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    static result_t _new(exlib::string init, obj_ptr<URLSearchParams_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    static result_t _new(v8::Local<v8::Object> init, obj_ptr<URLSearchParams_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    static result_t _new(v8::Local<v8::Array> init, obj_ptr<URLSearchParams_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    static result_t _new(URLSearchParams_base* init, obj_ptr<URLSearchParams_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());

public:
    static void __new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static result_t load(v8::Local<v8::Value> v, obj_ptr<URLSearchParams_base>& retVal);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

namespace fibjs {
inline ClassInfo& URLSearchParams_base::class_info()
{
    static ClassData s_cd = {
        "URLSearchParams", false, s__new, NULL,
        0, NULL, 0, NULL, 0, NULL, 0, NULL, NULL, NULL,
        &HttpCollection_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void URLSearchParams_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    CONSTRUCT_INIT();
    __new(args);
}

inline void URLSearchParams_base::__new(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<URLSearchParams_base> vr;

    CONSTRUCT_ENTER();

    METHOD_OVER(0, 0);

    hr = _new(vr, args.This());

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = _new(v0, vr, args.This());

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Object>, 0);

    hr = _new(v0, vr, args.This());

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Array>, 0);

    hr = _new(v0, vr, args.This());

    METHOD_OVER(1, 1);

    STRICT_ARG(obj_ptr<URLSearchParams_base>, 0);

    hr = _new(v0.get(), vr, args.This());

    CONSTRUCT_RETURN();
}

inline result_t URLSearchParams_base::load(v8::Local<v8::Value> v, obj_ptr<URLSearchParams_base>& retVal)
{
    obj_ptr<URLSearchParams_base> vr;

    LOAD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = _new(v0, vr, args.This());

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Object>, 0);

    hr = _new(v0, vr, args.This());

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Array>, 0);

    hr = _new(v0, vr, args.This());

    METHOD_OVER(1, 1);

    STRICT_ARG(obj_ptr<URLSearchParams_base>, 0);

    hr = _new(v0.get(), vr, args.This());

    LOAD_RETURN();
}
}
