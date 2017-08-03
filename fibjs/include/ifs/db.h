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

namespace fibjs {

class MySQL_base;
class MSSQL_base;
class SQLite_base;
class MongoDB_base;
class LevelDB_base;
class Redis_base;

class db_base : public object_base {
    DECLARE_CLASS(db_base);

public:
    // db_base
    static result_t open(exlib::string connString, obj_ptr<object_base>& retVal, AsyncEvent* ac);
    static result_t openMySQL(exlib::string connString, obj_ptr<MySQL_base>& retVal, AsyncEvent* ac);
    static result_t openMSSQL(exlib::string connString, obj_ptr<MSSQL_base>& retVal, AsyncEvent* ac);
    static result_t openSQLite(exlib::string connString, obj_ptr<SQLite_base>& retVal, AsyncEvent* ac);
    static result_t openMongoDB(exlib::string connString, obj_ptr<MongoDB_base>& retVal, AsyncEvent* ac);
    static result_t openLevelDB(exlib::string connString, obj_ptr<LevelDB_base>& retVal, AsyncEvent* ac);
    static result_t openRedis(exlib::string connString, obj_ptr<Redis_base>& retVal, AsyncEvent* ac);
    static result_t format(exlib::string sql, std::vector<v8::Local<v8::Value>>& args, exlib::string& retVal);
    static result_t formatMySQL(exlib::string sql, std::vector<v8::Local<v8::Value>>& args, exlib::string& retVal);
    static result_t formatMSSQL(exlib::string sql, std::vector<v8::Local<v8::Value>>& args, exlib::string& retVal);
    static result_t escape(exlib::string str, bool mysql, exlib::string& retVal);

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
    static void s_openMSSQL(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_openSQLite(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_openMongoDB(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_openLevelDB(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_openRedis(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_format(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_formatMySQL(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_formatMSSQL(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_escape(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
    ASYNC_STATICVALUE2(db_base, open, exlib::string, obj_ptr<object_base>);
    ASYNC_STATICVALUE2(db_base, openMySQL, exlib::string, obj_ptr<MySQL_base>);
    ASYNC_STATICVALUE2(db_base, openMSSQL, exlib::string, obj_ptr<MSSQL_base>);
    ASYNC_STATICVALUE2(db_base, openSQLite, exlib::string, obj_ptr<SQLite_base>);
    ASYNC_STATICVALUE2(db_base, openMongoDB, exlib::string, obj_ptr<MongoDB_base>);
    ASYNC_STATICVALUE2(db_base, openLevelDB, exlib::string, obj_ptr<LevelDB_base>);
    ASYNC_STATICVALUE2(db_base, openRedis, exlib::string, obj_ptr<Redis_base>);
};
}

#include "MySQL.h"
#include "MSSQL.h"
#include "SQLite.h"
#include "MongoDB.h"
#include "LevelDB.h"
#include "Redis.h"

namespace fibjs {
inline ClassInfo& db_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "open", s_open, true },
        { "openSync", s_open, true },
        { "openMySQL", s_openMySQL, true },
        { "openMySQLSync", s_openMySQL, true },
        { "openMSSQL", s_openMSSQL, true },
        { "openMSSQLSync", s_openMSSQL, true },
        { "openSQLite", s_openSQLite, true },
        { "openSQLiteSync", s_openSQLite, true },
        { "openMongoDB", s_openMongoDB, true },
        { "openMongoDBSync", s_openMongoDB, true },
        { "openLevelDB", s_openLevelDB, true },
        { "openLevelDBSync", s_openLevelDB, true },
        { "openRedis", s_openRedis, true },
        { "openRedisSync", s_openRedis, true },
        { "format", s_format, true },
        { "formatMySQL", s_formatMySQL, true },
        { "formatMSSQL", s_formatMSSQL, true },
        { "escape", s_escape, true }
    };

    static ClassData s_cd = {
        "db", true, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, 0, NULL, NULL, NULL,
        &object_base::class_info()
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void db_base::s_open(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<object_base> vr;

    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    if (!cb.IsEmpty()) {
        acb_open(v0, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = ac_open(v0, vr);

    METHOD_RETURN();
}

inline void db_base::s_openMySQL(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<MySQL_base> vr;

    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    if (!cb.IsEmpty()) {
        acb_openMySQL(v0, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = ac_openMySQL(v0, vr);

    METHOD_RETURN();
}

inline void db_base::s_openMSSQL(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<MSSQL_base> vr;

    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    if (!cb.IsEmpty()) {
        acb_openMSSQL(v0, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = ac_openMSSQL(v0, vr);

    METHOD_RETURN();
}

inline void db_base::s_openSQLite(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<SQLite_base> vr;

    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    if (!cb.IsEmpty()) {
        acb_openSQLite(v0, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = ac_openSQLite(v0, vr);

    METHOD_RETURN();
}

inline void db_base::s_openMongoDB(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<MongoDB_base> vr;

    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    if (!cb.IsEmpty()) {
        acb_openMongoDB(v0, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = ac_openMongoDB(v0, vr);

    METHOD_RETURN();
}

inline void db_base::s_openLevelDB(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<LevelDB_base> vr;

    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    if (!cb.IsEmpty()) {
        acb_openLevelDB(v0, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = ac_openLevelDB(v0, vr);

    METHOD_RETURN();
}

inline void db_base::s_openRedis(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Redis_base> vr;

    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    if (!cb.IsEmpty()) {
        acb_openRedis(v0, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = ac_openRedis(v0, vr);

    METHOD_RETURN();
}

inline void db_base::s_format(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER(-1, 1);

    ARG(exlib::string, 0);
    ARG_LIST(1);

    hr = format(v0, v1, vr);

    METHOD_RETURN();
}

inline void db_base::s_formatMySQL(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER(-1, 1);

    ARG(exlib::string, 0);
    ARG_LIST(1);

    hr = formatMySQL(v0, v1, vr);

    METHOD_RETURN();
}

inline void db_base::s_formatMSSQL(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER(-1, 1);

    ARG(exlib::string, 0);
    ARG_LIST(1);

    hr = formatMSSQL(v0, v1, vr);

    METHOD_RETURN();
}

inline void db_base::s_escape(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(bool, 1, false);

    hr = escape(v0, v1, vr);

    METHOD_RETURN();
}
}

#endif
