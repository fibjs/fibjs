/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
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
	virtual result_t close() = 0;
	virtual result_t begin() = 0;
	virtual result_t commit() = 0;
	virtual result_t rollback() = 0;
	virtual result_t execute(const char* sql, const v8::Arguments& args, obj_ptr<DBResult_base>& retVal) = 0;
	virtual result_t format(const char* sql, const v8::Arguments& args, std::string& retVal) = 0;

	DECLARE_CLASSINFO(DbConnection_base);

public:
	static v8::Handle<v8::Value> s_close(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_begin(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_commit(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_rollback(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_execute(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_format(const v8::Arguments& args);
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


	inline v8::Handle<v8::Value> DbConnection_base::s_close(const v8::Arguments& args)
	{
		METHOD_INSTANCE(DbConnection_base);
		METHOD_ENTER(0, 0);

		hr = pInst->close();

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> DbConnection_base::s_begin(const v8::Arguments& args)
	{
		METHOD_INSTANCE(DbConnection_base);
		METHOD_ENTER(0, 0);

		hr = pInst->begin();

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> DbConnection_base::s_commit(const v8::Arguments& args)
	{
		METHOD_INSTANCE(DbConnection_base);
		METHOD_ENTER(0, 0);

		hr = pInst->commit();

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> DbConnection_base::s_rollback(const v8::Arguments& args)
	{
		METHOD_INSTANCE(DbConnection_base);
		METHOD_ENTER(0, 0);

		hr = pInst->rollback();

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> DbConnection_base::s_execute(const v8::Arguments& args)
	{
		obj_ptr<DBResult_base> vr;

		METHOD_INSTANCE(DbConnection_base);
		METHOD_ENTER(-1, 1);

		ARG_String(0);

		hr = pInst->execute(v0, args, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> DbConnection_base::s_format(const v8::Arguments& args)
	{
		std::string vr;

		METHOD_INSTANCE(DbConnection_base);
		METHOD_ENTER(-1, 1);

		ARG_String(0);

		hr = pInst->format(v0, args, vr);

		METHOD_RETURN();
	}

}

#endif

