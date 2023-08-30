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

class punycode_base : public object_base {
    DECLARE_CLASS(punycode_base);

public:
    // punycode_base
    static result_t encode(exlib::string domain, exlib::string& retVal);
    static result_t decode(exlib::string domain, exlib::string& retVal);
    static result_t toASCII(exlib::string domain, exlib::string& retVal);
    static result_t toUnicode(exlib::string domain, exlib::string& retVal);

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
    static void s_static_toASCII(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_toUnicode(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

namespace fibjs {
inline ClassInfo& punycode_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "encode", s_static_encode, true, false },
        { "decode", s_static_decode, true, false },
        { "toASCII", s_static_toASCII, true, false },
        { "toUnicode", s_static_toUnicode, true, false }
    };

    static ClassData s_cd = {
        "punycode", true, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, 0, NULL, 0, NULL, NULL, NULL,
        &object_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void punycode_base::s_static_encode(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = encode(v0, vr);

    METHOD_RETURN();
}

inline void punycode_base::s_static_decode(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = decode(v0, vr);

    METHOD_RETURN();
}

inline void punycode_base::s_static_toASCII(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = toASCII(v0, vr);

    METHOD_RETURN();
}

inline void punycode_base::s_static_toUnicode(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = toUnicode(v0, vr);

    METHOD_RETURN();
}
}
