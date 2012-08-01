/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _SQLite_base_H_
#define _SQLite_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "DbConnection.h"

namespace fibjs
{

class DbConnection_base;

class SQLite_base : public DbConnection_base
{
public:
	// SQLite_base
	virtual result_t get_fileName(std::string& retVal) = 0;
	virtual result_t get_timeout(int32_t& retVal) = 0;
	virtual result_t set_timeout(int32_t newVal) = 0;
	virtual result_t backup(const char* fileName) = 0;

public:
	static ClassInfo& class_info();

	virtual ClassInfo& Classinfo()
	{
		return class_info();
	}

	virtual result_t toJSON(const char* key, v8::Handle<v8::Object>& retVal)
	{
		result_t hr = DbConnection_base::toJSON(key, retVal);
		if(hr < 0)return hr;

		CLONE_String(fileName);
		CLONE(timeout, int32_t);

		return 0;
	}

public:
	static v8::Handle<v8::Value> s_get_fileName(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_timeout(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static void s_set_timeout(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_backup(const v8::Arguments& args);
};

}


namespace fibjs
{
	inline ClassInfo& SQLite_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"backup", s_backup}
		};

		static ClassData::ClassProperty s_property[] = 
		{
			{"fileName", s_get_fileName},
			{"timeout", s_get_timeout, s_set_timeout}
		};

		static ClassData s_cd = 
		{ 
			"SQLite", NULL, 
			1, s_method, 0, NULL, 2, s_property, NULL, NULL,
			&DbConnection_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}

	inline v8::Handle<v8::Value> SQLite_base::s_get_fileName(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		std::string vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(SQLite_base);

		hr = pInst->get_fileName(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> SQLite_base::s_get_timeout(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		int32_t vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(SQLite_base);

		hr = pInst->get_timeout(vr);

		METHOD_RETURN();
	}

	inline void SQLite_base::s_set_timeout(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo &info)
	{
		PROPERTY_ENTER();
		PROPERTY_INSTANCE(SQLite_base);

		PROPERTY_VAL(int32_t);
		hr = pInst->set_timeout(v0);

		PROPERTY_SET_LEAVE();
	}

	inline v8::Handle<v8::Value> SQLite_base::s_backup(const v8::Arguments& args)
	{
		METHOD_INSTANCE(SQLite_base);
		METHOD_ENTER(1, 1);

		ARG_String(0);

		hr = pInst->backup(v0);

		METHOD_VOID();
	}

}

#endif

