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
    // HeapGraphEdge_base
    virtual result_t get_type(int32_t& retVal) = 0;
    virtual result_t get_name(exlib::string& retVal) = 0;
    virtual result_t get_description(exlib::string& retVal) = 0;
    virtual result_t getFromNode(obj_ptr<HeapGraphNode_base>& retVal) = 0;
    virtual result_t getToNode(obj_ptr<HeapGraphNode_base>& retVal) = 0;

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        Isolate* isolate = Isolate::current();

        isolate->m_isolate->ThrowException(
            isolate->NewFromUtf8("not a constructor"));
    }

public:
    static void s_get_type(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get_name(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get_description(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
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
            {"type", s_get_type, block_set, false},
            {"name", s_get_name, block_set, false},
            {"description", s_get_description, block_set, false}
        };

        static ClassData s_cd = 
        { 
            "HeapGraphEdge", s__new, NULL, 
            ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, NULL, NULL,
            &object_base::class_info()
        };

        static ClassInfo s_ci(s_cd);
        return s_ci;
    }

    inline void HeapGraphEdge_base::s_get_type(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        int32_t vr;

        METHOD_INSTANCE(HeapGraphEdge_base);
        PROPERTY_ENTER();

        hr = pInst->get_type(vr);

        METHOD_RETURN();
    }

    inline void HeapGraphEdge_base::s_get_name(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        exlib::string vr;

        METHOD_INSTANCE(HeapGraphEdge_base);
        PROPERTY_ENTER();

        hr = pInst->get_name(vr);

        METHOD_RETURN();
    }

    inline void HeapGraphEdge_base::s_get_description(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        exlib::string vr;

        METHOD_INSTANCE(HeapGraphEdge_base);
        PROPERTY_ENTER();

        hr = pInst->get_description(vr);

        METHOD_RETURN();
    }

    inline void HeapGraphEdge_base::s_getFromNode(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        obj_ptr<HeapGraphNode_base> vr;

        METHOD_INSTANCE(HeapGraphEdge_base);
        METHOD_ENTER();

        METHOD_OVER(0, 0);

        hr = pInst->getFromNode(vr);

        METHOD_RETURN();
    }

    inline void HeapGraphEdge_base::s_getToNode(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        obj_ptr<HeapGraphNode_base> vr;

        METHOD_INSTANCE(HeapGraphEdge_base);
        METHOD_ENTER();

        METHOD_OVER(0, 0);

        hr = pInst->getToNode(vr);

        METHOD_RETURN();
    }

}

#endif

