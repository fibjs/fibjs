/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _XmlProcessingInstruction_base_H_
#define _XmlProcessingInstruction_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "XmlNode.h"

namespace fibjs
{

class XmlNode_base;

class XmlProcessingInstruction_base : public XmlNode_base
{
    DECLARE_CLASS(XmlProcessingInstruction_base);

public:
    // XmlProcessingInstruction_base
    virtual result_t get_target(std::string& retVal) = 0;
    virtual result_t get_data(std::string& retVal) = 0;
    virtual result_t set_data(const char* newVal) = 0;

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        Isolate* isolate = Isolate::current();

        isolate->m_isolate->ThrowException(
            isolate->NewFromUtf8("not a constructor"));
    }

public:
    static void s_get_target(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get_data(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_set_data(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args);
};

}


namespace fibjs
{
    inline ClassInfo& XmlProcessingInstruction_base::class_info()
    {
        static ClassData::ClassProperty s_property[] = 
        {
            {"target", s_get_target, block_set, false},
            {"data", s_get_data, s_set_data, false}
        };

        static ClassData s_cd = 
        { 
            "XmlProcessingInstruction", s__new, NULL, 
            0, NULL, 0, NULL, 2, s_property, NULL, NULL,
            &XmlNode_base::class_info()
        };

        static ClassInfo s_ci(s_cd);
        return s_ci;
    }

    inline void XmlProcessingInstruction_base::s_get_target(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        std::string vr;

        PROPERTY_ENTER();
        PROPERTY_INSTANCE(XmlProcessingInstruction_base);

        hr = pInst->get_target(vr);

        METHOD_RETURN();
    }

    inline void XmlProcessingInstruction_base::s_get_data(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        std::string vr;

        PROPERTY_ENTER();
        PROPERTY_INSTANCE(XmlProcessingInstruction_base);

        hr = pInst->get_data(vr);

        METHOD_RETURN();
    }

    inline void XmlProcessingInstruction_base::s_set_data(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args)
    {
        PROPERTY_ENTER();
        PROPERTY_INSTANCE(XmlProcessingInstruction_base);

        PROPERTY_VAL(arg_string);
        hr = pInst->set_data(v0);

        PROPERTY_SET_LEAVE();
    }

}

#endif

