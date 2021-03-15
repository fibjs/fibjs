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
class console_base;
class process_base;
class Worker_base;
class Timer_base;

class global_base : public object_base {
    DECLARE_CLASS(global_base);

public:
    // global_base
    static result_t get_Master(obj_ptr<Worker_base>& retVal);
    static result_t get_global(v8::Local<v8::Object>& retVal);
    static result_t run(exlib::string fname, v8::Local<v8::Array> argv);
    static result_t require(exlib::string id, v8::Local<v8::Value>& retVal);
    static result_t get_argv(v8::Local<v8::Array>& retVal);
    static result_t get___filename(exlib::string& retVal);
    static result_t get___dirname(exlib::string& retVal);
    static result_t setTimeout(v8::Local<v8::Function> callback, double timeout, OptArgs args, obj_ptr<Timer_base>& retVal);
    static result_t clearTimeout(v8::Local<v8::Value> t);
    static result_t setInterval(v8::Local<v8::Function> callback, double timeout, OptArgs args, obj_ptr<Timer_base>& retVal);
    static result_t clearInterval(v8::Local<v8::Value> t);
    static result_t setHrInterval(v8::Local<v8::Function> callback, double timeout, OptArgs args, obj_ptr<Timer_base>& retVal);
    static result_t clearHrInterval(v8::Local<v8::Value> t);
    static result_t setImmediate(v8::Local<v8::Function> callback, OptArgs args, obj_ptr<Timer_base>& retVal);
    static result_t clearImmediate(v8::Local<v8::Value> t);
    static result_t GC();

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        Isolate* isolate = Isolate::current();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_static_get_Master(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_static_get_global(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_static_run(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_require(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_get_argv(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_static_get___filename(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_static_get___dirname(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_static_setTimeout(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_clearTimeout(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_setInterval(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_clearInterval(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_setHrInterval(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_clearHrInterval(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_setImmediate(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_clearImmediate(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_GC(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

#include "ifs/Buffer.h"
#include "ifs/console.h"
#include "ifs/process.h"
#include "ifs/Worker.h"
#include "ifs/Timer.h"

namespace fibjs {
inline ClassInfo& global_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "run", s_static_run, true },
        { "require", s_static_require, true },
        { "setTimeout", s_static_setTimeout, true },
        { "clearTimeout", s_static_clearTimeout, true },
        { "setInterval", s_static_setInterval, true },
        { "clearInterval", s_static_clearInterval, true },
        { "setHrInterval", s_static_setHrInterval, true },
        { "clearHrInterval", s_static_clearHrInterval, true },
        { "setImmediate", s_static_setImmediate, true },
        { "clearImmediate", s_static_clearImmediate, true },
        { "GC", s_static_GC, true }
    };

    static ClassData::ClassObject s_object[] = {
        { "Buffer", Buffer_base::class_info },
        { "console", console_base::class_info },
        { "process", process_base::class_info }
    };

    static ClassData::ClassProperty s_property[] = {
        { "Master", s_static_get_Master, block_set, true },
        { "global", s_static_get_global, block_set, true },
        { "argv", s_static_get_argv, block_set, true },
        { "__filename", s_static_get___filename, block_set, true },
        { "__dirname", s_static_get___dirname, block_set, true }
    };

    static ClassData s_cd = {
        "global", true, s__new, NULL,
        ARRAYSIZE(s_method), s_method, ARRAYSIZE(s_object), s_object, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &object_base::class_info()
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void global_base::s_static_get_Master(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    obj_ptr<Worker_base> vr;

    METHOD_NAME("global.Master");
    PROPERTY_ENTER();

    hr = get_Master(vr);

    METHOD_RETURN();
}

inline void global_base::s_static_get_global(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Object> vr;

    METHOD_NAME("global.global");
    PROPERTY_ENTER();

    hr = get_global(vr);

    METHOD_RETURN();
}

inline void global_base::s_static_run(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("global.run");
    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(v8::Local<v8::Array>, 1, v8::Array::New(isolate));

    hr = run(v0, v1);

    METHOD_VOID();
}

inline void global_base::s_static_require(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Value> vr;

    METHOD_NAME("global.require");
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = require(v0, vr);

    METHOD_RETURN();
}

inline void global_base::s_static_get_argv(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Array> vr;

    METHOD_NAME("global.argv");
    PROPERTY_ENTER();

    hr = get_argv(vr);

    METHOD_RETURN();
}

inline void global_base::s_static_get___filename(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_NAME("global.__filename");
    PROPERTY_ENTER();

    hr = get___filename(vr);

    METHOD_RETURN();
}

inline void global_base::s_static_get___dirname(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_NAME("global.__dirname");
    PROPERTY_ENTER();

    hr = get___dirname(vr);

    METHOD_RETURN();
}

inline void global_base::s_static_setTimeout(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Timer_base> vr;

    METHOD_NAME("global.setTimeout");
    METHOD_ENTER();

    METHOD_OVER(-1, 1);

    ARG(v8::Local<v8::Function>, 0);
    OPT_ARG(double, 1, 1);
    ARG_LIST(2);

    hr = setTimeout(v0, v1, v2, vr);

    METHOD_RETURN();
}

inline void global_base::s_static_clearTimeout(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("global.clearTimeout");
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = clearTimeout(v0);

    METHOD_VOID();
}

inline void global_base::s_static_setInterval(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Timer_base> vr;

    METHOD_NAME("global.setInterval");
    METHOD_ENTER();

    METHOD_OVER(-1, 2);

    ARG(v8::Local<v8::Function>, 0);
    ARG(double, 1);
    ARG_LIST(2);

    hr = setInterval(v0, v1, v2, vr);

    METHOD_RETURN();
}

inline void global_base::s_static_clearInterval(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("global.clearInterval");
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = clearInterval(v0);

    METHOD_VOID();
}

inline void global_base::s_static_setHrInterval(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Timer_base> vr;

    METHOD_NAME("global.setHrInterval");
    METHOD_ENTER();

    METHOD_OVER(-1, 2);

    ARG(v8::Local<v8::Function>, 0);
    ARG(double, 1);
    ARG_LIST(2);

    hr = setHrInterval(v0, v1, v2, vr);

    METHOD_RETURN();
}

inline void global_base::s_static_clearHrInterval(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("global.clearHrInterval");
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = clearHrInterval(v0);

    METHOD_VOID();
}

inline void global_base::s_static_setImmediate(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Timer_base> vr;

    METHOD_NAME("global.setImmediate");
    METHOD_ENTER();

    METHOD_OVER(-1, 1);

    ARG(v8::Local<v8::Function>, 0);
    ARG_LIST(1);

    hr = setImmediate(v0, v1, vr);

    METHOD_RETURN();
}

inline void global_base::s_static_clearImmediate(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("global.clearImmediate");
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = clearImmediate(v0);

    METHOD_VOID();
}

inline void global_base::s_static_GC(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("global.GC");
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = GC();

    METHOD_VOID();
}
}
