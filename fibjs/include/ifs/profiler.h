/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _profiler_base_H_
#define _profiler_base_H_

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
        _Node_Hidden = 0,
        _Node_Array = 1,
        _Node_String = 2,
        _Node_Object = 3,
        _Node_Code = 4,
        _Node_Closure = 5,
        _Node_RegExp = 6,
        _Node_HeapNumber = 7,
        _Node_Native = 8,
        _Node_Synthetic = 9,
        _Node_ConsString = 10,
        _Node_SlicedString = 11,
        _Node_Symbol = 12,
        _Node_SimdValue = 13,
        _Edge_ContextVariable = 0,
        _Edge_Element = 1,
        _Edge_Property = 2,
        _Edge_Internal = 3,
        _Edge_Hidden = 4,
        _Edge_Shortcut = 5,
        _Edge_Weak = 6
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
    static void s_saveSnapshot(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_loadSnapshot(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_takeSnapshot(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_diff(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_start(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

#include "HeapSnapshot.h"
#include "Timer.h"

namespace fibjs {
inline ClassInfo& profiler_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "saveSnapshot", s_saveSnapshot, true },
        { "loadSnapshot", s_loadSnapshot, true },
        { "takeSnapshot", s_takeSnapshot, true },
        { "diff", s_diff, true },
        { "start", s_start, true }
    };

    static ClassData::ClassConst s_const[] = {
        { "Node_Hidden", _Node_Hidden },
        { "Node_Array", _Node_Array },
        { "Node_String", _Node_String },
        { "Node_Object", _Node_Object },
        { "Node_Code", _Node_Code },
        { "Node_Closure", _Node_Closure },
        { "Node_RegExp", _Node_RegExp },
        { "Node_HeapNumber", _Node_HeapNumber },
        { "Node_Native", _Node_Native },
        { "Node_Synthetic", _Node_Synthetic },
        { "Node_ConsString", _Node_ConsString },
        { "Node_SlicedString", _Node_SlicedString },
        { "Node_Symbol", _Node_Symbol },
        { "Node_SimdValue", _Node_SimdValue },
        { "Edge_ContextVariable", _Edge_ContextVariable },
        { "Edge_Element", _Edge_Element },
        { "Edge_Property", _Edge_Property },
        { "Edge_Internal", _Edge_Internal },
        { "Edge_Hidden", _Edge_Hidden },
        { "Edge_Shortcut", _Edge_Shortcut },
        { "Edge_Weak", _Edge_Weak }
    };

    static ClassData s_cd = {
        "profiler", true, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, 0, NULL, ARRAYSIZE(s_const), s_const, NULL, NULL,
        &object_base::class_info()
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void profiler_base::s_saveSnapshot(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("profiler.saveSnapshot");
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = saveSnapshot(v0);

    METHOD_VOID();
}

inline void profiler_base::s_loadSnapshot(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<HeapSnapshot_base> vr;

    METHOD_NAME("profiler.loadSnapshot");
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = loadSnapshot(v0, vr);

    METHOD_RETURN();
}

inline void profiler_base::s_takeSnapshot(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<HeapSnapshot_base> vr;

    METHOD_NAME("profiler.takeSnapshot");
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = takeSnapshot(vr);

    METHOD_RETURN();
}

inline void profiler_base::s_diff(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Object> vr;

    METHOD_NAME("profiler.diff");
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Function>, 0);

    hr = diff(v0, vr);

    METHOD_RETURN();
}

inline void profiler_base::s_start(const v8::FunctionCallbackInfo<v8::Value>& args)
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

#endif
