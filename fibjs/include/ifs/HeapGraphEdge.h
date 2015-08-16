/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _HeapGraphEdge_base_H_
#define _HeapGraphEdge_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs
{

class HeapGraphNode_base;

class HeapGraphEdge_base : public object_base
{
    DECLARE_CLASS(HeapGraphEdge_base);

public:
    enum{
        _ContextVariable = 0,
        _Element = 1,
        _Property = 2,
        _Internal = 3,
        _Hidden = 4,
        _Shortcut = 5,
        _Weak = 6
    };

public:
    // HeapGraphEdge_base
    virtual result_t get_type(int32_t& retVal) = 0;
    virtual result_t get_name(std::string& retVal) = 0;
    virtual result_t getFromNode(obj_ptr<HeapGraphNode_base>& retVal) = 0;
    virtual result_t getToNode(obj_ptr<HeapGraphNode_base>& retVal) = 0;

public:
    static void s_get_ContextVariable(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get_Element(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get_Property(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get_Internal(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get_Hidden(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get_Shortcut(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get_Weak(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get_type(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get_name(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_getFromNode(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_getToNode(const v8::FunctionCallbackInfo<v8::Value>& args);
};

}

#include "HeapGraphNode.h"

namespace fibjs
{
    inline ClassInfo& HeapGraphEdge_base::class_info()
    {
        static ClassData::ClassMethod s_method[] = 
        {
            {"getFromNode", s_getFromNode, false},
            {"getToNode", s_getToNode, false}
        };

        static ClassData::ClassProperty s_property[] = 
        {
            {"ContextVariable", s_get_ContextVariable, block_set, true},
            {"Element", s_get_Element, block_set, true},
            {"Property", s_get_Property, block_set, true},
            {"Internal", s_get_Internal, block_set, true},
            {"Hidden", s_get_Hidden, block_set, true},
            {"Shortcut", s_get_Shortcut, block_set, true},
            {"Weak", s_get_Weak, block_set, true},
            {"type", s_get_type, block_set, false},
            {"name", s_get_name, block_set, false}
        };

        static ClassData s_cd = 
        { 
            "HeapGraphEdge", NULL, 
            2, s_method, 0, NULL, 9, s_property, NULL, NULL,
            &object_base::class_info()
        };

        static ClassInfo s_ci(s_cd);
        return s_ci;
    }

    inline void HeapGraphEdge_base::s_get_ContextVariable(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        int32_t vr = _ContextVariable;
        PROPERTY_ENTER();
        METHOD_RETURN();
    }

    inline void HeapGraphEdge_base::s_get_Element(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        int32_t vr = _Element;
        PROPERTY_ENTER();
        METHOD_RETURN();
    }

    inline void HeapGraphEdge_base::s_get_Property(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        int32_t vr = _Property;
        PROPERTY_ENTER();
        METHOD_RETURN();
    }

    inline void HeapGraphEdge_base::s_get_Internal(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        int32_t vr = _Internal;
        PROPERTY_ENTER();
        METHOD_RETURN();
    }

    inline void HeapGraphEdge_base::s_get_Hidden(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        int32_t vr = _Hidden;
        PROPERTY_ENTER();
        METHOD_RETURN();
    }

    inline void HeapGraphEdge_base::s_get_Shortcut(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        int32_t vr = _Shortcut;
        PROPERTY_ENTER();
        METHOD_RETURN();
    }

    inline void HeapGraphEdge_base::s_get_Weak(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        int32_t vr = _Weak;
        PROPERTY_ENTER();
        METHOD_RETURN();
    }

    inline void HeapGraphEdge_base::s_get_type(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        int32_t vr;

        PROPERTY_ENTER();
        PROPERTY_INSTANCE(HeapGraphEdge_base);

        hr = pInst->get_type(vr);

        METHOD_RETURN();
    }

    inline void HeapGraphEdge_base::s_get_name(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        std::string vr;

        PROPERTY_ENTER();
        PROPERTY_INSTANCE(HeapGraphEdge_base);

        hr = pInst->get_name(vr);

        METHOD_RETURN();
    }

    inline void HeapGraphEdge_base::s_getFromNode(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        obj_ptr<HeapGraphNode_base> vr;

        METHOD_INSTANCE(HeapGraphEdge_base);
        METHOD_ENTER(0, 0);

        hr = pInst->getFromNode(vr);

        METHOD_RETURN();
    }

    inline void HeapGraphEdge_base::s_getToNode(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        obj_ptr<HeapGraphNode_base> vr;

        METHOD_INSTANCE(HeapGraphEdge_base);
        METHOD_ENTER(0, 0);

        hr = pInst->getToNode(vr);

        METHOD_RETURN();
    }

}

#endif

