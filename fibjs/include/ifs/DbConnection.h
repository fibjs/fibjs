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
	virtual result_t use(const char* dbName) = 0;
	virtual result_t beginTrans() = 0;
	virtual result_t commitTrans() = 0;
	virtual result_t rollBack() = 0;
	virtual result_t execute(const char* sql, obj_ptr<DBResult_base>& retVal) = 0;

public:
	static ClassInfo& class_info();

	virtual ClassInfo& Classinfo()
	{
		return class_info();
	}

public:
	static v8::Handle<v8::Value> s_close(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_use(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_beginTrans(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_commitTrans(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_rollBack(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_execute(const v8::Arguments& args);
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
			{"use", s_use},
			{"beginTrans", s_beginTrans},
			{"commitTrans", s_commitTrans},
			{"rollBack", s_rollBack},
			{"execute", s_execute}
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

	inline v8::Handle<v8::Value> DbConnection_base::s_use(const v8::Arguments& args)
	{
		METHOD_INSTANCE(DbConnection_base);
		METHOD_ENTER(1, 1);

		ARG_String(0);

		hr = pInst->use(v0);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> DbConnection_base::s_beginTrans(const v8::Arguments& args)
	{
		METHOD_INSTANCE(DbConnection_base);
		METHOD_ENTER(0, 0);

		hr = pInst->beginTrans();

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> DbConnection_base::s_commitTrans(const v8::Arguments& args)
	{
		METHOD_INSTANCE(DbConnection_base);
		METHOD_ENTER(0, 0);

		hr = pInst->commitTrans();

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> DbConnection_base::s_rollBack(const v8::Arguments& args)
	{
		METHOD_INSTANCE(DbConnection_base);
		METHOD_ENTER(0, 0);

		hr = pInst->rollBack();

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> DbConnection_base::s_execute(const v8::Arguments& args)
	{
		obj_ptr<DBResult_base> vr;

		METHOD_INSTANCE(DbConnection_base);
		METHOD_ENTER(1, 1);

		ARG_String(0);

		hr = pInst->execute(v0, vr);

		METHOD_RETURN();
	}

}

#endif

