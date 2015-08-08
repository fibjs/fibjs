/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _XmlDocumentType_base_H_
#define _XmlDocumentType_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "XmlNode.h"

namespace fibjs
{

class XmlNode_base;

class XmlDocumentType_base : public XmlNode_base
{
    DECLARE_CLASS(XmlDocumentType_base);

public:
    // XmlDocumentType_base
    virtual result_t get_name(std::string& retVal) = 0;
    virtual result_t get_publicId(std::string& retVal) = 0;
    virtual result_t get_systemId(std::string& retVal) = 0;

public:
    static void s_get_name(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get_publicId(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get_systemId(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
};

}


namespace fibjs
{
    inline ClassInfo& XmlDocumentType_base::class_info()
    {
        static ClassData::ClassProperty s_property[] = 
        {
            {"name", s_get_name, block_set, false},
            {"publicId", s_get_publicId, block_set, false},
            {"systemId", s_get_systemId, block_set, false}
        };

        static ClassData s_cd = 
        { 
            "XmlDocumentType", NULL, 
            0, NULL, 0, NULL, 3, s_property, NULL, NULL,
            &XmlNode_base::class_info()
        };

        static ClassInfo s_ci(s_cd);
        return s_ci;
    }

    inline void XmlDocumentType_base::s_get_name(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        std::string vr;

        PROPERTY_ENTER();
        PROPERTY_INSTANCE(XmlDocumentType_base);

        hr = pInst->get_name(vr);

        METHOD_RETURN();
    }

    inline void XmlDocumentType_base::s_get_publicId(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        std::string vr;

        PROPERTY_ENTER();
        PROPERTY_INSTANCE(XmlDocumentType_base);

        hr = pInst->get_publicId(vr);

        METHOD_RETURN();
    }

    inline void XmlDocumentType_base::s_get_systemId(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        std::string vr;

        PROPERTY_ENTER();
        PROPERTY_INSTANCE(XmlDocumentType_base);

        hr = pInst->get_systemId(vr);

        METHOD_RETURN();
    }

}

#endif

