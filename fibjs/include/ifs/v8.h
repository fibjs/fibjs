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
class Buffer_base;

class v8_base : public object_base {
    DECLARE_CLASS(v8_base);

public:
    using Union_deserialize_data = std::variant<obj_ptr<Buffer_base>, exlib::string>;

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
    class GetHeapSpaceStatisticsType : public NType {
    public:
        virtual void to_value(Isolate* isolate, v8::Local<v8::Object>& retVal)
        {
            v8::Local<v8::Context> context = isolate->context();
            retVal->Set(context, isolate->NewString("space_name"), GetReturnValue(isolate, space_name)).Check();
            retVal->Set(context, isolate->NewString("space_size"), GetReturnValue(isolate, space_size)).Check();
            retVal->Set(context, isolate->NewString("space_used_size"), GetReturnValue(isolate, space_used_size)).Check();
            retVal->Set(context, isolate->NewString("space_available_size"), GetReturnValue(isolate, space_available_size)).Check();
            retVal->Set(context, isolate->NewString("physical_space_size"), GetReturnValue(isolate, physical_space_size)).Check();
        }

        virtual void to_args(Isolate* isolate, std::vector<v8::Local<v8::Value>>& args)
        {
            args.push_back(GetReturnValue(isolate, space_name));
            args.push_back(GetReturnValue(isolate, space_size));
            args.push_back(GetReturnValue(isolate, space_used_size));
            args.push_back(GetReturnValue(isolate, space_available_size));
            args.push_back(GetReturnValue(isolate, physical_space_size));
        }

    public:
        exlib::string space_name;
        double space_size;
        double space_used_size;
        double space_available_size;
        double physical_space_size;
    };
    class GetHeapStatisticsType : public NType {
    public:
        virtual void to_value(Isolate* isolate, v8::Local<v8::Object>& retVal)
        {
            v8::Local<v8::Context> context = isolate->context();
            retVal->Set(context, isolate->NewString("total_heap_size"), GetReturnValue(isolate, total_heap_size)).Check();
            retVal->Set(context, isolate->NewString("total_heap_size_executable"), GetReturnValue(isolate, total_heap_size_executable)).Check();
            retVal->Set(context, isolate->NewString("total_physical_size"), GetReturnValue(isolate, total_physical_size)).Check();
            retVal->Set(context, isolate->NewString("total_available_size"), GetReturnValue(isolate, total_available_size)).Check();
            retVal->Set(context, isolate->NewString("used_heap_size"), GetReturnValue(isolate, used_heap_size)).Check();
            retVal->Set(context, isolate->NewString("heap_size_limit"), GetReturnValue(isolate, heap_size_limit)).Check();
            retVal->Set(context, isolate->NewString("malloced_memory"), GetReturnValue(isolate, malloced_memory)).Check();
            retVal->Set(context, isolate->NewString("external_memory"), GetReturnValue(isolate, external_memory)).Check();
            retVal->Set(context, isolate->NewString("peak_malloced_memory"), GetReturnValue(isolate, peak_malloced_memory)).Check();
            retVal->Set(context, isolate->NewString("number_of_native_contexts"), GetReturnValue(isolate, number_of_native_contexts)).Check();
            retVal->Set(context, isolate->NewString("number_of_detached_contexts"), GetReturnValue(isolate, number_of_detached_contexts)).Check();
        }

        virtual void to_args(Isolate* isolate, std::vector<v8::Local<v8::Value>>& args)
        {
            args.push_back(GetReturnValue(isolate, total_heap_size));
            args.push_back(GetReturnValue(isolate, total_heap_size_executable));
            args.push_back(GetReturnValue(isolate, total_physical_size));
            args.push_back(GetReturnValue(isolate, total_available_size));
            args.push_back(GetReturnValue(isolate, used_heap_size));
            args.push_back(GetReturnValue(isolate, heap_size_limit));
            args.push_back(GetReturnValue(isolate, malloced_memory));
            args.push_back(GetReturnValue(isolate, external_memory));
            args.push_back(GetReturnValue(isolate, peak_malloced_memory));
            args.push_back(GetReturnValue(isolate, number_of_native_contexts));
            args.push_back(GetReturnValue(isolate, number_of_detached_contexts));
        }

    public:
        double total_heap_size;
        double total_heap_size_executable;
        double total_physical_size;
        double total_available_size;
        double used_heap_size;
        double heap_size_limit;
        double malloced_memory;
        double external_memory;
        double peak_malloced_memory;
        double number_of_native_contexts;
        double number_of_detached_contexts;
    };

public:
    // v8_base
    static result_t getHeapCodeStatistics(v8::Local<v8::Object>& retVal);
    static result_t getHeapSpaceStatistics(std::vector<obj_ptr<GetHeapSpaceStatisticsType>>& retVal);
    static result_t getHeapStatistics(obj_ptr<GetHeapStatisticsType>& retVal);
    static result_t saveSnapshot(exlib::string fname);
    static result_t loadSnapshot(exlib::string fname, obj_ptr<HeapSnapshot_base>& retVal);
    static result_t takeSnapshot(obj_ptr<HeapSnapshot_base>& retVal);
    static result_t diff(v8::Local<v8::Function> test, v8::Local<v8::Object>& retVal);
    static result_t start(exlib::string fname, int32_t time, int32_t interval, obj_ptr<Timer_base>& retVal);
    static result_t serialize(v8::Local<v8::Value> value, obj_ptr<Buffer_base>& retVal);
    static result_t deserialize(Union_deserialize_data data, v8::Local<v8::Value>& retVal);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        ThrowTypeError("not a constructor");
    }

    static result_t load(v8::Local<v8::Value> v, obj_ptr<v8_base>& retVal)
    { return CALL_E_TYPEMISMATCH; }

