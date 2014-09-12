/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _XmlElement_base_H_
#define _XmlElement_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "XmlNode.h"

namespace fibjs
{

class XmlNode_base;
class XmlNamedNodeMap_base;
class XmlAttr_base;
class XmlNodeList_base;

class XmlElement_base : public XmlNode_base
{
	DECLARE_CLASS(XmlElement_base);

public:
	// XmlElement_base
	virtual result_t get_tagName(std::string& retVal) = 0;
	virtual result_t get_attributes(obj_ptr<XmlNamedNodeMap_base>& retVal) = 0;
	virtual result_t getAttribute(const char* name, std::string& retVal) = 0;
	virtual result_t setAttribute(const char* name, const char* value) = 0;
	virtual result_t removeAttribute(const char* name) = 0;
	virtual result_t getAttributeNode(const char* name, obj_ptr<XmlAttr_base>& retVal) = 0;
	virtual result_t setAttributeNode(XmlAttr_base* newAttr, obj_ptr<XmlAttr_base>& retVal) = 0;
	virtual result_t removeAttributeNode(XmlAttr_base* oldAttr, obj_ptr<XmlAttr_base>& retVal) = 0;
	virtual result_t getElementsByTagName(const char* tagName, obj_ptr<XmlNodeList_base>& retVal) = 0;
	virtual result_t hasAttribute(bool& retVal) = 0;

public:
	static void s_get_tagName(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_attributes(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_getAttribute(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_setAttribute(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_removeAttribute(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_getAttributeNode(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_setAttributeNode(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_removeAttributeNode(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_getElementsByTagName(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_hasAttribute(const v8::FunctionCallbackInfo<v8::Value>& args);
};

}

#include "XmlNamedNodeMap.h"
#include "XmlAttr.h"
#include "XmlNodeList.h"

namespace fibjs
{
	inline ClassInfo& XmlElement_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"getAttribute", s_getAttribute},
			{"setAttribute", s_setAttribute},
			{"removeAttribute", s_removeAttribute},
			{"getAttributeNode", s_getAttributeNode},
			{"setAttributeNode", s_setAttributeNode},
			{"removeAttributeNode", s_removeAttributeNode},
			{"getElementsByTagName", s_getElementsByTagName},
			{"hasAttribute", s_hasAttribute}
		};

		static ClassData::ClassProperty s_property[] = 
		{
			{"tagName", s_get_tagName, block_set},
			{"attributes", s_get_attributes, block_set}
		};

		static ClassData s_cd = 
		{ 
			"XmlElement", NULL, 
			8, s_method, 0, NULL, 2, s_property, NULL, NULL,
			&XmlNode_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}

	inline void XmlElement_base::s_get_tagName(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		std::string vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(XmlElement_base);

		hr = pInst->get_tagName(vr);

		METHOD_RETURN();
	}

	inline void XmlElement_base::s_get_attributes(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		obj_ptr<XmlNamedNodeMap_base> vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(XmlElement_base);

		hr = pInst->get_attributes(vr);

		METHOD_RETURN();
	}

	inline void XmlElement_base::s_getAttribute(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		std::string vr;

		METHOD_INSTANCE(XmlElement_base);
		METHOD_ENTER(1, 1);

		ARG(arg_string, 0);

		hr = pInst->getAttribute(v0, vr);

		METHOD_RETURN();
	}

	inline void XmlElement_base::s_setAttribute(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(XmlElement_base);
		METHOD_ENTER(2, 2);

		ARG(arg_string, 0);
		ARG(arg_string, 1);

		hr = pInst->setAttribute(v0, v1);

		METHOD_VOID();
	}

	inline void XmlElement_base::s_removeAttribute(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(XmlElement_base);
		METHOD_ENTER(1, 1);

		ARG(arg_string, 0);

		hr = pInst->removeAttribute(v0);

		METHOD_VOID();
	}

	inline void XmlElement_base::s_getAttributeNode(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<XmlAttr_base> vr;

		METHOD_INSTANCE(XmlElement_base);
		METHOD_ENTER(1, 1);

		ARG(arg_string, 0);

		hr = pInst->getAttributeNode(v0, vr);

		METHOD_RETURN();
	}

	inline void XmlElement_base::s_setAttributeNode(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<XmlAttr_base> vr;

		METHOD_INSTANCE(XmlElement_base);
		METHOD_ENTER(1, 1);

		ARG(obj_ptr<XmlAttr_base>, 0);

		hr = pInst->setAttributeNode(v0, vr);

		METHOD_RETURN();
	}

	inline void XmlElement_base::s_removeAttributeNode(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<XmlAttr_base> vr;

		METHOD_INSTANCE(XmlElement_base);
		METHOD_ENTER(1, 1);

		ARG(obj_ptr<XmlAttr_base>, 0);

		hr = pInst->removeAttributeNode(v0, vr);

		METHOD_RETURN();
	}

	inline void XmlElement_base::s_getElementsByTagName(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<XmlNodeList_base> vr;

		METHOD_INSTANCE(XmlElement_base);
		METHOD_ENTER(1, 1);

		ARG(arg_string, 0);

		hr = pInst->getElementsByTagName(v0, vr);

		METHOD_RETURN();
	}

	inline void XmlElement_base::s_hasAttribute(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		bool vr;

		METHOD_INSTANCE(XmlElement_base);
		METHOD_ENTER(0, 0);

		hr = pInst->hasAttribute(vr);

		METHOD_RETURN();
	}

}

#endif

