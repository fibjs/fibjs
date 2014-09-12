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
class XmlDocumentType_base;
class XmlElement_base;
class XmlText_base;
class XmlComment_base;
class XmlCDATASection_base;
class XmlProcessingInstruction_base;
class XmlAttr_base;
class XmlNodeList_base;

class XmlDocument_base : public XmlNode_base
{
	DECLARE_CLASS(XmlDocument_base);

public:
	// XmlDocument_base
	static result_t _new(const char* source, obj_ptr<XmlDocument_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
	virtual result_t loadXML(const char* source) = 0;
	virtual result_t get_doctype(obj_ptr<XmlDocumentType_base>& retVal) = 0;
	virtual result_t get_documentElement(obj_ptr<XmlElement_base>& retVal) = 0;
	virtual result_t createElement(const char* tagName, obj_ptr<XmlElement_base>& retVal) = 0;
	virtual result_t createTextNode(const char* data, obj_ptr<XmlText_base>& retVal) = 0;
	virtual result_t createComment(const char* data, obj_ptr<XmlComment_base>& retVal) = 0;
	virtual result_t createCDATASection(const char* data, obj_ptr<XmlCDATASection_base>& retVal) = 0;
	virtual result_t createProcessingInstruction(const char* target, const char* data, obj_ptr<XmlProcessingInstruction_base>& retVal) = 0;
	virtual result_t createAttribute(const char* name, obj_ptr<XmlAttr_base>& retVal) = 0;
	virtual result_t getElementsByTagName(const char* tagName, obj_ptr<XmlNodeList_base>& retVal) = 0;
	virtual result_t getElementById(const char* elementId, obj_ptr<XmlElement_base>& retVal) = 0;

public:
	template<typename T>
	static void __new(const T &args);

public:
	static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_loadXML(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_get_doctype(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_documentElement(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_createElement(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_createTextNode(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_createComment(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_createCDATASection(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_createProcessingInstruction(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_createAttribute(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_getElementsByTagName(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_getElementById(const v8::FunctionCallbackInfo<v8::Value>& args);
};

}

#include "XmlDocumentType.h"
#include "XmlElement.h"
#include "XmlText.h"
#include "XmlComment.h"
#include "XmlCDATASection.h"
#include "XmlProcessingInstruction.h"
#include "XmlAttr.h"
#include "XmlNodeList.h"

namespace fibjs
{
	inline ClassInfo& XmlDocument_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"loadXML", s_loadXML},
			{"createElement", s_createElement},
			{"createTextNode", s_createTextNode},
			{"createComment", s_createComment},
			{"createCDATASection", s_createCDATASection},
			{"createProcessingInstruction", s_createProcessingInstruction},
			{"createAttribute", s_createAttribute},
			{"getElementsByTagName", s_getElementsByTagName},
			{"getElementById", s_getElementById}
		};

		static ClassData::ClassProperty s_property[] = 
		{
			{"doctype", s_get_doctype, block_set},
			{"documentElement", s_get_documentElement, block_set}
		};

		static ClassData s_cd = 
		{ 
			"XmlDocument", s__new, 
			9, s_method, 0, NULL, 2, s_property, NULL, NULL,
			&XmlNode_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
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

	inline void XmlDocument_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		CONSTRUCT_INIT();
		__new(args);
	}

	template<typename T>void XmlDocument_base::__new(const T& args)
	{
		obj_ptr<XmlDocument_base> vr;

		CONSTRUCT_ENTER(1, 0);

		OPT_ARG(arg_string, 0, "");

		hr = _new(v0, vr, args.This());

		CONSTRUCT_RETURN();
	}

	inline void XmlDocument_base::s_loadXML(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(XmlDocument_base);
		METHOD_ENTER(1, 1);

		ARG(arg_string, 0);

		hr = pInst->loadXML(v0);

		METHOD_VOID();
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

	inline void XmlDocument_base::s_createAttribute(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<XmlAttr_base> vr;

		METHOD_INSTANCE(XmlDocument_base);
		METHOD_ENTER(1, 1);

		ARG(arg_string, 0);

		hr = pInst->createAttribute(v0, vr);

		METHOD_RETURN();
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

	inline void XmlDocument_base::s_getElementById(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<XmlElement_base> vr;

		METHOD_INSTANCE(XmlDocument_base);
		METHOD_ENTER(1, 1);

		ARG(arg_string, 0);

		hr = pInst->getElementById(v0, vr);

		METHOD_RETURN();
	}

}

#endif

