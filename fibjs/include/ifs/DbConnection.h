/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _DbConnection_base_H_
#define _DbConnection_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs
{

class DBResult_base;

class DbConnection_base : public object_base
{
public:
	// DbConnection_base
	virtual result_t close(exlib::AsyncEvent* ac) = 0;
	virtual result_t begin(exlib::AsyncEvent* ac) = 0;
	virtual result_t commit(exlib::AsyncEvent* ac) = 0;
	virtual result_t rollback(exlib::AsyncEvent* ac) = 0;
	virtual result_t execute(const char* sql, obj_ptr<DBResult_base>& retVal, exlib::AsyncEvent* ac) = 0;
	virtual result_t execute(const char* sql, const v8::FunctionCallbackInfo<v8::Value>& args, obj_ptr<DBResult_base>& retVal) = 0;
	virtual result_t format(const char* sql, const v8::FunctionCallbackInfo<v8::Value>& args, std::string& retVal) = 0;

	DECLARE_CLASSINFO(DbConnection_base);

public:
	template<typename T>
	static void __new(const T &args){}

public:
	static void s_close(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_begin(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_commit(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_rollback(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_execute(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_format(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
	ASYNC_MEMBER0(DbConnection_base, close);
	ASYNC_MEMBER0(DbConnection_base, begin);
	ASYNC_MEMBER0(DbConnection_base, commit);
	ASYNC_MEMBER0(DbConnection_base, rollback);
	ASYNC_MEMBERVALUE2(DbConnection_base, execute, const char*, obj_ptr<DBResult_base>);
};

}

#include "DBResult.h"

namespace fibjs
{
	inline ClassInfo& DbConnection_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"close", s_close},
			{"begin", s_begin},
			{"commit", s_commit},
			{"rollback", s_rollback},
			{"execute", s_execute},
			{"format", s_format}
		};

		static ClassData s_cd = 
		{ 
			"DbConnection", NULL, 
			6, s_method, 0, NULL, 0, NULL, NULL, NULL,
			&object_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}


	inline void DbConnection_base::s_close(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(DbConnection_base);
		METHOD_ENTER(0, 0);

		hr = pInst->ac_close();

		METHOD_VOID();
	}

	inline void DbConnection_base::s_begin(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(DbConnection_base);
		METHOD_ENTER(0, 0);

		hr = pInst->ac_begin();

		METHOD_VOID();
	}

	inline void DbConnection_base::s_commit(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(DbConnection_base);
		METHOD_ENTER(0, 0);

		hr = pInst->ac_commit();

		METHOD_VOID();
	}

	inline void DbConnection_base::s_rollback(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(DbConnection_base);
		METHOD_ENTER(0, 0);

		hr = pInst->ac_rollback();

		METHOD_VOID();
	}

	inline void DbConnection_base::s_execute(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<DBResult_base> vr;

		METHOD_INSTANCE(DbConnection_base);
		METHOD_ENTER(1, 1);

		ARG(arg_string, 0);

		hr = pInst->ac_execute(v0, vr);

		METHOD_OVER(-1, 1);

		ARG(arg_string, 0);

		hr = pInst->execute(v0, args, vr);

		METHOD_RETURN();
	}

	inline void DbConnection_base::s_format(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		std::string vr;

		METHOD_INSTANCE(DbConnection_base);
		METHOD_ENTER(-1, 1);

		ARG(arg_string, 0);

		hr = pInst->format(v0, args, vr);

		METHOD_RETURN();
	}

}

#endif

