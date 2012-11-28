/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _MySQL_base_H_
#define _MySQL_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "DbConnection.h"

namespace fibjs
{

class DbConnection_base;

class MySQL_base : public DbConnection_base
{
public:
	// MySQL_base
	virtual result_t use(const char* dbName) = 0;
	virtual result_t get_rxBufferSize(int32_t& retVal) = 0;
	virtual result_t set_rxBufferSize(int32_t newVal) = 0;
	virtual result_t get_txBufferSize(int32_t& retVal) = 0;
	virtual result_t set_txBufferSize(int32_t newVal) = 0;

	DECLARE_CLASSINFO(MySQL_base);

public:
	static v8::Handle<v8::Value> s_use(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_get_rxBufferSize(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static void s_set_rxBufferSize(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_txBufferSize(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static void s_set_txBufferSize(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo &info);
};

}


namespace fibjs
{
	inline ClassInfo& MySQL_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"use", s_use}
		};

		static ClassData::ClassProperty s_property[] = 
		{
			{"rxBufferSize", s_get_rxBufferSize, s_set_rxBufferSize},
			{"txBufferSize", s_get_txBufferSize, s_set_txBufferSize}
		};

		static ClassData s_cd = 
		{ 
			"MySQL", NULL, 
			1, s_method, 0, NULL, 2, s_property, NULL, NULL,
			&DbConnection_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}

	inline v8::Handle<v8::Value> MySQL_base::s_get_rxBufferSize(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		int32_t vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(MySQL_base);

		hr = pInst->get_rxBufferSize(vr);

		METHOD_RETURN();
	}

	inline void MySQL_base::s_set_rxBufferSize(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo &info)
	{
		PROPERTY_ENTER();
		PROPERTY_INSTANCE(MySQL_base);

		PROPERTY_VAL(int32_t);
		hr = pInst->set_rxBufferSize(v0);

		PROPERTY_SET_LEAVE();
	}

	inline v8::Handle<v8::Value> MySQL_base::s_get_txBufferSize(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		int32_t vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(MySQL_base);

		hr = pInst->get_txBufferSize(vr);

		METHOD_RETURN();
	}

	inline void MySQL_base::s_set_txBufferSize(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo &info)
	{
		PROPERTY_ENTER();
		PROPERTY_INSTANCE(MySQL_base);

		PROPERTY_VAL(int32_t);
		hr = pInst->set_txBufferSize(v0);

		PROPERTY_SET_LEAVE();
	}

	inline v8::Handle<v8::Value> MySQL_base::s_use(const v8::Arguments& args)
	{
		METHOD_INSTANCE(MySQL_base);
		METHOD_ENTER(1, 1);

		ARG_String(0);

		hr = pInst->use(v0);

		METHOD_VOID();
	}

}

#endif

