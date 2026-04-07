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
#include "ifs/ZlibCodec.h"

namespace fibjs {

class ZlibCodec_base;

class Gunzip_base : public ZlibCodec_base {
    DECLARE_CLASS(Gunzip_base);

public:
    // Gunzip_base
    static result_t _new(v8::Local<v8::Object> opts, obj_ptr<Gunzip_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());

public:
    static void __new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static result_t load(v8::Local<v8::Value> v, obj_ptr<Gunzip_base>& retVal);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

namespace fibjs {
inline ClassInfo& Gunzip_base::class_info()
{
    static ClassData s_cd = {
        "Gunzip", false, s__new, NULL,
        0, NULL, 0, NULL, 0, NULL, 0, NULL, NULL, NULL,
        &ZlibCodec_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void Gunzip_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    CONSTRUCT_INIT();
    __new(args);
}

inline void Gunzip_base::__new(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Gunzip_base> vr;

    CONSTRUCT_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(v8::Local<v8::Object>, 0, v8::Object::New(isolate->m_isolate));

    hr = _new(v0, vr, args.This());

    CONSTRUCT_RETURN();
}

inline result_t Gunzip_base::load(v8::Local<v8::Value> v, obj_ptr<Gunzip_base>& retVal)
{
    obj_ptr<Gunzip_base> vr;

    LOAD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(v8::Local<v8::Object>, 0, v8::Object::New(isolate->m_isolate));

    hr = _new(v0, vr, args.This());

    LOAD_RETURN();
}
}
