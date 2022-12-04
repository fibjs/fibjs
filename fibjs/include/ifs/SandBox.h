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

class SandBox_base : public object_base {
    DECLARE_CLASS(SandBox_base);

public:
    // SandBox_base
    static result_t _new(v8::Local<v8::Object> mods, obj_ptr<SandBox_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    static result_t _new(v8::Local<v8::Object> mods, v8::Local<v8::Function> require, obj_ptr<SandBox_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    static result_t _new(v8::Local<v8::Object> mods, v8::Local<v8::Object> global, obj_ptr<SandBox_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    static result_t _new(v8::Local<v8::Object> mods, v8::Local<v8::Function> require, v8::Local<v8::Object> global, obj_ptr<SandBox_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    virtual result_t addBuiltinModules() = 0;
    virtual result_t add(exlib::string id, v8::Local<v8::Value> mod) = 0;
    virtual result_t add(v8::Local<v8::Object> mods) = 0;
    virtual result_t addScript(exlib::string srcname, Buffer_base* script, v8::Local<v8::Value>& retVal) = 0;
    virtual result_t remove(exlib::string id) = 0;
    virtual result_t has(exlib::string id, bool& retVal) = 0;
    virtual result_t clone(obj_ptr<SandBox_base>& retVal) = 0;
    virtual result_t freeze() = 0;
    virtual result_t refresh() = 0;
    virtual result_t run(exlib::string fname, v8::Local<v8::Array> argv) = 0;
    virtual result_t resolve(exlib::string id, exlib::string base, exlib::string& retVal) = 0;
    virtual result_t require(exlib::string id, exlib::string base, v8::Local<v8::Value>& retVal) = 0;
    virtual result_t setModuleCompiler(exlib::string extname, v8::Local<v8::Function> compiler) = 0;
    virtual result_t get_global(v8::Local<v8::Object>& retVal) = 0;
    virtual result_t get_modules(v8::Local<v8::Object>& retVal) = 0;

public:
    template <typename T>
    static void __new(const T& args);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_addBuiltinModules(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_add(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_addScript(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_remove(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_has(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_clone(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_freeze(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_refresh(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_run(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_resolve(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_require(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_setModuleCompiler(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_global(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_modules(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
};
}

#include "ifs/Buffer.h"

namespace fibjs {
inline ClassInfo& SandBox_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "addBuiltinModules", s_addBuiltinModules, false, false },
        { "add", s_add, false, false },
        { "addScript", s_addScript, false, false },
        { "remove", s_remove, false, false },
        { "has", s_has, false, false },
        { "clone", s_clone, false, false },
        { "freeze", s_freeze, false, false },
        { "refresh", s_refresh, false, false },
        { "run", s_run, false, false },
        { "resolve", s_resolve, false, false },
        { "require", s_require, false, false },
        { "setModuleCompiler", s_setModuleCompiler, false, false }
    };

    static ClassData::ClassProperty s_property[] = {
        { "global", s_get_global, block_set, false },
        { "modules", s_get_modules, block_set, false }
    };

    static ClassData s_cd = {
        "SandBox", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
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

    METHOD_NAME("new SandBox()");
    CONSTRUCT_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(v8::Local<v8::Object>, 0, v8::Object::New(isolate));

    hr = _new(v0, vr, args.This());

    METHOD_OVER(2, 2);

    ARG(v8::Local<v8::Object>, 0);
    ARG(v8::Local<v8::Function>, 1);

    hr = _new(v0, v1, vr, args.This());

    METHOD_OVER(2, 2);

    ARG(v8::Local<v8::Object>, 0);
    ARG(v8::Local<v8::Object>, 1);

    hr = _new(v0, v1, vr, args.This());

    METHOD_OVER(3, 3);

    ARG(v8::Local<v8::Object>, 0);
    ARG(v8::Local<v8::Function>, 1);
    ARG(v8::Local<v8::Object>, 2);

    hr = _new(v0, v1, v2, vr, args.This());

    CONSTRUCT_RETURN();
}

inline void SandBox_base::s_addBuiltinModules(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("SandBox.addBuiltinModules");
    METHOD_INSTANCE(SandBox_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->addBuiltinModules();

    METHOD_VOID();
}

inline void SandBox_base::s_add(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("SandBox.add");
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

    METHOD_NAME("SandBox.addScript");
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
    METHOD_NAME("SandBox.remove");
    METHOD_INSTANCE(SandBox_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = pInst->remove(v0);

    METHOD_VOID();
}

inline void SandBox_base::s_has(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_NAME("SandBox.has");
    METHOD_INSTANCE(SandBox_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = pInst->has(v0, vr);

    METHOD_RETURN();
}

inline void SandBox_base::s_clone(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<SandBox_base> vr;

    METHOD_NAME("SandBox.clone");
    METHOD_INSTANCE(SandBox_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->clone(vr);

    METHOD_RETURN();
}

inline void SandBox_base::s_freeze(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("SandBox.freeze");
    METHOD_INSTANCE(SandBox_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->freeze();

    METHOD_VOID();
}

inline void SandBox_base::s_refresh(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("SandBox.refresh");
    METHOD_INSTANCE(SandBox_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->refresh();

    METHOD_VOID();
}

inline void SandBox_base::s_run(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("SandBox.run");
    METHOD_INSTANCE(SandBox_base);
    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(v8::Local<v8::Array>, 1, v8::Array::New(isolate));

    hr = pInst->run(v0, v1);

    METHOD_VOID();
}

inline void SandBox_base::s_resolve(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_NAME("SandBox.resolve");
    METHOD_INSTANCE(SandBox_base);
    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(exlib::string, 1);

    hr = pInst->resolve(v0, v1, vr);

    METHOD_RETURN();
}

inline void SandBox_base::s_require(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Value> vr;

    METHOD_NAME("SandBox.require");
    METHOD_INSTANCE(SandBox_base);
    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(exlib::string, 1);

    hr = pInst->require(v0, v1, vr);

    METHOD_RETURN();
}

inline void SandBox_base::s_setModuleCompiler(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("SandBox.setModuleCompiler");
    METHOD_INSTANCE(SandBox_base);
    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(v8::Local<v8::Function>, 1);

    hr = pInst->setModuleCompiler(v0, v1);

    METHOD_VOID();
}

inline void SandBox_base::s_get_global(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Object> vr;

    METHOD_NAME("SandBox.global");
    METHOD_INSTANCE(SandBox_base);
    PROPERTY_ENTER();

    hr = pInst->get_global(vr);

    METHOD_RETURN();
}

inline void SandBox_base::s_get_modules(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Object> vr;

    METHOD_NAME("SandBox.modules");
    METHOD_INSTANCE(SandBox_base);
    PROPERTY_ENTER();

    hr = pInst->get_modules(vr);

    METHOD_RETURN();
}
}
