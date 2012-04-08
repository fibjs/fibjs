/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _Stat_base_H_
#define _Stat_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs
{

class Stat_base : public object_base
{
public:
	// Stat_base
	virtual result_t get_size(double& retVal) = 0;
	virtual result_t get_mtime(int64_t& retVal) = 0;
	virtual result_t get_atime(int64_t& retVal) = 0;
	virtual result_t get_ctime(int64_t& retVal) = 0;

public:
	static ClassInfo& class_info()
	{
		static ClassProperty s_property[] = 
		{
			{"size", s_get_size},
			{"mtime", s_get_mtime},
			{"atime", s_get_atime},
			{"ctime", s_get_ctime}
		};

		static ClassData s_cd = 
		{ 
			"Stat", NULL, 
			0, NULL, 4, s_property, NULL,
			&object_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}

	virtual ClassInfo& Classinfo()
	{
		return class_info();
	}

	virtual result_t toJSON(const char* key, v8::Handle<v8::Object>& retVal)
	{
		result_t hr = object_base::toJSON(key, retVal);
		if(hr < 0)return hr;

		CLONE(size, double);
		CLONE(mtime, int64_t);
		CLONE(atime, int64_t);
		CLONE(ctime, int64_t);

		return 0;
	}

private:
	static v8::Handle<v8::Value> s_get_size(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_mtime(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_atime(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_ctime(v8::Local<v8::String> property, const v8::AccessorInfo &info);
};

}

namespace fibjs
{
	inline v8::Handle<v8::Value> Stat_base::s_get_size(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Stat_base);

		double vr;
		hr = pInst->get_size(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> Stat_base::s_get_mtime(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Stat_base);

		int64_t vr;
		hr = pInst->get_mtime(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> Stat_base::s_get_atime(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Stat_base);

		int64_t vr;
		hr = pInst->get_atime(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> Stat_base::s_get_ctime(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Stat_base);

		int64_t vr;
		hr = pInst->get_ctime(vr);

		METHOD_RETURN();
	}

}

#endif

