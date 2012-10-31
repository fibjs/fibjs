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
class object_base;
class MySQL_base;
class SQLite_base;
class MongoDB_base;

class db_base : public module_base
{
public:
	// db_base
	static result_t open(const char* connString, obj_ptr<object_base>& retVal);
	static result_t openMySQL(const char* connString, obj_ptr<MySQL_base>& retVal);
	static result_t openSQLite(const char* connString, obj_ptr<SQLite_base>& retVal);
	static result_t openMongoDB(const char* connString, obj_ptr<MongoDB_base>& retVal);
	static result_t format(const char* sql, const v8::Arguments& args, std::string& retVal);
	static result_t formatMySQL(const char* sql, const v8::Arguments& args, std::string& retVal);
	static result_t escape(const char* str, bool mysql, std::string& retVal);

	DECLARE_CLASSINFO(db_base);

public:
	static v8::Handle<v8::Value> s_open(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_openMySQL(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_openSQLite(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_openMongoDB(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_format(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_formatMySQL(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_escape(const v8::Arguments& args);
};

}

#include "MySQL.h"
#include "SQLite.h"
#include "MongoDB.h"

namespace fibjs
{
	inline ClassInfo& db_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"open", s_open, true},
			{"openMySQL", s_openMySQL, true},
			{"openSQLite", s_openSQLite, true},
			{"openMongoDB", s_openMongoDB, true},
			{"format", s_format, true},
			{"formatMySQL", s_formatMySQL, true},
			{"escape", s_escape, true}
		};

		static ClassData s_cd = 
		{ 
			"db", NULL, 
			7, s_method, 0, NULL, 0, NULL, NULL, NULL,
			&module_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}


	inline v8::Handle<v8::Value> db_base::s_open(const v8::Arguments& args)
	{
		obj_ptr<object_base> vr;

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

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> db_base::s_openSQLite(const v8::Arguments& args)
	{
		obj_ptr<SQLite_base> vr;

		METHOD_ENTER(1, 1);

		ARG_String(0);

		hr = openSQLite(v0, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> db_base::s_openMongoDB(const v8::Arguments& args)
	{
		obj_ptr<MongoDB_base> vr;

		METHOD_ENTER(1, 1);

		ARG_String(0);

		hr = openMongoDB(v0, vr);

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

	inline v8::Handle<v8::Value> db_base::s_formatMySQL(const v8::Arguments& args)
	{
		std::string vr;

		METHOD_ENTER(-1, 1);

		ARG_String(0);

		hr = formatMySQL(v0, args, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> db_base::s_escape(const v8::Arguments& args)
	{
		std::string vr;

		METHOD_ENTER(2, 1);

		ARG_String(0);
		OPT_ARG(bool, 1, false);

		hr = escape(v0, v1, vr);

		METHOD_RETURN();
	}

}

#endif

