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

class Buffer_base;

class msgpack_base : public object_base {
    DECLARE_CLASS(msgpack_base);

public:
    // msgpack_base
    static result_t encode(v8::Local<v8::Value> data, obj_ptr<Buffer_base>& retVal);
    static result_t decode(Buffer_base* data, v8::Local<v8::Value>& retVal);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_static_encode(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_decode(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

#include "ifs/Buffer.h"

namespace fibjs {
inline ClassInfo& msgpack_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "encode", s_static_encode, true, false },
        { "decode", s_static_decode, true, false }
    };

    static ClassData s_cd = {
        "msgpack", true, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, 0, NULL, 0, NULL, NULL, NULL,
        &object_base::class_info()
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void msgpack_base::s_static_encode(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    METHOD_NAME("msgpack.encode");
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = encode(v0, vr);

    METHOD_RETURN();
}

inline void msgpack_base::s_static_decode(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Value> vr;

    METHOD_NAME("msgpack.decode");
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(obj_ptr<Buffer_base>, 0);

    hr = decode(v0, vr);

    METHOD_RETURN();
}
}
