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
#include "ifs/HttpCollection.h"

namespace fibjs {

class HttpCollection_base;
class Buffer_base;
class Blob_base;

class FormData_base : public HttpCollection_base {
    DECLARE_CLASS(FormData_base);

public:
    using HttpCollection_base::append;
    using HttpCollection_base::set;

public:
    // FormData_base
    static result_t _new(obj_ptr<FormData_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    static result_t _new(exlib::string init, obj_ptr<FormData_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    static result_t _new(Buffer_base* init, exlib::string boundary, obj_ptr<FormData_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    static result_t _new(Blob_base* init, exlib::string boundary, obj_ptr<FormData_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    static result_t _new(v8::Local<v8::Object> init, obj_ptr<FormData_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    static result_t _new(FormData_base* init, obj_ptr<FormData_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    virtual result_t append(exlib::string name, Blob_base* value) = 0;
    virtual result_t append(exlib::string name, Blob_base* value, exlib::string filename) = 0;
    virtual result_t set(exlib::string name, Blob_base* value) = 0;
    virtual result_t set(exlib::string name, Blob_base* value, exlib::string filename) = 0;
    virtual result_t encode(exlib::string type, obj_ptr<Blob_base>& retVal) = 0;

public:
    static void __new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static result_t load(Isolate* isolate, v8::Local<v8::Value> v, obj_ptr<FormData_base>& retVal);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_append(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_encode(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

#include "ifs/Buffer.h"
#include "ifs/Blob.h"

namespace fibjs {
inline ClassInfo& FormData_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "append", s_append, false, ClassData::ASYNC_SYNC },
        { "set", s_set, false, ClassData::ASYNC_SYNC },
        { "encode", s_encode, false, ClassData::ASYNC_SYNC }
    };

    static ClassData s_cd = {
        "FormData", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, 0, NULL, 0, NULL, NULL, NULL,
        &HttpCollection_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void FormData_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    CONSTRUCT_INIT();
    __new(args);
}

inline void FormData_base::__new(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<FormData_base> vr;

    CONSTRUCT_ENTER();

    METHOD_OVER(0, 0);

    hr = _new(vr, args.This());

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = _new(v0, vr, args.This());

    METHOD_OVER(2, 2);

    ARG(obj_ptr<Buffer_base>, 0);
    ARG(exlib::string, 1);

    hr = _new(v0.get(), v1, vr, args.This());

    METHOD_OVER(2, 1);

    ARG(obj_ptr<Blob_base>, 0);
    OPT_ARG(exlib::string, 1, "");

    hr = _new(v0.get(), v1, vr, args.This());

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Object>, 0);

    hr = _new(v0, vr, args.This());

    METHOD_OVER(1, 1);

    STRICT_ARG(obj_ptr<FormData_base>, 0);

    hr = _new(v0.get(), vr, args.This());

    CONSTRUCT_RETURN();
}

inline result_t FormData_base::load(Isolate* isolate, v8::Local<v8::Value> v, obj_ptr<FormData_base>& retVal)
{
    obj_ptr<FormData_base> vr;

    LOAD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = _new(v0, vr, args.This());

    METHOD_OVER(2, 1);

    ARG(obj_ptr<Blob_base>, 0);
    OPT_ARG(exlib::string, 1, "");

    hr = _new(v0.get(), v1, vr, args.This());

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Object>, 0);

    hr = _new(v0, vr, args.This());

    METHOD_OVER(1, 1);

    STRICT_ARG(obj_ptr<FormData_base>, 0);

    hr = _new(v0.get(), vr, args.This());

    LOAD_RETURN();
}

inline void FormData_base::s_append(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(FormData_base);
    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(obj_ptr<Blob_base>, 1);

    hr = pInst->append(v0, v1.get());

    METHOD_OVER(3, 3);

    ARG(exlib::string, 0);
    ARG(obj_ptr<Blob_base>, 1);
    ARG(exlib::string, 2);

    hr = pInst->append(v0, v1.get(), v2);

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Object>, 0);

    hr = pInst->append(v0);

    METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(v8::Local<v8::Array>, 1);

    hr = pInst->append(v0, v1);

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Array>, 0);

    hr = pInst->append(v0);

    METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(Variant, 1);

    hr = pInst->append(v0, v1);

    METHOD_VOID();
}

inline void FormData_base::s_set(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(FormData_base);
    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(obj_ptr<Blob_base>, 1);

    hr = pInst->set(v0, v1.get());

    METHOD_OVER(3, 3);

    ARG(exlib::string, 0);
    ARG(obj_ptr<Blob_base>, 1);
    ARG(exlib::string, 2);

    hr = pInst->set(v0, v1.get(), v2);

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Object>, 0);

    hr = pInst->set(v0);

    METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(v8::Local<v8::Array>, 1);

    hr = pInst->set(v0, v1);

    METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(Variant, 1);

    hr = pInst->set(v0, v1);

    METHOD_VOID();
}

inline void FormData_base::s_encode(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Blob_base> vr;

    METHOD_INSTANCE(FormData_base);
    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(exlib::string, 0, "application/x-www-form-urlencoded");

    hr = pInst->encode(v0, vr);

    METHOD_RETURN();
}
}
