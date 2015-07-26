/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
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
	DECLARE_CLASS(SQLite_base);

public:
	// SQLite_base
	virtual result_t get_fileName(std::string& retVal) = 0;
	virtual result_t get_timeout(int32_t& retVal) = 0;
	virtual result_t set_timeout(int32_t newVal) = 0;
	virtual result_t backup(const char* fileName, AsyncEvent* ac) = 0;

public:
	static void s_get_fileName(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_timeout(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_set_timeout(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args);
	static void s_backup(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
	ASYNC_MEMBER1(SQLite_base, backup, const char*);
};

}


namespace fibjs
{
	inline ClassInfo& SQLite_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"backup", s_backup, false}
		};

		static ClassData::ClassProperty s_property[] = 
		{
			{"fileName", s_get_fileName, block_set},
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

	inline void SQLite_base::s_get_fileName(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		std::string vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(SQLite_base);

		hr = pInst->get_fileName(vr);

		METHOD_RETURN();
	}

	inline void SQLite_base::s_get_timeout(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(SQLite_base);

		hr = pInst->get_timeout(vr);

		METHOD_RETURN();
	}

	inline void SQLite_base::s_set_timeout(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args)
	{
		PROPERTY_ENTER();
		PROPERTY_INSTANCE(SQLite_base);

		PROPERTY_VAL(int32_t);
		hr = pInst->set_timeout(v0);

		PROPERTY_SET_LEAVE();
	}

	inline void SQLite_base::s_backup(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(SQLite_base);
		METHOD_ENTER(1, 1);

		ARG(arg_string, 0);

		hr = pInst->ac_backup(v0);

		METHOD_VOID();
	}

}

#endif

