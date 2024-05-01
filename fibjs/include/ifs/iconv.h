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

class iconv_base : public object_base {
    DECLARE_CLASS(iconv_base);

public:
    // iconv_base
    static result_t encode(exlib::string charset, exlib::string data, obj_ptr<Buffer_base>& retVal);
    static result_t decode(exlib::string charset, Buffer_base* data, exlib::string& retVal);
    static result_t isEncoding(exlib::string charset, bool& retVal);

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
    static void s_static_isEncoding(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

#include "ifs/Buffer.h"

namespace fibjs {
inline ClassInfo& iconv_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "encode", s_static_encode, true, ClassData::ASYNC_SYNC },
        { "decode", s_static_decode, true, ClassData::ASYNC_SYNC },
        { "isEncoding", s_static_isEncoding, true, ClassData::ASYNC_SYNC }
    };

    static ClassData s_cd = {
        "iconv", true, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, 0, NULL, 0, NULL, NULL, NULL,
        &object_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void iconv_base::s_static_encode(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(exlib::string, 1);

    hr = encode(v0, v1, vr);

    METHOD_RETURN();
}

inline void iconv_base::s_static_decode(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(obj_ptr<Buffer_base>, 1);

    hr = decode(v0, v1, vr);

    METHOD_RETURN();
}

inline void iconv_base::s_static_isEncoding(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = isEncoding(v0, vr);

    METHOD_RETURN();
}
}
