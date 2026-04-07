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

class Deflate_base : public ZlibCodec_base {
    DECLARE_CLASS(Deflate_base);

public:
    // Deflate_base
    static result_t _new(v8::Local<v8::Object> opts, obj_ptr<Deflate_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());

public:
    static void __new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static result_t load(v8::Local<v8::Value> v, obj_ptr<Deflate_base>& retVal);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

namespace fibjs {
inline ClassInfo& Deflate_base::class_info()
{
    static ClassData s_cd = {
        "Deflate", false, s__new, NULL,
        0, NULL, 0, NULL, 0, NULL, 0, NULL, NULL, NULL,
        &ZlibCodec_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void Deflate_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    CONSTRUCT_INIT();
    __new(args);
}

inline void Deflate_base::__new(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Deflate_base> vr;

    CONSTRUCT_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(v8::Local<v8::Object>, 0, v8::Object::New(isolate->m_isolate));

    hr = _new(v0, vr, args.This());

    CONSTRUCT_RETURN();
}

inline result_t Deflate_base::load(v8::Local<v8::Value> v, obj_ptr<Deflate_base>& retVal)
{
    obj_ptr<Deflate_base> vr;

    LOAD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(v8::Local<v8::Object>, 0, v8::Object::New(isolate->m_isolate));

    hr = _new(v0, vr, args.This());

    LOAD_RETURN();
}
}
