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

class Script_base : public object_base {
    DECLARE_CLASS(Script_base);

public:
    // Script_base
    static result_t _new(exlib::string code, v8::Local<v8::Object> opts, obj_ptr<Script_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    virtual result_t runInContext(v8::Local<v8::Object> contextifiedObject, v8::Local<v8::Object> opts, v8::Local<v8::Value>& retVal) = 0;
    virtual result_t runInNewContext(v8::Local<v8::Object> contextObject, v8::Local<v8::Object> opts, v8::Local<v8::Value>& retVal) = 0;
    virtual result_t runInThisContext(v8::Local<v8::Object> opts, v8::Local<v8::Value>& retVal) = 0;
    virtual result_t createCachedData(obj_ptr<Buffer_base>& retVal) = 0;

public:
    template <typename T>
    static void __new(const T& args);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_runInContext(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_runInNewContext(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_runInThisContext(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_createCachedData(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

#include "ifs/Buffer.h"

namespace fibjs {
inline ClassInfo& Script_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "runInContext", s_runInContext, false, ClassData::ASYNC_SYNC },
        { "runInNewContext", s_runInNewContext, false, ClassData::ASYNC_SYNC },
        { "runInThisContext", s_runInThisContext, false, ClassData::ASYNC_SYNC },
        { "createCachedData", s_createCachedData, false, ClassData::ASYNC_SYNC }
    };

    static ClassData s_cd = {
        "Script", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, 0, NULL, 0, NULL, NULL, NULL,
        &object_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void Script_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    CONSTRUCT_INIT();
    __new(args);
}

template <typename T>
void Script_base::__new(const T& args)
{
    obj_ptr<Script_base> vr;

    CONSTRUCT_ENTER();

    METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate->m_isolate));

    hr = _new(v0, v1, vr, args.This());

    CONSTRUCT_RETURN();
}

inline void Script_base::s_runInContext(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Value> vr;

    METHOD_INSTANCE(Script_base);
    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(v8::Local<v8::Object>, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate->m_isolate));

    hr = pInst->runInContext(v0, v1, vr);

    METHOD_RETURN();
}

inline void Script_base::s_runInNewContext(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Value> vr;

    METHOD_INSTANCE(Script_base);
    METHOD_ENTER();

    METHOD_OVER(2, 0);

    OPT_ARG(v8::Local<v8::Object>, 0, v8::Object::New(isolate->m_isolate));
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate->m_isolate));

    hr = pInst->runInNewContext(v0, v1, vr);

    METHOD_RETURN();
}

inline void Script_base::s_runInThisContext(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Value> vr;

    METHOD_INSTANCE(Script_base);
    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(v8::Local<v8::Object>, 0, v8::Object::New(isolate->m_isolate));

    hr = pInst->runInThisContext(v0, vr);

    METHOD_RETURN();
}

inline void Script_base::s_createCachedData(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    METHOD_INSTANCE(Script_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->createCachedData(vr);

    METHOD_RETURN();
}
}
