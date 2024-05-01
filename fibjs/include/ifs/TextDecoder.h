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

class TextDecoder_base : public object_base {
    DECLARE_CLASS(TextDecoder_base);

public:
    // TextDecoder_base
    static result_t _new(exlib::string codec, v8::Local<v8::Object> opts, obj_ptr<TextDecoder_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    virtual result_t decode(Buffer_base* data, v8::Local<v8::Object> opts, exlib::string& retVal) = 0;
    virtual result_t decode(exlib::string& retVal) = 0;
    virtual result_t get_encoding(exlib::string& retVal) = 0;

public:
    template <typename T>
    static void __new(const T& args);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_decode(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_encoding(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
};
}

#include "ifs/Buffer.h"

namespace fibjs {
inline ClassInfo& TextDecoder_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "decode", s_decode, false, ClassData::ASYNC_SYNC }
    };

    static ClassData::ClassProperty s_property[] = {
        { "encoding", s_get_encoding, block_set, false }
    };

    static ClassData s_cd = {
        "TextDecoder", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &object_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void TextDecoder_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    CONSTRUCT_INIT();
    __new(args);
}

template <typename T>
void TextDecoder_base::__new(const T& args)
{
    obj_ptr<TextDecoder_base> vr;

    CONSTRUCT_ENTER();

    METHOD_OVER(2, 0);

    OPT_ARG(exlib::string, 0, "utf8");
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate->m_isolate));

    hr = _new(v0, v1, vr, args.This());

    CONSTRUCT_RETURN();
}

inline void TextDecoder_base::s_decode(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(TextDecoder_base);
    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(obj_ptr<Buffer_base>, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate->m_isolate));

    hr = pInst->decode(v0, v1, vr);

    METHOD_OVER(0, 0);

    hr = pInst->decode(vr);

    METHOD_RETURN();
}

inline void TextDecoder_base::s_get_encoding(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(TextDecoder_base);
    PROPERTY_ENTER();

    hr = pInst->get_encoding(vr);

    METHOD_RETURN();
}
}
