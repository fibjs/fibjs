/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _DBResult_base_H_
#define _DBResult_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "List.h"

namespace fibjs
{

class List_base;

class DBResult_base : public List_base
{
public:
	// DBResult_base
	virtual result_t get_insertId(int64_t& retVal) = 0;
	virtual result_t get_affected(int64_t& retVal) = 0;
	virtual result_t get_fields(v8::Handle<v8::Array>& retVal) = 0;

	DECLARE_CLASSINFO(DBResult_base);

public:
	static v8::Handle<v8::Value> s_get_insertId(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_affected(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_fields(v8::Local<v8::String> property, const v8::AccessorInfo &info);
};

}


namespace fibjs
{
	inline ClassInfo& DBResult_base::class_info()
	{
		static ClassData::ClassProperty s_property[] = 
		{
			{"insertId", s_get_insertId},
			{"affected", s_get_affected},
			{"fields", s_get_fields}
		};

		static ClassData s_cd = 
		{ 
			"DBResult", NULL, 
			0, NULL, 0, NULL, 3, s_property, NULL, NULL,
			&List_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}

	inline v8::Handle<v8::Value> DBResult_base::s_get_insertId(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		int64_t vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(DBResult_base);

		hr = pInst->get_insertId(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> DBResult_base::s_get_affected(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		int64_t vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(DBResult_base);

		hr = pInst->get_affected(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> DBResult_base::s_get_fields(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		v8::Handle<v8::Array> vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(DBResult_base);

		hr = pInst->get_fields(vr);

		METHOD_RETURN();
	}

}

#endif

