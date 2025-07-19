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

class Blob_base : public object_base {
    DECLARE_CLASS(Blob_base);

public:
    // Blob_base
    static result_t _new(v8::Local<v8::Array> blobParts, v8::Local<v8::Object> options, obj_ptr<Blob_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    static result_t _new(Buffer_base* blobData, v8::Local<v8::Object> options, obj_ptr<Blob_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    virtual result_t get_type(exlib::string& retVal) = 0;
    virtual result_t get_size(int32_t& retVal) = 0;
    virtual result_t slice(int32_t start, int32_t end, exlib::string contentType, obj_ptr<Blob_base>& retVal) = 0;
    virtual result_t text(exlib::string& retVal, AsyncEvent* ac) = 0;
    virtual result_t arrayBuffer(v8::Local<v8::ArrayBuffer>& retVal, AsyncEvent* ac) = 0;

public:
    static void __new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static result_t load(Isolate* isolate, v8::Local<v8::Value> v, obj_ptr<Blob_base>& retVal);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_type(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_size(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_slice(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_text(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_arrayBuffer(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
    ASYNC_MEMBERVALUE1(Blob_base, text, exlib::string);
    ASYNC_MEMBERVALUE1(Blob_base, arrayBuffer, v8::Local<v8::ArrayBuffer>);
};
}

#include "ifs/Buffer.h"

namespace fibjs {
inline ClassInfo& Blob_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "slice", s_slice, false, ClassData::ASYNC_SYNC },
        { "text", s_text, false, ClassData::ASYNC_PROMISE },
        { "arrayBuffer", s_arrayBuffer, false, ClassData::ASYNC_PROMISE }
    };

    static ClassData::ClassProperty s_property[] = {
        { "type", s_get_type, block_set, false },
        { "size", s_get_size, block_set, false }
    };

    static ClassData s_cd = {
        "Blob", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &object_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void Blob_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    CONSTRUCT_INIT();
    __new(args);
}

inline void Blob_base::__new(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Blob_base> vr;

    CONSTRUCT_ENTER();

    METHOD_OVER(2, 0);

    OPT_ARG(v8::Local<v8::Array>, 0, v8::Array::New(isolate->m_isolate));
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate->m_isolate));

    hr = _new(v0, v1, vr, args.This());

    METHOD_OVER(2, 1);

    ARG(obj_ptr<Buffer_base>, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate->m_isolate));

    hr = _new(v0.get(), v1, vr, args.This());

    CONSTRUCT_RETURN();
}

inline result_t Blob_base::load(Isolate* isolate, v8::Local<v8::Value> v, obj_ptr<Blob_base>& retVal)
{
    obj_ptr<Blob_base> vr;

    LOAD_ENTER();

    METHOD_OVER(2, 0);

    OPT_ARG(v8::Local<v8::Array>, 0, v8::Array::New(isolate->m_isolate));
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate->m_isolate));

    hr = _new(v0, v1, vr, args.This());

    METHOD_OVER(2, 1);

    ARG(obj_ptr<Buffer_base>, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate->m_isolate));

    hr = _new(v0.get(), v1, vr, args.This());

    LOAD_RETURN();
}

inline void Blob_base::s_get_type(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(Blob_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_type(vr);

    METHOD_RETURN();
}

inline void Blob_base::s_get_size(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(Blob_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_size(vr);

    METHOD_RETURN();
}

inline void Blob_base::s_slice(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Blob_base> vr;

    METHOD_INSTANCE(Blob_base);
    METHOD_ENTER();

    METHOD_OVER(3, 0);

    OPT_ARG(int32_t, 0, 0);
    OPT_ARG(int32_t, 1, -1);
    OPT_ARG(exlib::string, 2, "");

    hr = pInst->slice(v0, v1, v2, vr);

    METHOD_RETURN();
}

inline void Blob_base::s_text(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    ASYNC_METHOD_INSTANCE(Blob_base);
    ASYNC_METHOD_ENTER("Blob.text");

    METHOD_OVER(0, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_text(cb, args);
    else
        hr = pInst->ac_text(vr);

    METHOD_RETURN();
}

inline void Blob_base::s_arrayBuffer(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::ArrayBuffer> vr;

    ASYNC_METHOD_INSTANCE(Blob_base);
    ASYNC_METHOD_ENTER("Blob.arrayBuffer");

    METHOD_OVER(0, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_arrayBuffer(cb, args);
    else
        hr = pInst->ac_arrayBuffer(vr);

    METHOD_RETURN();
}
}
