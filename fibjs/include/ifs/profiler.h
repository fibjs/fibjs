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

class HeapSnapshot_base;
class Timer_base;

class profiler_base : public object_base {
    DECLARE_CLASS(profiler_base);

public:
    enum {
        __Node_Hidden = 0,
        __Node_Array = 1,
        __Node_String = 2,
        __Node_Object = 3,
        __Node_Code = 4,
        __Node_Closure = 5,
        __Node_RegExp = 6,
        __Node_HeapNumber = 7,
        __Node_Native = 8,
        __Node_Synthetic = 9,
        __Node_ConsString = 10,
        __Node_SlicedString = 11,
        __Node_Symbol = 12,
        __Node_SimdValue = 13,
        __Edge_ContextVariable = 0,
        __Edge_Element = 1,
        __Edge_Property = 2,
        __Edge_Internal = 3,
        __Edge_Hidden = 4,
        __Edge_Shortcut = 5,
        __Edge_Weak = 6
    };

public:
    // profiler_base
    static result_t saveSnapshot(exlib::string fname);
    static result_t loadSnapshot(exlib::string fname, obj_ptr<HeapSnapshot_base>& retVal);
    static result_t takeSnapshot(obj_ptr<HeapSnapshot_base>& retVal);
    static result_t diff(v8::Local<v8::Function> test, v8::Local<v8::Object>& retVal);
    static result_t start(exlib::string fname, int32_t time, int32_t interval, obj_ptr<Timer_base>& retVal);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        Isolate* isolate = Isolate::current();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_static_saveSnapshot(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_loadSnapshot(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_takeSnapshot(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_diff(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_start(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

#include "ifs/HeapSnapshot.h"
#include "ifs/Timer.h"

namespace fibjs {
inline ClassInfo& profiler_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "saveSnapshot", s_static_saveSnapshot, true },
        { "loadSnapshot", s_static_loadSnapshot, true },
        { "takeSnapshot", s_static_takeSnapshot, true },
        { "diff", s_static_diff, true },
        { "start", s_static_start, true }
    };

    static ClassData::ClassConst s_const[] = {
        { "Node_Hidden", __Node_Hidden },
        { "Node_Array", __Node_Array },
        { "Node_String", __Node_String },
        { "Node_Object", __Node_Object },
        { "Node_Code", __Node_Code },
        { "Node_Closure", __Node_Closure },
        { "Node_RegExp", __Node_RegExp },
        { "Node_HeapNumber", __Node_HeapNumber },
        { "Node_Native", __Node_Native },
        { "Node_Synthetic", __Node_Synthetic },
        { "Node_ConsString", __Node_ConsString },
        { "Node_SlicedString", __Node_SlicedString },
        { "Node_Symbol", __Node_Symbol },
        { "Node_SimdValue", __Node_SimdValue },
        { "Edge_ContextVariable", __Edge_ContextVariable },
        { "Edge_Element", __Edge_Element },
        { "Edge_Property", __Edge_Property },
        { "Edge_Internal", __Edge_Internal },
        { "Edge_Hidden", __Edge_Hidden },
        { "Edge_Shortcut", __Edge_Shortcut },
        { "Edge_Weak", __Edge_Weak }
    };

    static ClassData s_cd = {
        "profiler", true, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, 0, NULL, ARRAYSIZE(s_const), s_const, NULL, NULL,
        &object_base::class_info()
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void profiler_base::s_static_saveSnapshot(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("profiler.saveSnapshot");
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = saveSnapshot(v0);

    METHOD_VOID();
}

inline void profiler_base::s_static_loadSnapshot(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<HeapSnapshot_base> vr;

    METHOD_NAME("profiler.loadSnapshot");
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = loadSnapshot(v0, vr);

    METHOD_RETURN();
}

inline void profiler_base::s_static_takeSnapshot(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<HeapSnapshot_base> vr;

    METHOD_NAME("profiler.takeSnapshot");
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = takeSnapshot(vr);

    METHOD_RETURN();
}

inline void profiler_base::s_static_diff(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Object> vr;

    METHOD_NAME("profiler.diff");
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Function>, 0);

    hr = diff(v0, vr);

    METHOD_RETURN();
}

inline void profiler_base::s_static_start(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Timer_base> vr;

    METHOD_NAME("profiler.start");
    METHOD_ENTER();

    METHOD_OVER(3, 1);

    ARG(exlib::string, 0);
    OPT_ARG(int32_t, 1, 60000);
    OPT_ARG(int32_t, 2, 100);

    hr = start(v0, v1, v2, vr);

    METHOD_RETURN();
}
}
