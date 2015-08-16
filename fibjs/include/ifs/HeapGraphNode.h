/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _HeapGraphNode_base_H_
#define _HeapGraphNode_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs
{

class List_base;

class HeapGraphNode_base : public object_base
{
    DECLARE_CLASS(HeapGraphNode_base);

public:
    enum{
        _Hidden = 0,
        _Array = 1,
        _String = 2,
        _Object = 3,
        _Code = 4,
        _Closure = 5,
        _RegExp = 6,
        _HeapNumber = 7,
        _Native = 8,
        _Synthetic = 9,
        _ConsString = 10,
        _SlicedString = 11,
        _Symbol = 12,
        _SimdValue = 13
    };

public:
    // HeapGraphNode_base
    virtual result_t get_type(int32_t& retVal) = 0;
    virtual result_t get_name(std::string& retVal) = 0;
    virtual result_t get_id(int32_t& retVal) = 0;
    virtual result_t get_shallowSize(int32_t& retVal) = 0;
    virtual result_t get_childs(obj_ptr<List_base>& retVal) = 0;

public:
    static void s_get_Hidden(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get_Array(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get_String(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get_Object(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get_Code(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get_Closure(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get_RegExp(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get_HeapNumber(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get_Native(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get_Synthetic(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get_ConsString(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get_SlicedString(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get_Symbol(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get_SimdValue(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get_type(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get_name(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get_id(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get_shallowSize(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get_childs(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
};

}

#include "List.h"

namespace fibjs
{
    inline ClassInfo& HeapGraphNode_base::class_info()
    {
        static ClassData::ClassProperty s_property[] = 
        {
            {"Hidden", s_get_Hidden, block_set, true},
            {"Array", s_get_Array, block_set, true},
            {"String", s_get_String, block_set, true},
            {"Object", s_get_Object, block_set, true},
            {"Code", s_get_Code, block_set, true},
            {"Closure", s_get_Closure, block_set, true},
            {"RegExp", s_get_RegExp, block_set, true},
            {"HeapNumber", s_get_HeapNumber, block_set, true},
            {"Native", s_get_Native, block_set, true},
            {"Synthetic", s_get_Synthetic, block_set, true},
            {"ConsString", s_get_ConsString, block_set, true},
            {"SlicedString", s_get_SlicedString, block_set, true},
            {"Symbol", s_get_Symbol, block_set, true},
            {"SimdValue", s_get_SimdValue, block_set, true},
            {"type", s_get_type, block_set, false},
            {"name", s_get_name, block_set, false},
            {"id", s_get_id, block_set, false},
            {"shallowSize", s_get_shallowSize, block_set, false},
            {"childs", s_get_childs, block_set, false}
        };

        static ClassData s_cd = 
        { 
            "HeapGraphNode", NULL, 
            0, NULL, 0, NULL, 19, s_property, NULL, NULL,
            &object_base::class_info()
        };

        static ClassInfo s_ci(s_cd);
        return s_ci;
    }

    inline void HeapGraphNode_base::s_get_Hidden(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        int32_t vr = _Hidden;
        PROPERTY_ENTER();
        METHOD_RETURN();
    }

    inline void HeapGraphNode_base::s_get_Array(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        int32_t vr = _Array;
        PROPERTY_ENTER();
        METHOD_RETURN();
    }

    inline void HeapGraphNode_base::s_get_String(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        int32_t vr = _String;
        PROPERTY_ENTER();
        METHOD_RETURN();
    }

    inline void HeapGraphNode_base::s_get_Object(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        int32_t vr = _Object;
        PROPERTY_ENTER();
        METHOD_RETURN();
    }

    inline void HeapGraphNode_base::s_get_Code(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        int32_t vr = _Code;
        PROPERTY_ENTER();
        METHOD_RETURN();
    }

    inline void HeapGraphNode_base::s_get_Closure(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        int32_t vr = _Closure;
        PROPERTY_ENTER();
        METHOD_RETURN();
    }

    inline void HeapGraphNode_base::s_get_RegExp(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        int32_t vr = _RegExp;
        PROPERTY_ENTER();
        METHOD_RETURN();
    }

    inline void HeapGraphNode_base::s_get_HeapNumber(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        int32_t vr = _HeapNumber;
        PROPERTY_ENTER();
        METHOD_RETURN();
    }

    inline void HeapGraphNode_base::s_get_Native(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        int32_t vr = _Native;
        PROPERTY_ENTER();
        METHOD_RETURN();
    }

    inline void HeapGraphNode_base::s_get_Synthetic(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        int32_t vr = _Synthetic;
        PROPERTY_ENTER();
        METHOD_RETURN();
    }

    inline void HeapGraphNode_base::s_get_ConsString(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        int32_t vr = _ConsString;
        PROPERTY_ENTER();
        METHOD_RETURN();
    }

    inline void HeapGraphNode_base::s_get_SlicedString(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        int32_t vr = _SlicedString;
        PROPERTY_ENTER();
        METHOD_RETURN();
    }

    inline void HeapGraphNode_base::s_get_Symbol(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        int32_t vr = _Symbol;
        PROPERTY_ENTER();
        METHOD_RETURN();
    }

    inline void HeapGraphNode_base::s_get_SimdValue(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        int32_t vr = _SimdValue;
        PROPERTY_ENTER();
        METHOD_RETURN();
    }

    inline void HeapGraphNode_base::s_get_type(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        int32_t vr;

        PROPERTY_ENTER();
        PROPERTY_INSTANCE(HeapGraphNode_base);

        hr = pInst->get_type(vr);

        METHOD_RETURN();
    }

    inline void HeapGraphNode_base::s_get_name(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        std::string vr;

        PROPERTY_ENTER();
        PROPERTY_INSTANCE(HeapGraphNode_base);

        hr = pInst->get_name(vr);

        METHOD_RETURN();
    }

    inline void HeapGraphNode_base::s_get_id(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        int32_t vr;

        PROPERTY_ENTER();
        PROPERTY_INSTANCE(HeapGraphNode_base);

        hr = pInst->get_id(vr);

        METHOD_RETURN();
    }

    inline void HeapGraphNode_base::s_get_shallowSize(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        int32_t vr;

        PROPERTY_ENTER();
        PROPERTY_INSTANCE(HeapGraphNode_base);

        hr = pInst->get_shallowSize(vr);

        METHOD_RETURN();
    }

    inline void HeapGraphNode_base::s_get_childs(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        obj_ptr<List_base> vr;

        PROPERTY_ENTER();
        PROPERTY_INSTANCE(HeapGraphNode_base);

        hr = pInst->get_childs(vr);

        METHOD_RETURN();
    }

}

#endif

