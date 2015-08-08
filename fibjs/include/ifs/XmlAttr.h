/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _XmlAttr_base_H_
#define _XmlAttr_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs
{

class XmlAttr_base : public object_base
{
    DECLARE_CLASS(XmlAttr_base);

public:
    // XmlAttr_base
    virtual result_t get_localName(std::string& retVal) = 0;
    virtual result_t get_value(std::string& retVal) = 0;
    virtual result_t set_value(const char* newVal) = 0;
    virtual result_t get_name(std::string& retVal) = 0;
    virtual result_t get_namespaceURI(std::string& retVal) = 0;
    virtual result_t get_prefix(std::string& retVal) = 0;
    virtual result_t set_prefix(const char* newVal) = 0;
    virtual result_t get_nodeName(std::string& retVal) = 0;
    virtual result_t get_nodeValue(std::string& retVal) = 0;
    virtual result_t set_nodeValue(const char* newVal) = 0;

public:
    static void s_get_localName(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get_value(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_set_value(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args);
    static void s_get_name(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get_namespaceURI(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get_prefix(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_set_prefix(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args);
    static void s_get_nodeName(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get_nodeValue(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_set_nodeValue(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args);
};

}

namespace fibjs
{
    inline ClassInfo& XmlAttr_base::class_info()
    {
        static ClassData::ClassProperty s_property[] = 
        {
            {"localName", s_get_localName, block_set, false},
            {"value", s_get_value, s_set_value, false},
            {"name", s_get_name, block_set, false},
            {"namespaceURI", s_get_namespaceURI, block_set, false},
            {"prefix", s_get_prefix, s_set_prefix, false},
            {"nodeName", s_get_nodeName, block_set, false},
            {"nodeValue", s_get_nodeValue, s_set_nodeValue, false}
        };

        static ClassData s_cd = 
        { 
            "XmlAttr", NULL, 
            0, NULL, 0, NULL, 7, s_property, NULL, NULL,
            &object_base::class_info()
        };

        static ClassInfo s_ci(s_cd);
        return s_ci;
    }

    inline void XmlAttr_base::s_get_localName(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        std::string vr;

        PROPERTY_ENTER();
        PROPERTY_INSTANCE(XmlAttr_base);

        hr = pInst->get_localName(vr);

        METHOD_RETURN();
    }

    inline void XmlAttr_base::s_get_value(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        std::string vr;

        PROPERTY_ENTER();
        PROPERTY_INSTANCE(XmlAttr_base);

        hr = pInst->get_value(vr);

        METHOD_RETURN();
    }

    inline void XmlAttr_base::s_set_value(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args)
    {
        PROPERTY_ENTER();
        PROPERTY_INSTANCE(XmlAttr_base);

        PROPERTY_VAL(arg_string);
        hr = pInst->set_value(v0);

        PROPERTY_SET_LEAVE();
    }

    inline void XmlAttr_base::s_get_name(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        std::string vr;

        PROPERTY_ENTER();
        PROPERTY_INSTANCE(XmlAttr_base);

        hr = pInst->get_name(vr);

        METHOD_RETURN();
    }

    inline void XmlAttr_base::s_get_namespaceURI(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        std::string vr;

        PROPERTY_ENTER();
        PROPERTY_INSTANCE(XmlAttr_base);

        hr = pInst->get_namespaceURI(vr);

        METHOD_RETURN();
    }

    inline void XmlAttr_base::s_get_prefix(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        std::string vr;

        PROPERTY_ENTER();
        PROPERTY_INSTANCE(XmlAttr_base);

        hr = pInst->get_prefix(vr);

        METHOD_RETURN();
    }

    inline void XmlAttr_base::s_set_prefix(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args)
    {
        PROPERTY_ENTER();
        PROPERTY_INSTANCE(XmlAttr_base);

        PROPERTY_VAL(arg_string);
        hr = pInst->set_prefix(v0);

        PROPERTY_SET_LEAVE();
    }

    inline void XmlAttr_base::s_get_nodeName(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        std::string vr;

        PROPERTY_ENTER();
        PROPERTY_INSTANCE(XmlAttr_base);

        hr = pInst->get_nodeName(vr);

        METHOD_RETURN();
    }

    inline void XmlAttr_base::s_get_nodeValue(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        std::string vr;

        PROPERTY_ENTER();
        PROPERTY_INSTANCE(XmlAttr_base);

        hr = pInst->get_nodeValue(vr);

        METHOD_RETURN();
    }

    inline void XmlAttr_base::s_set_nodeValue(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args)
    {
        PROPERTY_ENTER();
        PROPERTY_INSTANCE(XmlAttr_base);

        PROPERTY_VAL(arg_string);
        hr = pInst->set_nodeValue(v0);

        PROPERTY_SET_LEAVE();
    }

}

#endif

