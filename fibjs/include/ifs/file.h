/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _file_H_
#define _file_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "if.h"

namespace fibjs
{

class file_base : public object_base
{
public:
	static const int32_t FSEEK_SET = 0;
	static const int32_t FSEEK_CUR = 1;
	static const int32_t FSEEK_END = 2;

public:
	// file_base
	virtual result_t get_data(double& retVal) = 0;
	virtual result_t set_data(double newVal) = 0;
	virtual result_t getData(double& retVal) = 0;

public:
	static ClassInfo& info()
	{
		static ClassMethod s_method[] = 
		{
			{"getData", m_getData}
		};

		static ClassProperty s_property[] = 
		{
			{"FSEEK_SET", s_get_FSEEK_SET},
			{"FSEEK_CUR", s_get_FSEEK_CUR},
			{"FSEEK_END", s_get_FSEEK_END},
			{"data", m_get_data, m_set_data}
		};

		static ClassInfo s_ci("file", 1, s_method, 4, s_property, &object_base::info());

		return s_ci;
	}

    virtual v8::Handle<v8::Value> ToJSObject()
	{
		return wrap(info());
	}

private:
	static v8::Handle<v8::Value> s_get_FSEEK_SET(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		PROPERTY_ENTER();

		int32_t vr = FSEEK_SET;

		METHOD_RETURN();
	}

	static v8::Handle<v8::Value> s_get_FSEEK_CUR(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		PROPERTY_ENTER();

		int32_t vr = FSEEK_CUR;

		METHOD_RETURN();
	}

	static v8::Handle<v8::Value> s_get_FSEEK_END(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		PROPERTY_ENTER();

		int32_t vr = FSEEK_END;

		METHOD_RETURN();
	}

	static v8::Handle<v8::Value> m_get_data(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		PROPERTY_ENTER();
		PROPERTY_INSTANCE(file_base);

		double vr;
		hr = pInst->get_data(vr);

		METHOD_RETURN();
	}

	static void m_set_data(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo &info)
	{
		PROPERTY_ENTER();
		PROPERTY_SET_INSTANCE(file_base);

		PROPERTY_VAL_number();
		hr = pInst->set_data(v0);

		PROPERTY_SET_LEAVE();
	}

	static v8::Handle<v8::Value> m_getData(const v8::Arguments& args)
	{
		METHOD_ENTER(0, 0);
		METHOD_INSTANCE(file_base);

		double vr;
		hr = pInst->getData(vr);

		METHOD_RETURN();
	}

};

}

#endif

