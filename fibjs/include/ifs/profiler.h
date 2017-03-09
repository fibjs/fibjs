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

namespace fibjs
{

class HeapSnapshot_base;

class profiler_base : public object_base
{
    DECLARE_CLASS(profiler_base);

public:
    enum{
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

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        Isolate* isolate = Isolate::current();

        isolate->m_isolate->ThrowException(
            isolate->NewFromUtf8("not a constructor"));
    }

public:
    static void s_get_Node_Hidden(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get_Node_Array(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get_Node_String(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get_Node_Object(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get_Node_Code(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get_Node_Closure(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get_Node_RegExp(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get_Node_HeapNumber(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get_Node_Native(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get_Node_Synthetic(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get_Node_ConsString(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get_Node_SlicedString(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get_Node_Symbol(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get_Node_SimdValue(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get_Edge_ContextVariable(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get_Edge_Element(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get_Edge_Property(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get_Edge_Internal(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get_Edge_Hidden(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get_Edge_Shortcut(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get_Edge_Weak(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_saveSnapshot(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_loadSnapshot(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_takeSnapshot(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_diff(const v8::FunctionCallbackInfo<v8::Value>& args);
};

}

#include "HeapSnapshot.h"

namespace fibjs
{
    inline ClassInfo& profiler_base::class_info()
    {
        static ClassData::ClassMethod s_method[] = 
        {
            {"saveSnapshot", s_saveSnapshot, true},
            {"loadSnapshot", s_loadSnapshot, true},
            {"takeSnapshot", s_takeSnapshot, true},
            {"diff", s_diff, true}
        };

        static ClassData::ClassProperty s_property[] = 
        {
            {"Node_Hidden", s_get_Node_Hidden, block_set, true},
            {"Node_Array", s_get_Node_Array, block_set, true},
            {"Node_String", s_get_Node_String, block_set, true},
            {"Node_Object", s_get_Node_Object, block_set, true},
            {"Node_Code", s_get_Node_Code, block_set, true},
            {"Node_Closure", s_get_Node_Closure, block_set, true},
            {"Node_RegExp", s_get_Node_RegExp, block_set, true},
            {"Node_HeapNumber", s_get_Node_HeapNumber, block_set, true},
            {"Node_Native", s_get_Node_Native, block_set, true},
            {"Node_Synthetic", s_get_Node_Synthetic, block_set, true},
            {"Node_ConsString", s_get_Node_ConsString, block_set, true},
            {"Node_SlicedString", s_get_Node_SlicedString, block_set, true},
            {"Node_Symbol", s_get_Node_Symbol, block_set, true},
            {"Node_SimdValue", s_get_Node_SimdValue, block_set, true},
            {"Edge_ContextVariable", s_get_Edge_ContextVariable, block_set, true},
            {"Edge_Element", s_get_Edge_Element, block_set, true},
            {"Edge_Property", s_get_Edge_Property, block_set, true},
            {"Edge_Internal", s_get_Edge_Internal, block_set, true},
            {"Edge_Hidden", s_get_Edge_Hidden, block_set, true},
            {"Edge_Shortcut", s_get_Edge_Shortcut, block_set, true},
            {"Edge_Weak", s_get_Edge_Weak, block_set, true}
        };

        static ClassData s_cd = 
        { 
            "profiler", true, s__new, NULL, 
            ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, NULL, NULL,
            &object_base::class_info()
        };

        static ClassInfo s_ci(s_cd);
        return s_ci;
    }

    inline void profiler_base::s_get_Node_Hidden(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        int32_t vr = _Node_Hidden;
        PROPERTY_ENTER();
        METHOD_RETURN();
    }

    inline void profiler_base::s_get_Node_Array(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        int32_t vr = _Node_Array;
        PROPERTY_ENTER();
        METHOD_RETURN();
    }

    inline void profiler_base::s_get_Node_String(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        int32_t vr = _Node_String;
        PROPERTY_ENTER();
        METHOD_RETURN();
    }

    inline void profiler_base::s_get_Node_Object(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        int32_t vr = _Node_Object;
        PROPERTY_ENTER();
        METHOD_RETURN();
    }

    inline void profiler_base::s_get_Node_Code(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        int32_t vr = _Node_Code;
        PROPERTY_ENTER();
        METHOD_RETURN();
    }

    inline void profiler_base::s_get_Node_Closure(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        int32_t vr = _Node_Closure;
        PROPERTY_ENTER();
        METHOD_RETURN();
    }

    inline void profiler_base::s_get_Node_RegExp(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        int32_t vr = _Node_RegExp;
        PROPERTY_ENTER();
        METHOD_RETURN();
    }

    inline void profiler_base::s_get_Node_HeapNumber(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        int32_t vr = _Node_HeapNumber;
        PROPERTY_ENTER();
        METHOD_RETURN();
    }

    inline void profiler_base::s_get_Node_Native(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        int32_t vr = _Node_Native;
        PROPERTY_ENTER();
        METHOD_RETURN();
    }

    inline void profiler_base::s_get_Node_Synthetic(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        int32_t vr = _Node_Synthetic;
        PROPERTY_ENTER();
        METHOD_RETURN();
    }

    inline void profiler_base::s_get_Node_ConsString(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        int32_t vr = _Node_ConsString;
        PROPERTY_ENTER();
        METHOD_RETURN();
    }

    inline void profiler_base::s_get_Node_SlicedString(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        int32_t vr = _Node_SlicedString;
        PROPERTY_ENTER();
        METHOD_RETURN();
    }

    inline void profiler_base::s_get_Node_Symbol(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        int32_t vr = _Node_Symbol;
        PROPERTY_ENTER();
        METHOD_RETURN();
    }

    inline void profiler_base::s_get_Node_SimdValue(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        int32_t vr = _Node_SimdValue;
        PROPERTY_ENTER();
        METHOD_RETURN();
    }

    inline void profiler_base::s_get_Edge_ContextVariable(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        int32_t vr = _Edge_ContextVariable;
        PROPERTY_ENTER();
        METHOD_RETURN();
    }

    inline void profiler_base::s_get_Edge_Element(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        int32_t vr = _Edge_Element;
        PROPERTY_ENTER();
        METHOD_RETURN();
    }

    inline void profiler_base::s_get_Edge_Property(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        int32_t vr = _Edge_Property;
        PROPERTY_ENTER();
        METHOD_RETURN();
    }

    inline void profiler_base::s_get_Edge_Internal(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        int32_t vr = _Edge_Internal;
        PROPERTY_ENTER();
        METHOD_RETURN();
    }

    inline void profiler_base::s_get_Edge_Hidden(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        int32_t vr = _Edge_Hidden;
        PROPERTY_ENTER();
        METHOD_RETURN();
    }

    inline void profiler_base::s_get_Edge_Shortcut(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        int32_t vr = _Edge_Shortcut;
        PROPERTY_ENTER();
        METHOD_RETURN();
    }

    inline void profiler_base::s_get_Edge_Weak(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        int32_t vr = _Edge_Weak;
        PROPERTY_ENTER();
        METHOD_RETURN();
    }

    inline void profiler_base::s_saveSnapshot(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        METHOD_ENTER();

        METHOD_OVER(1, 1);

        ARG(exlib::string, 0);

        hr = saveSnapshot(v0);

        METHOD_VOID();
    }

    inline void profiler_base::s_loadSnapshot(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        obj_ptr<HeapSnapshot_base> vr;

        METHOD_ENTER();

        METHOD_OVER(1, 1);

        ARG(exlib::string, 0);

        hr = loadSnapshot(v0, vr);

        METHOD_RETURN();
    }

    inline void profiler_base::s_takeSnapshot(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        obj_ptr<HeapSnapshot_base> vr;

        METHOD_ENTER();

        METHOD_OVER(0, 0);

        hr = takeSnapshot(vr);

        METHOD_RETURN();
    }

    inline void profiler_base::s_diff(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        v8::Local<v8::Object> vr;

        METHOD_ENTER();

        METHOD_OVER(1, 1);

        ARG(v8::Local<v8::Function>, 0);

        hr = diff(v0, vr);

        METHOD_RETURN();
    }

}

#endif

