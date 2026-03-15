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
#include "ifs/Blob.h"

namespace fibjs {

class Blob_base;
class Buffer_base;

class File_base : public Blob_base {
    DECLARE_CLASS(File_base);

public:
    // File_base
    static result_t _new(v8::Local<v8::Array> blobParts, exlib::string name, v8::Local<v8::Object> options, obj_ptr<File_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    static result_t _new(Buffer_base* blobData, exlib::string name, v8::Local<v8::Object> options, obj_ptr<File_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    static result_t _new(v8::Local<v8::Object> options, obj_ptr<File_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    virtual result_t get_name(exlib::string& retVal) = 0;
    virtual result_t get_lastModified(double& retVal) = 0;

public:
    static void __new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static result_t load(v8::Local<v8::Value> v, obj_ptr<File_base>& retVal);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_name(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_lastModified(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

#include "ifs/Buffer.h"

namespace fibjs {
inline ClassInfo& File_base::class_info()
{
    static ClassData::ClassProperty s_property[] = {
        { "name", s_get_name, block_set, false },
        { "lastModified", s_get_lastModified, block_set, false }
    };

    static ClassData s_cd = {
        "File", false, s__new, NULL,
        0, NULL, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &Blob_base::class_info(),
        true
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void File_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    CONSTRUCT_INIT();
    __new(args);
}

inline void File_base::__new(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<File_base> vr;

    CONSTRUCT_ENTER();

    METHOD_OVER(3, 2);

    ARG(v8::Local<v8::Array>, 0);
    ARG(exlib::string, 1);
    OPT_ARG(v8::Local<v8::Object>, 2, v8::Object::New(isolate->m_isolate));

    hr = _new(v0, v1, v2, vr, args.This());

    METHOD_OVER(3, 2);

    ARG(obj_ptr<Buffer_base>, 0);
    ARG(exlib::string, 1);
    OPT_ARG(v8::Local<v8::Object>, 2, v8::Object::New(isolate->m_isolate));

    hr = _new(v0.get(), v1, v2, vr, args.This());

    METHOD_OVER(1, 0);

    OPT_ARG(v8::Local<v8::Object>, 0, v8::Object::New(isolate->m_isolate));

    hr = _new(v0, vr, args.This());

    CONSTRUCT_RETURN();
}

inline result_t File_base::load(v8::Local<v8::Value> v, obj_ptr<File_base>& retVal)
{
    obj_ptr<File_base> vr;

    LOAD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(v8::Local<v8::Object>, 0, v8::Object::New(isolate->m_isolate));

    hr = _new(v0, vr, args.This());

    LOAD_RETURN();
}

inline void File_base::s_get_name(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(File_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_name(vr);

    METHOD_RETURN();
}

inline void File_base::s_get_lastModified(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    double vr;

    METHOD_INSTANCE(File_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_lastModified(vr);

    METHOD_RETURN();
}
}
