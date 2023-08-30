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

class XmlDocument_base;
class Buffer_base;
class XmlNode_base;

class xml_base : public object_base {
    DECLARE_CLASS(xml_base);

public:
    enum {
        C_ELEMENT_NODE = 1,
        C_ATTRIBUTE_NODE = 2,
        C_TEXT_NODE = 3,
        C_CDATA_SECTION_NODE = 4,
        C_PROCESSING_INSTRUCTION_NODE = 7,
        C_COMMENT_NODE = 8,
        C_DOCUMENT_NODE = 9,
        C_DOCUMENT_TYPE_NODE = 10
    };

public:
    // xml_base
    static result_t parse(exlib::string source, exlib::string type, obj_ptr<XmlDocument_base>& retVal);
    static result_t parse(Buffer_base* source, exlib::string type, obj_ptr<XmlDocument_base>& retVal);
    static result_t serialize(XmlNode_base* node, exlib::string& retVal);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_static_parse(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_serialize(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

#include "ifs/XmlDocument.h"
#include "ifs/Buffer.h"
#include "ifs/XmlNode.h"

namespace fibjs {
inline ClassInfo& xml_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "parse", s_static_parse, true, false },
        { "serialize", s_static_serialize, true, false }
    };

    static ClassData::ClassObject s_object[] = {
        { "Document", XmlDocument_base::class_info }
    };

    static ClassData::ClassConst s_const[] = {
        { "ELEMENT_NODE", C_ELEMENT_NODE },
        { "ATTRIBUTE_NODE", C_ATTRIBUTE_NODE },
        { "TEXT_NODE", C_TEXT_NODE },
        { "CDATA_SECTION_NODE", C_CDATA_SECTION_NODE },
        { "PROCESSING_INSTRUCTION_NODE", C_PROCESSING_INSTRUCTION_NODE },
        { "COMMENT_NODE", C_COMMENT_NODE },
        { "DOCUMENT_NODE", C_DOCUMENT_NODE },
        { "DOCUMENT_TYPE_NODE", C_DOCUMENT_TYPE_NODE }
    };

    static ClassData s_cd = {
        "xml", true, s__new, NULL,
        ARRAYSIZE(s_method), s_method, ARRAYSIZE(s_object), s_object, 0, NULL, ARRAYSIZE(s_const), s_const, NULL, NULL,
        &object_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void xml_base::s_static_parse(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<XmlDocument_base> vr;

    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(exlib::string, 1, "text/xml");

    hr = parse(v0, v1, vr);

    METHOD_OVER(2, 1);

    ARG(obj_ptr<Buffer_base>, 0);
    OPT_ARG(exlib::string, 1, "text/xml");

    hr = parse(v0, v1, vr);

    METHOD_RETURN();
}

inline void xml_base::s_static_serialize(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(obj_ptr<XmlNode_base>, 0);

    hr = serialize(v0, vr);

    METHOD_RETURN();
}
}
