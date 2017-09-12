/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _StringDecoder_base_H_
#define _StringDecoder_base_H_

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
    static void s_get_lastNeed(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_set_lastNeed(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_get_lastTotal(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_set_lastTotal(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_get_lastChar(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_set_lastChar(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_get_encoding(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_set_encoding(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
};
}

#include "Buffer.h"

namespace fibjs {
inline ClassInfo& StringDecoder_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "end", s_end, false },
        { "write", s_write, false },
        { "text", s_text, false },
        { "fillLast", s_fillLast, false }
    };

    static ClassData::ClassProperty s_property[] = {
        { "lastNeed", s_get_lastNeed, s_set_lastNeed, false },
        { "lastTotal", s_get_lastTotal, s_set_lastTotal, false },
        { "lastChar", s_get_lastChar, s_set_lastChar, false },
        { "encoding", s_get_encoding, s_set_encoding, false }
    };

    static ClassData s_cd = {
        "StringDecoder", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, NULL, NULL,
        &object_base::class_info()
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

inline void StringDecoder_base::s_get_lastNeed(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(StringDecoder_base);
    PROPERTY_ENTER();

    hr = pInst->get_lastNeed(vr);

    METHOD_RETURN();
}

inline void StringDecoder_base::s_set_lastNeed(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_INSTANCE(StringDecoder_base);
    PROPERTY_ENTER();
    PROPERTY_VAL(int32_t);

    hr = pInst->set_lastNeed(v0);

    PROPERTY_SET_LEAVE();
}

inline void StringDecoder_base::s_get_lastTotal(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(StringDecoder_base);
    PROPERTY_ENTER();

    hr = pInst->get_lastTotal(vr);

    METHOD_RETURN();
}

inline void StringDecoder_base::s_set_lastTotal(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_INSTANCE(StringDecoder_base);
    PROPERTY_ENTER();
    PROPERTY_VAL(int32_t);

    hr = pInst->set_lastTotal(v0);

    PROPERTY_SET_LEAVE();
}

inline void StringDecoder_base::s_get_lastChar(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    METHOD_INSTANCE(StringDecoder_base);
    PROPERTY_ENTER();

    hr = pInst->get_lastChar(vr);

    METHOD_RETURN();
}

inline void StringDecoder_base::s_set_lastChar(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_INSTANCE(StringDecoder_base);
    PROPERTY_ENTER();
    PROPERTY_VAL(obj_ptr<Buffer_base>);

    hr = pInst->set_lastChar(v0);

    PROPERTY_SET_LEAVE();
}

inline void StringDecoder_base::s_get_encoding(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(StringDecoder_base);
    PROPERTY_ENTER();

    hr = pInst->get_encoding(vr);

    METHOD_RETURN();
}

inline void StringDecoder_base::s_set_encoding(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_INSTANCE(StringDecoder_base);
    PROPERTY_ENTER();
    PROPERTY_VAL(exlib::string);

    hr = pInst->set_encoding(v0);

    PROPERTY_SET_LEAVE();
}
}

#endif
