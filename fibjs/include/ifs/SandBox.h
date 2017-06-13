/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _SandBox_base_H_
#define _SandBox_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs {

class Buffer_base;

class SandBox_base : public object_base {
    DECLARE_CLASS(SandBox_base);

public:
    // SandBox_base
    static result_t _new(v8::Local<v8::Object> mods, obj_ptr<SandBox_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    static result_t _new(v8::Local<v8::Object> mods, v8::Local<v8::Function> require, obj_ptr<SandBox_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    virtual result_t add(exlib::string id, v8::Local<v8::Value> mod) = 0;
    virtual result_t add(v8::Local<v8::Object> mods) = 0;
    virtual result_t addScript(exlib::string srcname, Buffer_base* script, v8::Local<v8::Value>& retVal) = 0;
    virtual result_t remove(exlib::string id) = 0;
    virtual result_t clone(obj_ptr<SandBox_base>& retVal) = 0;
    virtual result_t run(exlib::string fname, v8::Local<v8::Array> argv) = 0;
    virtual result_t require(exlib::string id, exlib::string base, v8::Local<v8::Value>& retVal) = 0;

public:
    template <typename T>
    static void __new(const T& args);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_add(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_addScript(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_remove(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_clone(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_run(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_require(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

#include "Buffer.h"

namespace fibjs {
inline ClassInfo& SandBox_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "add", s_add, false },
        { "addScript", s_addScript, false },
        { "remove", s_remove, false },
        { "clone", s_clone, false },
        { "run", s_run, false },
        { "require", s_require, false }
    };

    static ClassData s_cd = {
        "SandBox", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, 0, NULL, NULL, NULL,
        &object_base::class_info()
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void SandBox_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    CONSTRUCT_INIT();
    __new(args);
}

template <typename T>
void SandBox_base::__new(const T& args)
{
    obj_ptr<SandBox_base> vr;

    CONSTRUCT_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Object>, 0);

    hr = _new(v0, vr, args.This());

    METHOD_OVER(2, 2);

    ARG(v8::Local<v8::Object>, 0);
    ARG(v8::Local<v8::Function>, 1);

    hr = _new(v0, v1, vr, args.This());

    CONSTRUCT_RETURN();
}

inline void SandBox_base::s_add(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(SandBox_base);
    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(v8::Local<v8::Value>, 1);

    hr = pInst->add(v0, v1);

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Object>, 0);

    hr = pInst->add(v0);

    METHOD_VOID();
}

inline void SandBox_base::s_addScript(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Value> vr;

    METHOD_INSTANCE(SandBox_base);
    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(obj_ptr<Buffer_base>, 1);

    hr = pInst->addScript(v0, v1, vr);

    METHOD_RETURN();
}

inline void SandBox_base::s_remove(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(SandBox_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = pInst->remove(v0);

    METHOD_VOID();
}

inline void SandBox_base::s_clone(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<SandBox_base> vr;

    METHOD_INSTANCE(SandBox_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->clone(vr);

    METHOD_RETURN();
}

inline void SandBox_base::s_run(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(SandBox_base);
    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(v8::Local<v8::Array>, 1, v8::Array::New(isolate));

    hr = pInst->run(v0, v1);

    METHOD_VOID();
}

inline void SandBox_base::s_require(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Value> vr;

    METHOD_INSTANCE(SandBox_base);
    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(exlib::string, 1, "");

    hr = pInst->require(v0, v1, vr);

    METHOD_RETURN();
}
}

#endif
