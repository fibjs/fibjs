/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _xml_base_H_
#define _xml_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs
{

class XmlDocument_base;
class Buffer_base;
class XmlNode_base;

class xml_base : public object_base
{
	DECLARE_CLASS(xml_base);

public:
	enum{
		_ELEMENT_NODE = 1,
		_ATTRIBUTE_NODE = 2,
		_TEXT_NODE = 3,
		_CDATA_SECTION_NODE = 4,
		_PROCESSING_INSTRUCTION_NODE = 7,
		_COMMENT_NODE = 8,
		_DOCUMENT_NODE = 9,
		_DOCUMENT_TYPE_NODE = 10
	};

public:
	// xml_base
	static result_t parse(const char* source, const char* type, obj_ptr<XmlDocument_base>& retVal);
	static result_t parse(Buffer_base* source, const char* type, obj_ptr<XmlDocument_base>& retVal);
	static result_t serialize(XmlNode_base* node, std::string& retVal);

public:
	static void s_get_ELEMENT_NODE(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_ATTRIBUTE_NODE(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_TEXT_NODE(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_CDATA_SECTION_NODE(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_PROCESSING_INSTRUCTION_NODE(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_COMMENT_NODE(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_DOCUMENT_NODE(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_DOCUMENT_TYPE_NODE(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_parse(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_serialize(const v8::FunctionCallbackInfo<v8::Value>& args);
};

}

#include "XmlDocument.h"
#include "Buffer.h"
#include "XmlNode.h"

namespace fibjs
{
	inline ClassInfo& xml_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"parse", s_parse},
			{"serialize", s_serialize}
		};

		static ClassData::ClassObject s_object[] = 
		{
			{"Document", XmlDocument_base::class_info}
		};

		static ClassData::ClassProperty s_property[] = 
		{
			{"ELEMENT_NODE", s_get_ELEMENT_NODE, block_set},
			{"ATTRIBUTE_NODE", s_get_ATTRIBUTE_NODE, block_set},
			{"TEXT_NODE", s_get_TEXT_NODE, block_set},
			{"CDATA_SECTION_NODE", s_get_CDATA_SECTION_NODE, block_set},
			{"PROCESSING_INSTRUCTION_NODE", s_get_PROCESSING_INSTRUCTION_NODE, block_set},
			{"COMMENT_NODE", s_get_COMMENT_NODE, block_set},
			{"DOCUMENT_NODE", s_get_DOCUMENT_NODE, block_set},
			{"DOCUMENT_TYPE_NODE", s_get_DOCUMENT_TYPE_NODE, block_set}
		};

		static ClassData s_cd = 
		{ 
			"xml", NULL, 
			2, s_method, 1, s_object, 8, s_property, NULL, NULL,
			NULL
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}

	inline void xml_base::s_get_ELEMENT_NODE(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr = _ELEMENT_NODE;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline void xml_base::s_get_ATTRIBUTE_NODE(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr = _ATTRIBUTE_NODE;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline void xml_base::s_get_TEXT_NODE(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr = _TEXT_NODE;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline void xml_base::s_get_CDATA_SECTION_NODE(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr = _CDATA_SECTION_NODE;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline void xml_base::s_get_PROCESSING_INSTRUCTION_NODE(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr = _PROCESSING_INSTRUCTION_NODE;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline void xml_base::s_get_COMMENT_NODE(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr = _COMMENT_NODE;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline void xml_base::s_get_DOCUMENT_NODE(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr = _DOCUMENT_NODE;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline void xml_base::s_get_DOCUMENT_TYPE_NODE(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr = _DOCUMENT_TYPE_NODE;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline void xml_base::s_parse(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<XmlDocument_base> vr;

		METHOD_ENTER(2, 1);

		ARG(arg_string, 0);
		OPT_ARG(arg_string, 1, "text/xml");

		hr = parse(v0, v1, vr);

		METHOD_OVER(2, 1);

		ARG(obj_ptr<Buffer_base>, 0);
		OPT_ARG(arg_string, 1, "text/xml");

		hr = parse(v0, v1, vr);

		METHOD_RETURN();
	}

	inline void xml_base::s_serialize(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		std::string vr;

		METHOD_ENTER(1, 1);

		ARG(obj_ptr<XmlNode_base>, 0);

		hr = serialize(v0, vr);

		METHOD_RETURN();
	}

}

#endif

