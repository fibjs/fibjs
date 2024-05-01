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
        C_Node_Hidden = 0,
        C_Node_Array = 1,
        C_Node_String = 2,
        C_Node_Object = 3,
        C_Node_Code = 4,
        C_Node_Closure = 5,
        C_Node_RegExp = 6,
        C_Node_HeapNumber = 7,
        C_Node_Native = 8,
        C_Node_Synthetic = 9,
        C_Node_ConsString = 10,
        C_Node_SlicedString = 11,
        C_Node_Symbol = 12,
        C_Node_SimdValue = 13,
        C_Edge_ContextVariable = 0,
        C_Edge_Element = 1,
        C_Edge_Property = 2,
        C_Edge_Internal = 3,
        C_Edge_Hidden = 4,
        C_Edge_Shortcut = 5,
        C_Edge_Weak = 6
    };

public:
    // profiler_base
    static result_t getHeapCodeStatistics(v8::Local<v8::Object>& retVal);
    static result_t getHeapSpaceStatistics(v8::Local<v8::Array>& retVal);
    static result_t getHeapStatistics(v8::Local<v8::Object>& retVal);
    static result_t saveSnapshot(exlib::string fname);
    static result_t loadSnapshot(exlib::string fname, obj_ptr<HeapSnapshot_base>& retVal);
    static result_t takeSnapshot(obj_ptr<HeapSnapshot_base>& retVal);
    static result_t diff(v8::Local<v8::Function> test, v8::Local<v8::Object>& retVal);
    static result_t start(exlib::string fname, int32_t time, int32_t interval, obj_ptr<Timer_base>& retVal);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_static_getHeapCodeStatistics(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_getHeapSpaceStatistics(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_getHeapStatistics(const v8::FunctionCallbackInfo<v8::Value>& args);
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
        { "getHeapCodeStatistics", s_static_getHeapCodeStatistics, true, ClassData::ASYNC_SYNC },
        { "getHeapSpaceStatistics", s_static_getHeapSpaceStatistics, true, ClassData::ASYNC_SYNC },
        { "getHeapStatistics", s_static_getHeapStatistics, true, ClassData::ASYNC_SYNC },
        { "saveSnapshot", s_static_saveSnapshot, true, ClassData::ASYNC_SYNC },
        { "loadSnapshot", s_static_loadSnapshot, true, ClassData::ASYNC_SYNC },
        { "takeSnapshot", s_static_takeSnapshot, true, ClassData::ASYNC_SYNC },
        { "diff", s_static_diff, true, ClassData::ASYNC_SYNC },
        { "start", s_static_start, true, ClassData::ASYNC_SYNC }
    };

    static ClassData::ClassConst s_const[] = {
        { "Node_Hidden", C_Node_Hidden },
        { "Node_Array", C_Node_Array },
        { "Node_String", C_Node_String },
        { "Node_Object", C_Node_Object },
        { "Node_Code", C_Node_Code },
        { "Node_Closure", C_Node_Closure },
        { "Node_RegExp", C_Node_RegExp },
        { "Node_HeapNumber", C_Node_HeapNumber },
        { "Node_Native", C_Node_Native },
        { "Node_Synthetic", C_Node_Synthetic },
        { "Node_ConsString", C_Node_ConsString },
        { "Node_SlicedString", C_Node_SlicedString },
        { "Node_Symbol", C_Node_Symbol },
        { "Node_SimdValue", C_Node_SimdValue },
        { "Edge_ContextVariable", C_Edge_ContextVariable },
        { "Edge_Element", C_Edge_Element },
        { "Edge_Property", C_Edge_Property },
        { "Edge_Internal", C_Edge_Internal },
        { "Edge_Hidden", C_Edge_Hidden },
        { "Edge_Shortcut", C_Edge_Shortcut },
        { "Edge_Weak", C_Edge_Weak }
    };

    static ClassData s_cd = {
        "profiler", true, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, 0, NULL, ARRAYSIZE(s_const), s_const, NULL, NULL,
        &object_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void profiler_base::s_static_getHeapCodeStatistics(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Object> vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = getHeapCodeStatistics(vr);

    METHOD_RETURN();
}

inline void profiler_base::s_static_getHeapSpaceStatistics(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Array> vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = getHeapSpaceStatistics(vr);

    METHOD_RETURN();
}

inline void profiler_base::s_static_getHeapStatistics(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Object> vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = getHeapStatistics(vr);

    METHOD_RETURN();
}

inline void profiler_base::s_static_saveSnapshot(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = saveSnapshot(v0);

    METHOD_VOID();
}

inline void profiler_base::s_static_loadSnapshot(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<HeapSnapshot_base> vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = loadSnapshot(v0, vr);

    METHOD_RETURN();
}

inline void profiler_base::s_static_takeSnapshot(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<HeapSnapshot_base> vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = takeSnapshot(vr);

    METHOD_RETURN();
}

inline void profiler_base::s_static_diff(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Object> vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Function>, 0);

    hr = diff(v0, vr);

    METHOD_RETURN();
}

inline void profiler_base::s_static_start(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Timer_base> vr;

    METHOD_ENTER();

    METHOD_OVER(3, 1);

    ARG(exlib::string, 0);
    OPT_ARG(int32_t, 1, 60000);
    OPT_ARG(int32_t, 2, 100);

    hr = start(v0, v1, v2, vr);

    METHOD_RETURN();
}
}
