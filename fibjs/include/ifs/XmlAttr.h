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
#include "XmlNode.h"

namespace fibjs
{

class XmlNode_base;

class XmlAttr_base : public XmlNode_base
{
	DECLARE_CLASS(XmlAttr_base);

public:
	// XmlAttr_base
	virtual result_t get_name(std::string& retVal) = 0;
	virtual result_t get_specified(bool& retVal) = 0;
	virtual result_t get_value(std::string& retVal) = 0;
	virtual result_t set_value(const char* newVal) = 0;

public:
	static void s_get_name(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_specified(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_value(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_set_value(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args);
};

}


namespace fibjs
{
	inline ClassInfo& XmlAttr_base::class_info()
	{
		static ClassData::ClassProperty s_property[] = 
		{
			{"name", s_get_name, block_set},
			{"specified", s_get_specified, block_set},
			{"value", s_get_value, s_set_value}
		};

		static ClassData s_cd = 
		{ 
			"XmlAttr", NULL, 
			0, NULL, 0, NULL, 3, s_property, NULL, NULL,
			&XmlNode_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}

	inline void XmlAttr_base::s_get_name(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		std::string vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(XmlAttr_base);

		hr = pInst->get_name(vr);

		METHOD_RETURN();
	}

	inline void XmlAttr_base::s_get_specified(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		bool vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(XmlAttr_base);

		hr = pInst->get_specified(vr);

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

}

#endif

