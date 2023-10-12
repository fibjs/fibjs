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
class UrlObject_base;
class TextDecoder_base;
class TextEncoder_base;
class console_base;
class process_base;
class performance_base;
class Timer_base;

class global_base : public object_base {
    DECLARE_CLASS(global_base);

public:
    // global_base
    static result_t get_global(v8::Local<v8::Object>& retVal);
    static result_t get_globalThis(v8::Local<v8::Object>& retVal);
    static result_t run(exlib::string fname);
    static result_t require(exlib::string id, v8::Local<v8::Value>& retVal);
    static result_t setTimeout(v8::Local<v8::Function> callback, double timeout, OptArgs args, obj_ptr<Timer_base>& retVal);
    static result_t clearTimeout(v8::Local<v8::Value> t);
    static result_t setInterval(v8::Local<v8::Function> callback, double timeout, OptArgs args, obj_ptr<Timer_base>& retVal);
    static result_t clearInterval(v8::Local<v8::Value> t);
    static result_t setHrInterval(v8::Local<v8::Function> callback, double timeout, OptArgs args, obj_ptr<Timer_base>& retVal);
    static result_t clearHrInterval(v8::Local<v8::Value> t);
    static result_t setImmediate(v8::Local<v8::Function> callback, OptArgs args, obj_ptr<Timer_base>& retVal);
    static result_t clearImmediate(v8::Local<v8::Value> t);
    static result_t btoa(Buffer_base* data, bool url, exlib::string& retVal);
    static result_t atob(exlib::string data, obj_ptr<Buffer_base>& retVal);
    static result_t GC();

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_static_get_global(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_static_get_globalThis(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_static_run(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_require(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_setTimeout(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_clearTimeout(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_setInterval(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_clearInterval(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_setHrInterval(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_clearHrInterval(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_setImmediate(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_clearImmediate(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_btoa(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_atob(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_GC(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

#include "ifs/Buffer.h"
#include "ifs/UrlObject.h"
#include "ifs/TextDecoder.h"
#include "ifs/TextEncoder.h"
#include "ifs/console.h"
#include "ifs/process.h"
#include "ifs/performance.h"
#include "ifs/Timer.h"

namespace fibjs {
inline ClassInfo& global_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "run", s_static_run, true, false },
        { "require", s_static_require, true, false },
        { "setTimeout", s_static_setTimeout, true, false },
        { "clearTimeout", s_static_clearTimeout, true, false },
        { "setInterval", s_static_setInterval, true, false },
        { "clearInterval", s_static_clearInterval, true, false },
        { "setHrInterval", s_static_setHrInterval, true, false },
        { "clearHrInterval", s_static_clearHrInterval, true, false },
        { "setImmediate", s_static_setImmediate, true, false },
        { "clearImmediate", s_static_clearImmediate, true, false },
        { "btoa", s_static_btoa, true, false },
        { "atob", s_static_atob, true, false },
        { "GC", s_static_GC, true, false }
    };

    static ClassData::ClassObject s_object[] = {
        { "Buffer", Buffer_base::class_info },
        { "URL", UrlObject_base::class_info },
        { "TextDecoder", TextDecoder_base::class_info },
        { "TextEncoder", TextEncoder_base::class_info },
        { "console", console_base::class_info },
        { "process", process_base::class_info },
        { "performance", performance_base::class_info }
    };

    static ClassData::ClassProperty s_property[] = {
        { "global", s_static_get_global, block_set, true },
        { "globalThis", s_static_get_globalThis, block_set, true }
    };

    static ClassData s_cd = {
        "global", true, s__new, NULL,
        ARRAYSIZE(s_method), s_method, ARRAYSIZE(s_object), s_object, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &object_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void global_base::s_static_get_global(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Object> vr;

    PROPERTY_ENTER();

    hr = get_global(vr);

    METHOD_RETURN();
}

inline void global_base::s_static_get_globalThis(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Object> vr;

    PROPERTY_ENTER();

    hr = get_globalThis(vr);

    METHOD_RETURN();
}

inline void global_base::s_static_run(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = run(v0);

    METHOD_VOID();
}

inline void global_base::s_static_require(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Value> vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = require(v0, vr);

    METHOD_RETURN();
}

inline void global_base::s_static_setTimeout(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Timer_base> vr;

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
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = clearTimeout(v0);

    METHOD_VOID();
}

inline void global_base::s_static_setInterval(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Timer_base> vr;

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
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = clearInterval(v0);

    METHOD_VOID();
}

inline void global_base::s_static_setHrInterval(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Timer_base> vr;

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
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = clearHrInterval(v0);

    METHOD_VOID();
}

inline void global_base::s_static_setImmediate(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Timer_base> vr;

    METHOD_ENTER();

    METHOD_OVER(-1, 1);

    ARG(v8::Local<v8::Function>, 0);
    ARG_LIST(1);

    hr = setImmediate(v0, v1, vr);

    METHOD_RETURN();
}

inline void global_base::s_static_clearImmediate(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = clearImmediate(v0);

    METHOD_VOID();
}

inline void global_base::s_static_btoa(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(obj_ptr<Buffer_base>, 0);
    OPT_ARG(bool, 1, false);

    hr = btoa(v0, v1, vr);

    METHOD_RETURN();
}

inline void global_base::s_static_atob(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = atob(v0, vr);

    METHOD_RETURN();
}

inline void global_base::s_static_GC(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = GC();

    METHOD_VOID();
}
}
