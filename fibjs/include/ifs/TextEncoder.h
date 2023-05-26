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

class TextEncoder_base : public object_base {
    DECLARE_CLASS(TextEncoder_base);

public:
    // TextEncoder_base
    static result_t _new(exlib::string codec, v8::Local<v8::Object> opts, obj_ptr<TextEncoder_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    virtual result_t encode(exlib::string data, v8::Local<v8::Object> opts, obj_ptr<Buffer_base>& retVal) = 0;
    virtual result_t get_encoding(exlib::string& retVal) = 0;

public:
    template <typename T>
    static void __new(const T& args);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_encode(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_encoding(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
};
}

#include "ifs/Buffer.h"

namespace fibjs {
inline ClassInfo& TextEncoder_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "encode", s_encode, false, false }
    };

    static ClassData::ClassProperty s_property[] = {
        { "encoding", s_get_encoding, block_set, false }
    };

    static ClassData s_cd = {
        "TextEncoder", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &object_base::class_info()
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void TextEncoder_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    CONSTRUCT_INIT();
    __new(args);
}

template <typename T>
void TextEncoder_base::__new(const T& args)
{
    obj_ptr<TextEncoder_base> vr;

    CONSTRUCT_ENTER();

    METHOD_OVER(2, 0);

    OPT_ARG(exlib::string, 0, "utf8");
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate->m_isolate));

    hr = _new(v0, v1, vr, args.This());

    CONSTRUCT_RETURN();
}

inline void TextEncoder_base::s_encode(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    METHOD_INSTANCE(TextEncoder_base);
    METHOD_ENTER();

    METHOD_OVER(2, 0);

    OPT_ARG(exlib::string, 0, "");
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate->m_isolate));

    hr = pInst->encode(v0, v1, vr);

    METHOD_RETURN();
}

inline void TextEncoder_base::s_get_encoding(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(TextEncoder_base);
    PROPERTY_ENTER();

    hr = pInst->get_encoding(vr);

    METHOD_RETURN();
}
}
