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

class Headers_base : public HttpCollection_base {
    DECLARE_CLASS(Headers_base);

public:
    // Headers_base
    static result_t _new(obj_ptr<Headers_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    static result_t _new(v8::Local<v8::Object> init, obj_ptr<Headers_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    static result_t _new(v8::Local<v8::Array> init, obj_ptr<Headers_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    static result_t _new(Headers_base* init, obj_ptr<Headers_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    virtual result_t getSetCookie(obj_ptr<NArray>& retVal) = 0;

public:
    static void __new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static result_t load(v8::Local<v8::Value> v, obj_ptr<Headers_base>& retVal);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_getSetCookie(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

namespace fibjs {
inline ClassInfo& Headers_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "getSetCookie", s_getSetCookie, false, ClassData::ASYNC_SYNC }
    };

    static ClassData s_cd = {
        "Headers", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, 0, NULL, 0, NULL, NULL, NULL,
        &HttpCollection_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void Headers_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    CONSTRUCT_INIT();
    __new(args);
}

inline void Headers_base::__new(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Headers_base> vr;

    CONSTRUCT_ENTER();

    METHOD_OVER(0, 0);

    hr = _new(vr, args.This());

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Object>, 0);

    hr = _new(v0, vr, args.This());

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Array>, 0);

    hr = _new(v0, vr, args.This());

    METHOD_OVER(1, 1);

    STRICT_ARG(obj_ptr<Headers_base>, 0);

    hr = _new(v0.get(), vr, args.This());

    CONSTRUCT_RETURN();
}

inline result_t Headers_base::load(v8::Local<v8::Value> v, obj_ptr<Headers_base>& retVal)
{
    obj_ptr<Headers_base> vr;

    LOAD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Object>, 0);

    hr = _new(v0, vr, args.This());

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Array>, 0);

    hr = _new(v0, vr, args.This());

    METHOD_OVER(1, 1);

    STRICT_ARG(obj_ptr<Headers_base>, 0);

    hr = _new(v0.get(), vr, args.This());

    LOAD_RETURN();
}

inline void Headers_base::s_getSetCookie(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<NArray> vr;

    METHOD_INSTANCE(Headers_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->getSetCookie(vr);

    METHOD_RETURN();
}
}
