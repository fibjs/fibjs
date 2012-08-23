/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _db_base_H_
#define _db_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "module.h"

namespace fibjs
{

class module_base;
class DbConnection_base;
class MySQL_base;

class db_base : public module_base
{
public:
	// db_base
	static result_t open(const char* connString, obj_ptr<DbConnection_base>& retVal);
	static result_t openMySQL(const char* connString, obj_ptr<MySQL_base>& retVal);
	static result_t openMySQL(const char* host, int32_t port, const char* username, const char* password, const char* dbName, obj_ptr<MySQL_base>& retVal);
	static result_t openSQLite(const char* connString, obj_ptr<DbConnection_base>& retVal);
	static result_t format(const char* sql, const v8::Arguments& args, std::string& retVal);

	DECLARE_CLASSINFO(db_base);

public:
	static v8::Handle<v8::Value> s_open(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_openMySQL(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_openSQLite(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_format(const v8::Arguments& args);
};

}

#include "DbConnection.h"
#include "MySQL.h"

namespace fibjs
{
	inline ClassInfo& db_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"open", s_open, true},
			{"openMySQL", s_openMySQL, true},
			{"openSQLite", s_openSQLite, true},
			{"format", s_format, true}
		};

		static ClassData s_cd = 
		{ 
			"db", NULL, 
			4, s_method, 0, NULL, 0, NULL, NULL, NULL,
			&module_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}


	inline v8::Handle<v8::Value> db_base::s_open(const v8::Arguments& args)
	{
		obj_ptr<DbConnection_base> vr;

		METHOD_ENTER(1, 1);

		ARG_String(0);

		hr = open(v0, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> db_base::s_openMySQL(const v8::Arguments& args)
	{
		obj_ptr<MySQL_base> vr;

		METHOD_ENTER(1, 1);

		ARG_String(0);

		hr = openMySQL(v0, vr);

		METHOD_OVER(5, 4);

		ARG_String(0);
		ARG(int32_t, 1);
		ARG_String(2);
		ARG_String(3);
		OPT_ARG_String(4, "");

		hr = openMySQL(v0, v1, v2, v3, v4, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> db_base::s_openSQLite(const v8::Arguments& args)
	{
		obj_ptr<DbConnection_base> vr;

		METHOD_ENTER(1, 1);

		ARG_String(0);

		hr = openSQLite(v0, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> db_base::s_format(const v8::Arguments& args)
	{
		std::string vr;

		METHOD_ENTER(-1, 1);

		ARG_String(0);

		hr = format(v0, args, vr);

		METHOD_RETURN();
	}

}

#endif

