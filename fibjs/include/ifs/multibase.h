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

class multibase_base : public object_base {
    DECLARE_CLASS(multibase_base);

public:
    using Union_encode_data = std::variant<obj_ptr<Buffer_base>, exlib::string>;

public:
    // multibase_base
    static result_t encode(Union_encode_data data, exlib::string codec, exlib::string& retVal);
    static result_t decode(exlib::string data, obj_ptr<Buffer_base>& retVal);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        ThrowTypeError("not a constructor");
    }

    static result_t load(v8::Local<v8::Value> v, obj_ptr<multibase_base>& retVal)
    { return CALL_E_TYPEMISMATCH; }

public:
    static void s_static_encode(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_decode(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

#include "ifs/Buffer.h"

namespace fibjs {
inline ClassInfo& multibase_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "encode", s_static_encode, true, ClassData::ASYNC_SYNC },
        { "decode", s_static_decode, true, ClassData::ASYNC_SYNC }
    };

    static ClassData s_cd = {
        "multibase", true, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, 0, NULL, 0, NULL, NULL, NULL,
        &object_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void multibase_base::s_static_encode(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(Union_encode_data, 0);
    ARG(exlib::string, 1);

    hr = encode(v0, v1, vr);

    METHOD_RETURN();
}

inline void multibase_base::s_static_decode(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = decode(v0, vr);

    METHOD_RETURN();
}
}
