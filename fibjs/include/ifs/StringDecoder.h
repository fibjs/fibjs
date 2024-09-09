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

class StringDecoder_base : public object_base {
    DECLARE_CLASS(StringDecoder_base);

public:
    // StringDecoder_base
    static result_t _new(exlib::string encoding, obj_ptr<StringDecoder_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    virtual result_t end(exlib::string& retVal) = 0;
    virtual result_t end(Buffer_base* buf, exlib::string& retVal) = 0;
    virtual result_t write(Buffer_base* buf, exlib::string& retVal) = 0;
    virtual result_t text(Buffer_base* buf, int32_t offset, exlib::string& retVal) = 0;
    virtual result_t fillLast(Buffer_base* buf, exlib::string& retVal) = 0;
    virtual result_t get_lastNeed(int32_t& retVal) = 0;
    virtual result_t set_lastNeed(int32_t newVal) = 0;
    virtual result_t get_lastTotal(int32_t& retVal) = 0;
    virtual result_t set_lastTotal(int32_t newVal) = 0;
    virtual result_t get_lastChar(obj_ptr<Buffer_base>& retVal) = 0;
    virtual result_t set_lastChar(Buffer_base* newVal) = 0;
    virtual result_t get_encoding(exlib::string& retVal) = 0;
    virtual result_t set_encoding(exlib::string newVal) = 0;

public:
    template <typename T>
    static void __new(const T& args);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_end(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_write(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_text(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_fillLast(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_lastNeed(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_lastNeed(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_lastTotal(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_lastTotal(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_lastChar(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_lastChar(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_encoding(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_encoding(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

#include "ifs/Buffer.h"

namespace fibjs {
inline ClassInfo& StringDecoder_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "end", s_end, false, ClassData::ASYNC_SYNC },
        { "write", s_write, false, ClassData::ASYNC_SYNC },
        { "text", s_text, false, ClassData::ASYNC_SYNC },
        { "fillLast", s_fillLast, false, ClassData::ASYNC_SYNC }
    };

    static ClassData::ClassProperty s_property[] = {
        { "lastNeed", s_get_lastNeed, s_set_lastNeed, false },
        { "lastTotal", s_get_lastTotal, s_set_lastTotal, false },
        { "lastChar", s_get_lastChar, s_set_lastChar, false },
        { "encoding", s_get_encoding, s_set_encoding, false }
    };

    static ClassData s_cd = {
        "StringDecoder", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &object_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void StringDecoder_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    CONSTRUCT_INIT();
    __new(args);
}

template <typename T>
void StringDecoder_base::__new(const T& args)
{
    obj_ptr<StringDecoder_base> vr;

    CONSTRUCT_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(exlib::string, 0, "utf8");

    hr = _new(v0, vr, args.This());

    CONSTRUCT_RETURN();
}

inline void StringDecoder_base::s_end(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(StringDecoder_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->end(vr);

    METHOD_OVER(1, 1);

    ARG(obj_ptr<Buffer_base>, 0);

    hr = pInst->end(v0, vr);

    METHOD_RETURN();
}

inline void StringDecoder_base::s_write(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(StringDecoder_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(obj_ptr<Buffer_base>, 0);

    hr = pInst->write(v0, vr);

    METHOD_RETURN();
}

inline void StringDecoder_base::s_text(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(StringDecoder_base);
    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(obj_ptr<Buffer_base>, 0);
    ARG(int32_t, 1);

    hr = pInst->text(v0, v1, vr);

    METHOD_RETURN();
}

inline void StringDecoder_base::s_fillLast(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(StringDecoder_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(obj_ptr<Buffer_base>, 0);

    hr = pInst->fillLast(v0, vr);

    METHOD_RETURN();
}

inline void StringDecoder_base::s_get_lastNeed(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(StringDecoder_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_lastNeed(vr);

    METHOD_RETURN();
}

inline void StringDecoder_base::s_set_lastNeed(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(StringDecoder_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(int32_t, 0);

    hr = pInst->set_lastNeed(v0);

    METHOD_VOID();
}

inline void StringDecoder_base::s_get_lastTotal(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(StringDecoder_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_lastTotal(vr);

    METHOD_RETURN();
}

inline void StringDecoder_base::s_set_lastTotal(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(StringDecoder_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(int32_t, 0);

    hr = pInst->set_lastTotal(v0);

    METHOD_VOID();
}

inline void StringDecoder_base::s_get_lastChar(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    METHOD_INSTANCE(StringDecoder_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_lastChar(vr);

    METHOD_RETURN();
}

inline void StringDecoder_base::s_set_lastChar(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(StringDecoder_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(obj_ptr<Buffer_base>, 0);

    hr = pInst->set_lastChar(v0);

    METHOD_VOID();
}

inline void StringDecoder_base::s_get_encoding(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(StringDecoder_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_encoding(vr);

    METHOD_RETURN();
}

inline void StringDecoder_base::s_set_encoding(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(StringDecoder_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = pInst->set_encoding(v0);

    METHOD_VOID();
}
}