public:
    static void s_static_getHeapCodeStatistics(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_getHeapSpaceStatistics(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_getHeapStatistics(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_saveSnapshot(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_loadSnapshot(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_takeSnapshot(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_diff(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_start(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_serialize(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_deserialize(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

#include "ifs/HeapSnapshot.h"
#include "ifs/Timer.h"
#include "ifs/Buffer.h"

namespace fibjs {
inline ClassInfo& v8_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "getHeapCodeStatistics", s_static_getHeapCodeStatistics, true, ClassData::ASYNC_SYNC },
        { "getHeapSpaceStatistics", s_static_getHeapSpaceStatistics, true, ClassData::ASYNC_SYNC },
        { "getHeapStatistics", s_static_getHeapStatistics, true, ClassData::ASYNC_SYNC },
        { "saveSnapshot", s_static_saveSnapshot, true, ClassData::ASYNC_SYNC },
        { "loadSnapshot", s_static_loadSnapshot, true, ClassData::ASYNC_SYNC },
        { "takeSnapshot", s_static_takeSnapshot, true, ClassData::ASYNC_SYNC },
        { "diff", s_static_diff, true, ClassData::ASYNC_SYNC },
        { "start", s_static_start, true, ClassData::ASYNC_SYNC },
        { "serialize", s_static_serialize, true, ClassData::ASYNC_SYNC },
        { "deserialize", s_static_deserialize, true, ClassData::ASYNC_SYNC }
    };

    static ClassData::ClassConst s_const[] = {
        { "Node_Hidden", ClassData::CONST_Integer, { .intValue = C_Node_Hidden } },
        { "Node_Array", ClassData::CONST_Integer, { .intValue = C_Node_Array } },
        { "Node_String", ClassData::CONST_Integer, { .intValue = C_Node_String } },
        { "Node_Object", ClassData::CONST_Integer, { .intValue = C_Node_Object } },
        { "Node_Code", ClassData::CONST_Integer, { .intValue = C_Node_Code } },
        { "Node_Closure", ClassData::CONST_Integer, { .intValue = C_Node_Closure } },
        { "Node_RegExp", ClassData::CONST_Integer, { .intValue = C_Node_RegExp } },
        { "Node_HeapNumber", ClassData::CONST_Integer, { .intValue = C_Node_HeapNumber } },
        { "Node_Native", ClassData::CONST_Integer, { .intValue = C_Node_Native } },
        { "Node_Synthetic", ClassData::CONST_Integer, { .intValue = C_Node_Synthetic } },
        { "Node_ConsString", ClassData::CONST_Integer, { .intValue = C_Node_ConsString } },
        { "Node_SlicedString", ClassData::CONST_Integer, { .intValue = C_Node_SlicedString } },
        { "Node_Symbol", ClassData::CONST_Integer, { .intValue = C_Node_Symbol } },
        { "Node_SimdValue", ClassData::CONST_Integer, { .intValue = C_Node_SimdValue } },
        { "Edge_ContextVariable", ClassData::CONST_Integer, { .intValue = C_Edge_ContextVariable } },
        { "Edge_Element", ClassData::CONST_Integer, { .intValue = C_Edge_Element } },
        { "Edge_Property", ClassData::CONST_Integer, { .intValue = C_Edge_Property } },
        { "Edge_Internal", ClassData::CONST_Integer, { .intValue = C_Edge_Internal } },
        { "Edge_Hidden", ClassData::CONST_Integer, { .intValue = C_Edge_Hidden } },
        { "Edge_Shortcut", ClassData::CONST_Integer, { .intValue = C_Edge_Shortcut } },
        { "Edge_Weak", ClassData::CONST_Integer, { .intValue = C_Edge_Weak } }
    };

    static ClassData s_cd = {
        "v8", true, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, 0, NULL, ARRAYSIZE(s_const), s_const, NULL, NULL,
        &object_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void v8_base::s_static_getHeapCodeStatistics(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Object> vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = getHeapCodeStatistics(vr);

    METHOD_RETURN();
}

inline void v8_base::s_static_getHeapSpaceStatistics(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    std::vector<obj_ptr<GetHeapSpaceStatisticsType>> vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = getHeapSpaceStatistics(vr);

    METHOD_RETURN();
}

inline void v8_base::s_static_getHeapStatistics(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<GetHeapStatisticsType> vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = getHeapStatistics(vr);

    METHOD_RETURN();
}

inline void v8_base::s_static_saveSnapshot(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = saveSnapshot(v0);

    METHOD_VOID();
}

inline void v8_base::s_static_loadSnapshot(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<HeapSnapshot_base> vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = loadSnapshot(v0, vr);

    METHOD_RETURN();
}

inline void v8_base::s_static_takeSnapshot(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<HeapSnapshot_base> vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = takeSnapshot(vr);

    METHOD_RETURN();
}

inline void v8_base::s_static_diff(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Object> vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Function>, 0);

    hr = diff(v0, vr);

    METHOD_RETURN();
}

inline void v8_base::s_static_start(const v8::FunctionCallbackInfo<v8::Value>& args)
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

inline void v8_base::s_static_serialize(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = serialize(v0, vr);

    METHOD_RETURN();
}

inline void v8_base::s_static_deserialize(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Value> vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(Union_deserialize_data, 0);

    hr = deserialize(v0, vr);

    METHOD_RETURN();
}
}
