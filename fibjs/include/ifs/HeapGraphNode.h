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

namespace fibjs {

class HeapGraphNode_base : public object_base {
    DECLARE_CLASS(HeapGraphNode_base);

public:
    // HeapGraphNode_base
    virtual result_t get_type(int32_t& retVal) = 0;
    virtual result_t get_name(exlib::string& retVal) = 0;
    virtual result_t get_description(exlib::string& retVal) = 0;
    virtual result_t get_id(int32_t& retVal) = 0;
    virtual result_t get_shallowSize(int32_t& retVal) = 0;
    virtual result_t get_childs(obj_ptr<NArray>& retVal) = 0;

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        Isolate* isolate = Isolate::current();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_get_type(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_name(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_description(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_id(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_shallowSize(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_childs(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
};
}

namespace fibjs {
inline ClassInfo& HeapGraphNode_base::class_info()
{
    static ClassData::ClassProperty s_property[] = {
        { "type", s_get_type, block_set, false },
        { "name", s_get_name, block_set, false },
        { "description", s_get_description, block_set, false },
        { "id", s_get_id, block_set, false },
        { "shallowSize", s_get_shallowSize, block_set, false },
        { "childs", s_get_childs, block_set, false }
    };

    static ClassData s_cd = {
        "HeapGraphNode", false, s__new, NULL,
        0, NULL, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &object_base::class_info()
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void HeapGraphNode_base::s_get_type(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_NAME("HeapGraphNode.type");
    METHOD_INSTANCE(HeapGraphNode_base);
    PROPERTY_ENTER();

    hr = pInst->get_type(vr);

    METHOD_RETURN();
}

inline void HeapGraphNode_base::s_get_name(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_NAME("HeapGraphNode.name");
    METHOD_INSTANCE(HeapGraphNode_base);
    PROPERTY_ENTER();

    hr = pInst->get_name(vr);

    METHOD_RETURN();
}

inline void HeapGraphNode_base::s_get_description(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_NAME("HeapGraphNode.description");
    METHOD_INSTANCE(HeapGraphNode_base);
    PROPERTY_ENTER();

    hr = pInst->get_description(vr);

    METHOD_RETURN();
}

inline void HeapGraphNode_base::s_get_id(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_NAME("HeapGraphNode.id");
    METHOD_INSTANCE(HeapGraphNode_base);
    PROPERTY_ENTER();

    hr = pInst->get_id(vr);

    METHOD_RETURN();
}

inline void HeapGraphNode_base::s_get_shallowSize(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_NAME("HeapGraphNode.shallowSize");
    METHOD_INSTANCE(HeapGraphNode_base);
    PROPERTY_ENTER();

    hr = pInst->get_shallowSize(vr);

    METHOD_RETURN();
}

inline void HeapGraphNode_base::s_get_childs(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    obj_ptr<NArray> vr;

    METHOD_NAME("HeapGraphNode.childs");
    METHOD_INSTANCE(HeapGraphNode_base);
    PROPERTY_ENTER();

    hr = pInst->get_childs(vr);

    METHOD_RETURN();
}
}

#endif
