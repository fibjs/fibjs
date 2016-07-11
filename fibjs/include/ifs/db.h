/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _db_base_H_
#define _db_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs
{

class object_base;
class MySQL_base;
class SQLite_base;
class MongoDB_base;
class LevelDB_base;
class Redis_base;

class db_base : public object_base
{
    DECLARE_CLASS(db_base);

public:
    // db_base
    static result_t open(const char* connString, obj_ptr<object_base>& retVal, AsyncEvent* ac);
    static result_t openMySQL(const char* connString, obj_ptr<MySQL_base>& retVal, AsyncEvent* ac);
    static result_t openSQLite(const char* connString, obj_ptr<SQLite_base>& retVal, AsyncEvent* ac);
    static result_t openMongoDB(const char* connString, obj_ptr<MongoDB_base>& retVal, AsyncEvent* ac);
    static result_t openLevelDB(const char* connString, obj_ptr<LevelDB_base>& retVal, AsyncEvent* ac);
    static result_t openRedis(const char* connString, obj_ptr<Redis_base>& retVal, AsyncEvent* ac);
    static result_t format(const char* sql, const v8::FunctionCallbackInfo<v8::Value>& args, qstring& retVal);
    static result_t formatMySQL(const char* sql, const v8::FunctionCallbackInfo<v8::Value>& args, qstring& retVal);
    static result_t escape(const char* str, bool mysql, qstring& retVal);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        Isolate* isolate = Isolate::current();

        isolate->m_isolate->ThrowException(
            isolate->NewFromUtf8("not a constructor"));
    }

public:
    static void s_open(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_openMySQL(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_openSQLite(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_openMongoDB(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_openLevelDB(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_openRedis(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_format(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_formatMySQL(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_escape(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
    ASYNC_STATICVALUE2(db_base, open, const char*, obj_ptr<object_base>);
    ASYNC_STATICVALUE2(db_base, openMySQL, const char*, obj_ptr<MySQL_base>);
    ASYNC_STATICVALUE2(db_base, openSQLite, const char*, obj_ptr<SQLite_base>);
    ASYNC_STATICVALUE2(db_base, openMongoDB, const char*, obj_ptr<MongoDB_base>);
    ASYNC_STATICVALUE2(db_base, openLevelDB, const char*, obj_ptr<LevelDB_base>);
    ASYNC_STATICVALUE2(db_base, openRedis, const char*, obj_ptr<Redis_base>);
};

}

#include "MySQL.h"
#include "SQLite.h"
#include "MongoDB.h"
#include "LevelDB.h"
#include "Redis.h"

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
            {"openLevelDB", s_openLevelDB, true},
            {"openRedis", s_openRedis, true},
            {"format", s_format, true},
            {"formatMySQL", s_formatMySQL, true},
            {"escape", s_escape, true}
        };

        static ClassData s_cd = 
        { 
            "db", s__new, NULL, 
            9, s_method, 0, NULL, 0, NULL, NULL, NULL,
            NULL
        };

        static ClassInfo s_ci(s_cd);
        return s_ci;
    }


    inline void db_base::s_open(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        obj_ptr<object_base> vr;

        ASYNC_METHOD_ENTER(1, 1);

        ARG(arg_string, 0);

        if(!cb.IsEmpty()) {
            acb_open(v0, vr, cb);
            hr = CALL_RETURN_NULL;
        } else
            hr = ac_open(v0, vr);

        METHOD_RETURN();
    }

    inline void db_base::s_openMySQL(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        obj_ptr<MySQL_base> vr;

        ASYNC_METHOD_ENTER(1, 1);

        ARG(arg_string, 0);

        if(!cb.IsEmpty()) {
            acb_openMySQL(v0, vr, cb);
            hr = CALL_RETURN_NULL;
        } else
            hr = ac_openMySQL(v0, vr);

        METHOD_RETURN();
    }

    inline void db_base::s_openSQLite(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        obj_ptr<SQLite_base> vr;

        ASYNC_METHOD_ENTER(1, 1);

        ARG(arg_string, 0);

        if(!cb.IsEmpty()) {
            acb_openSQLite(v0, vr, cb);
            hr = CALL_RETURN_NULL;
        } else
            hr = ac_openSQLite(v0, vr);

        METHOD_RETURN();
    }

    inline void db_base::s_openMongoDB(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        obj_ptr<MongoDB_base> vr;

        ASYNC_METHOD_ENTER(1, 1);

        ARG(arg_string, 0);

        if(!cb.IsEmpty()) {
            acb_openMongoDB(v0, vr, cb);
            hr = CALL_RETURN_NULL;
        } else
            hr = ac_openMongoDB(v0, vr);

        METHOD_RETURN();
    }

    inline void db_base::s_openLevelDB(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        obj_ptr<LevelDB_base> vr;

        ASYNC_METHOD_ENTER(1, 1);

        ARG(arg_string, 0);

        if(!cb.IsEmpty()) {
            acb_openLevelDB(v0, vr, cb);
            hr = CALL_RETURN_NULL;
        } else
            hr = ac_openLevelDB(v0, vr);

        METHOD_RETURN();
    }

    inline void db_base::s_openRedis(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        obj_ptr<Redis_base> vr;

        ASYNC_METHOD_ENTER(1, 1);

        ARG(arg_string, 0);

        if(!cb.IsEmpty()) {
            acb_openRedis(v0, vr, cb);
            hr = CALL_RETURN_NULL;
        } else
            hr = ac_openRedis(v0, vr);

        METHOD_RETURN();
    }

    inline void db_base::s_format(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        qstring vr;

        METHOD_ENTER(-1, 1);

        ARG(arg_string, 0);

        hr = format(v0, args, vr);

        METHOD_RETURN();
    }

    inline void db_base::s_formatMySQL(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        qstring vr;

        METHOD_ENTER(-1, 1);

        ARG(arg_string, 0);

        hr = formatMySQL(v0, args, vr);

        METHOD_RETURN();
    }

    inline void db_base::s_escape(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        qstring vr;

        METHOD_ENTER(2, 1);

        ARG(arg_string, 0);
        OPT_ARG(bool, 1, false);

        hr = escape(v0, v1, vr);

        METHOD_RETURN();
    }

}

#endif

