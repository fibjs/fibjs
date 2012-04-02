/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _file_base_H_
#define _file_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "if.h"
#include "stream.h"

namespace fibjs
{

class stream_base;

class file_base : public stream_base
{
public:
	// file_base
	virtual result_t get_data(double& retVal) = 0;
	virtual result_t set_data(double newVal) = 0;
	virtual result_t getData(double& retVal) = 0;

public:
	static ClassInfo& class_info()
	{
		static ClassMethod s_method[] = 
		{
			{"getData", s_getData}
		};

		static ClassProperty s_property[] = 
		{
			{"data", s_get_data, s_set_data}
		};

		static ClassData s_cd = 
		{ 
			"file", NULL, 
			1, s_method, 1, s_property, NULL,
			&stream_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}

	virtual ClassInfo& Classinfo()
	{
		return class_info();
	}

private:
	static v8::Handle<v8::Value> s_get_data(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static void s_set_data(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_getData(const v8::Arguments& args);
};

}


namespace fibjs
{
	inline v8::Handle<v8::Value> file_base::s_get_data(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		PROPERTY_ENTER();
		PROPERTY_INSTANCE(file_base);

		double vr;
		hr = pInst->get_data(vr);

		METHOD_RETURN();
	}

	inline void file_base::s_set_data(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo &info)
	{
		PROPERTY_ENTER();
		PROPERTY_INSTANCE(file_base);

		PROPERTY_VAL(double);
		hr = pInst->set_data(v0);

		PROPERTY_SET_LEAVE();
	}

	inline v8::Handle<v8::Value> file_base::s_getData(const v8::Arguments& args)
	{
		METHOD_ENTER(0, 0);
		METHOD_INSTANCE(file_base);

		double vr;
		hr = pInst->getData(vr);

		METHOD_RETURN();
	}

}

#endif

