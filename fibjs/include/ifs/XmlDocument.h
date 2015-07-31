/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _XmlDocument_base_H_
#define _XmlDocument_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "XmlNode.h"

namespace fibjs
{

class XmlNode_base;
class Buffer_base;
class XmlDocumentType_base;
class XmlElement_base;
class XmlNodeList_base;
class XmlText_base;
class XmlComment_base;
class XmlCDATASection_base;
class XmlProcessingInstruction_base;

class XmlDocument_base : public XmlNode_base
{
	DECLARE_CLASS(XmlDocument_base);

public:
	// XmlDocument_base
	static result_t _new(const char* type, obj_ptr<XmlDocument_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
	virtual result_t load(const char* source) = 0;
	virtual result_t load(Buffer_base* source) = 0;
	virtual result_t get_inputEncoding(std::string& retVal) = 0;
	virtual result_t get_xmlStandalone(bool& retVal) = 0;
	virtual result_t set_xmlStandalone(bool newVal) = 0;
	virtual result_t get_xmlVersion(std::string& retVal) = 0;
	virtual result_t set_xmlVersion(const char* newVal) = 0;
	virtual result_t get_doctype(obj_ptr<XmlDocumentType_base>& retVal) = 0;
	virtual result_t get_documentElement(obj_ptr<XmlElement_base>& retVal) = 0;
	virtual result_t get_head(obj_ptr<XmlElement_base>& retVal) = 0;
	virtual result_t get_title(std::string& retVal) = 0;
	virtual result_t get_body(obj_ptr<XmlElement_base>& retVal) = 0;
	virtual result_t getElementsByTagName(const char* tagName, obj_ptr<XmlNodeList_base>& retVal) = 0;
	virtual result_t getElementsByTagNameNS(const char* namespaceURI, const char* localName, obj_ptr<XmlNodeList_base>& retVal) = 0;
	virtual result_t createElement(const char* tagName, obj_ptr<XmlElement_base>& retVal) = 0;
	virtual result_t createElementNS(const char* namespaceURI, const char* qualifiedName, obj_ptr<XmlElement_base>& retVal) = 0;
	virtual result_t createTextNode(const char* data, obj_ptr<XmlText_base>& retVal) = 0;
	virtual result_t createComment(const char* data, obj_ptr<XmlComment_base>& retVal) = 0;
	virtual result_t createCDATASection(const char* data, obj_ptr<XmlCDATASection_base>& retVal) = 0;
	virtual result_t createProcessingInstruction(const char* target, const char* data, obj_ptr<XmlProcessingInstruction_base>& retVal) = 0;

public:
	template<typename T>
	static void __new(const T &args);

public:
	static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_load(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_get_inputEncoding(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_xmlStandalone(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_set_xmlStandalone(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args);
	static void s_get_xmlVersion(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_set_xmlVersion(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args);
	static void s_get_doctype(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_documentElement(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_head(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_title(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_body(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_getElementsByTagName(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_getElementsByTagNameNS(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_createElement(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_createElementNS(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_createTextNode(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_createComment(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_createCDATASection(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_createProcessingInstruction(const v8::FunctionCallbackInfo<v8::Value>& args);
};

}

#include "Buffer.h"
#include "XmlDocumentType.h"
#include "XmlElement.h"
#include "XmlNodeList.h"
#include "XmlText.h"
#include "XmlComment.h"
#include "XmlCDATASection.h"
#include "XmlProcessingInstruction.h"

namespace fibjs
{
	inline ClassInfo& XmlDocument_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"load", s_load, false},
			{"getElementsByTagName", s_getElementsByTagName, false},
			{"getElementsByTagNameNS", s_getElementsByTagNameNS, false},
			{"createElement", s_createElement, false},
			{"createElementNS", s_createElementNS, false},
			{"createTextNode", s_createTextNode, false},
			{"createComment", s_createComment, false},
			{"createCDATASection", s_createCDATASection, false},
			{"createProcessingInstruction", s_createProcessingInstruction, false}
		};

		static ClassData::ClassProperty s_property[] = 
		{
			{"inputEncoding", s_get_inputEncoding, block_set, false},
			{"xmlStandalone", s_get_xmlStandalone, s_set_xmlStandalone, false},
			{"xmlVersion", s_get_xmlVersion, s_set_xmlVersion, false},
			{"doctype", s_get_doctype, block_set, false},
			{"documentElement", s_get_documentElement, block_set, false},
			{"head", s_get_head, block_set, false},
			{"title", s_get_title, block_set, false},
			{"body", s_get_body, block_set, false}
		};

		static ClassData s_cd = 
		{ 
			"XmlDocument", s__new, 
			9, s_method, 0, NULL, 8, s_property, NULL, NULL,
			&XmlNode_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}

	inline void XmlDocument_base::s_get_inputEncoding(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		std::string vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(XmlDocument_base);

		hr = pInst->get_inputEncoding(vr);

		METHOD_RETURN();
	}

	inline void XmlDocument_base::s_get_xmlStandalone(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		bool vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(XmlDocument_base);

		hr = pInst->get_xmlStandalone(vr);

		METHOD_RETURN();
	}

	inline void XmlDocument_base::s_set_xmlStandalone(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args)
	{
		PROPERTY_ENTER();
		PROPERTY_INSTANCE(XmlDocument_base);

		PROPERTY_VAL(bool);
		hr = pInst->set_xmlStandalone(v0);

		PROPERTY_SET_LEAVE();
	}

	inline void XmlDocument_base::s_get_xmlVersion(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		std::string vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(XmlDocument_base);

		hr = pInst->get_xmlVersion(vr);

		METHOD_RETURN();
	}

	inline void XmlDocument_base::s_set_xmlVersion(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args)
	{
		PROPERTY_ENTER();
		PROPERTY_INSTANCE(XmlDocument_base);

		PROPERTY_VAL(arg_string);
		hr = pInst->set_xmlVersion(v0);

		PROPERTY_SET_LEAVE();
	}

	inline void XmlDocument_base::s_get_doctype(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		obj_ptr<XmlDocumentType_base> vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(XmlDocument_base);

		hr = pInst->get_doctype(vr);

		METHOD_RETURN();
	}

	inline void XmlDocument_base::s_get_documentElement(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		obj_ptr<XmlElement_base> vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(XmlDocument_base);

		hr = pInst->get_documentElement(vr);

		METHOD_RETURN();
	}

	inline void XmlDocument_base::s_get_head(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		obj_ptr<XmlElement_base> vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(XmlDocument_base);

		hr = pInst->get_head(vr);

		METHOD_RETURN();
	}

	inline void XmlDocument_base::s_get_title(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		std::string vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(XmlDocument_base);

		hr = pInst->get_title(vr);

		METHOD_RETURN();
	}

	inline void XmlDocument_base::s_get_body(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		obj_ptr<XmlElement_base> vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(XmlDocument_base);

		hr = pInst->get_body(vr);

		METHOD_RETURN();
	}

	inline void XmlDocument_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		CONSTRUCT_INIT();
		__new(args);
	}

	template<typename T>void XmlDocument_base::__new(const T& args)
	{
		obj_ptr<XmlDocument_base> vr;

		CONSTRUCT_ENTER(1, 0);

		OPT_ARG(arg_string, 0, "text/xml");

		hr = _new(v0, vr, args.This());

		CONSTRUCT_RETURN();
	}

	inline void XmlDocument_base::s_load(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(XmlDocument_base);
		METHOD_ENTER(1, 1);

		ARG(arg_string, 0);

		hr = pInst->load(v0);

		METHOD_OVER(1, 1);

		ARG(obj_ptr<Buffer_base>, 0);

		hr = pInst->load(v0);

		METHOD_VOID();
	}

	inline void XmlDocument_base::s_getElementsByTagName(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<XmlNodeList_base> vr;

		METHOD_INSTANCE(XmlDocument_base);
		METHOD_ENTER(1, 1);

		ARG(arg_string, 0);

		hr = pInst->getElementsByTagName(v0, vr);

		METHOD_RETURN();
	}

	inline void XmlDocument_base::s_getElementsByTagNameNS(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<XmlNodeList_base> vr;

		METHOD_INSTANCE(XmlDocument_base);
		METHOD_ENTER(2, 2);

		ARG(arg_string, 0);
		ARG(arg_string, 1);

		hr = pInst->getElementsByTagNameNS(v0, v1, vr);

		METHOD_RETURN();
	}

	inline void XmlDocument_base::s_createElement(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<XmlElement_base> vr;

		METHOD_INSTANCE(XmlDocument_base);
		METHOD_ENTER(1, 1);

		ARG(arg_string, 0);

		hr = pInst->createElement(v0, vr);

		METHOD_RETURN();
	}

	inline void XmlDocument_base::s_createElementNS(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<XmlElement_base> vr;

		METHOD_INSTANCE(XmlDocument_base);
		METHOD_ENTER(2, 2);

		ARG(arg_string, 0);
		ARG(arg_string, 1);

		hr = pInst->createElementNS(v0, v1, vr);

		METHOD_RETURN();
	}

	inline void XmlDocument_base::s_createTextNode(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<XmlText_base> vr;

		METHOD_INSTANCE(XmlDocument_base);
		METHOD_ENTER(1, 1);

		ARG(arg_string, 0);

		hr = pInst->createTextNode(v0, vr);

		METHOD_RETURN();
	}

	inline void XmlDocument_base::s_createComment(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<XmlComment_base> vr;

		METHOD_INSTANCE(XmlDocument_base);
		METHOD_ENTER(1, 1);

		ARG(arg_string, 0);

		hr = pInst->createComment(v0, vr);

		METHOD_RETURN();
	}

	inline void XmlDocument_base::s_createCDATASection(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<XmlCDATASection_base> vr;

		METHOD_INSTANCE(XmlDocument_base);
		METHOD_ENTER(1, 1);

		ARG(arg_string, 0);

		hr = pInst->createCDATASection(v0, vr);

		METHOD_RETURN();
	}

	inline void XmlDocument_base::s_createProcessingInstruction(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<XmlProcessingInstruction_base> vr;

		METHOD_INSTANCE(XmlDocument_base);
		METHOD_ENTER(2, 2);

		ARG(arg_string, 0);
		ARG(arg_string, 1);

		hr = pInst->createProcessingInstruction(v0, v1, vr);

		METHOD_RETURN();
	}

}

#endif

