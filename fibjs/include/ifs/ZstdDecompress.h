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

class ZstdDecompress_base : public ZlibCodec_base {
    DECLARE_CLASS(ZstdDecompress_base);

public:
    // ZstdDecompress_base
    static result_t _new(v8::Local<v8::Object> opts, obj_ptr<ZstdDecompress_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());

public:
    static void __new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static result_t load(v8::Local<v8::Value> v, obj_ptr<ZstdDecompress_base>& retVal);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

namespace fibjs {
inline ClassInfo& ZstdDecompress_base::class_info()
{
    static ClassData s_cd = {
        "ZstdDecompress", false, s__new, NULL,
        0, NULL, 0, NULL, 0, NULL, 0, NULL, NULL, NULL,
        &ZlibCodec_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void ZstdDecompress_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    CONSTRUCT_INIT();
    __new(args);
}

inline void ZstdDecompress_base::__new(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<ZstdDecompress_base> vr;

    CONSTRUCT_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG_NAMED(v8::Local<v8::Object>, 0, v8::Object::New(isolate->m_isolate), "opts");

    hr = _new(v0, vr, args.This());

    CONSTRUCT_RETURN();
}

inline result_t ZstdDecompress_base::load(v8::Local<v8::Value> v, obj_ptr<ZstdDecompress_base>& retVal)
{
    obj_ptr<ZstdDecompress_base> vr;

    LOAD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG_NAMED(v8::Local<v8::Object>, 0, v8::Object::New(isolate->m_isolate), "opts");

    hr = _new(v0, vr, args.This());

    LOAD_RETURN();
}
}
