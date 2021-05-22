/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#pragma once

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs {

class MySQL_base;
class DbConnection_base;
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
    static result_t openMSSQL(exlib::string connString, obj_ptr<DbConnection_base>& retVal, AsyncEvent* ac);
    static result_t openSQLite(exlib::string connString, obj_ptr<SQLite_base>& retVal, AsyncEvent* ac);
    static result_t openOdbc(exlib::string connString, obj_ptr<DbConnection_base>& retVal, AsyncEvent* ac);
    static result_t openMongoDB(exlib::string connString, obj_ptr<MongoDB_base>& retVal, AsyncEvent* ac);
    static result_t openLevelDB(exlib::string connString, obj_ptr<LevelDB_base>& retVal, AsyncEvent* ac);
    static result_t openRedis(exlib::string connString, obj_ptr<Redis_base>& retVal, AsyncEvent* ac);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        Isolate* isolate = Isolate::current();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_static_open(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_openMySQL(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_openMSSQL(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_openSQLite(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_openOdbc(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_openMongoDB(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_openLevelDB(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_openRedis(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
    ASYNC_STATICVALUE2(db_base, open, exlib::string, obj_ptr<object_base>);
    ASYNC_STATICVALUE2(db_base, openMySQL, exlib::string, obj_ptr<MySQL_base>);
    ASYNC_STATICVALUE2(db_base, openMSSQL, exlib::string, obj_ptr<DbConnection_base>);
    ASYNC_STATICVALUE2(db_base, openSQLite, exlib::string, obj_ptr<SQLite_base>);
    ASYNC_STATICVALUE2(db_base, openOdbc, exlib::string, obj_ptr<DbConnection_base>);
    ASYNC_STATICVALUE2(db_base, openMongoDB, exlib::string, obj_ptr<MongoDB_base>);
    ASYNC_STATICVALUE2(db_base, openLevelDB, exlib::string, obj_ptr<LevelDB_base>);
    ASYNC_STATICVALUE2(db_base, openRedis, exlib::string, obj_ptr<Redis_base>);
};
}

#include "ifs/MySQL.h"
#include "ifs/DbConnection.h"
#include "ifs/SQLite.h"
#include "ifs/MongoDB.h"
#include "ifs/LevelDB.h"
#include "ifs/Redis.h"

namespace fibjs {
inline ClassInfo& db_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "open", s_static_open, true },
        { "openSync", s_static_open, true },
        { "openMySQL", s_static_openMySQL, true },
        { "openMySQLSync", s_static_openMySQL, true },
        { "openMSSQL", s_static_openMSSQL, true },
        { "openMSSQLSync", s_static_openMSSQL, true },
        { "openSQLite", s_static_openSQLite, true },
        { "openSQLiteSync", s_static_openSQLite, true },
        { "openOdbc", s_static_openOdbc, true },
        { "openOdbcSync", s_static_openOdbc, true },
        { "openMongoDB", s_static_openMongoDB, true },
        { "openMongoDBSync", s_static_openMongoDB, true },
        { "openLevelDB", s_static_openLevelDB, true },
        { "openLevelDBSync", s_static_openLevelDB, true },
        { "openRedis", s_static_openRedis, true },
        { "openRedisSync", s_static_openRedis, true }
    };

    static ClassData s_cd = {
        "db", true, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, 0, NULL, 0, NULL, NULL, NULL,
        &object_base::class_info()
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void db_base::s_static_open(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<object_base> vr;

    METHOD_NAME("db.open");
    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    if (!cb.IsEmpty())
        hr = acb_open(v0, cb, args);
    else
        hr = ac_open(v0, vr);

    METHOD_RETURN();
}

inline void db_base::s_static_openMySQL(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<MySQL_base> vr;

    METHOD_NAME("db.openMySQL");
    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    if (!cb.IsEmpty())
        hr = acb_openMySQL(v0, cb, args);
    else
        hr = ac_openMySQL(v0, vr);

    METHOD_RETURN();
}

inline void db_base::s_static_openMSSQL(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<DbConnection_base> vr;

    METHOD_NAME("db.openMSSQL");
    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    if (!cb.IsEmpty())
        hr = acb_openMSSQL(v0, cb, args);
    else
        hr = ac_openMSSQL(v0, vr);

    METHOD_RETURN();
}

inline void db_base::s_static_openSQLite(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<SQLite_base> vr;

    METHOD_NAME("db.openSQLite");
    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    if (!cb.IsEmpty())
        hr = acb_openSQLite(v0, cb, args);
    else
        hr = ac_openSQLite(v0, vr);

    METHOD_RETURN();
}

inline void db_base::s_static_openOdbc(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<DbConnection_base> vr;

    METHOD_NAME("db.openOdbc");
    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    if (!cb.IsEmpty())
        hr = acb_openOdbc(v0, cb, args);
    else
        hr = ac_openOdbc(v0, vr);

    METHOD_RETURN();
}

inline void db_base::s_static_openMongoDB(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<MongoDB_base> vr;

    METHOD_NAME("db.openMongoDB");
    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    if (!cb.IsEmpty())
        hr = acb_openMongoDB(v0, cb, args);
    else
        hr = ac_openMongoDB(v0, vr);

    METHOD_RETURN();
}

inline void db_base::s_static_openLevelDB(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<LevelDB_base> vr;

    METHOD_NAME("db.openLevelDB");
    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    if (!cb.IsEmpty())
        hr = acb_openLevelDB(v0, cb, args);
    else
        hr = ac_openLevelDB(v0, vr);

    METHOD_RETURN();
}

inline void db_base::s_static_openRedis(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Redis_base> vr;

    METHOD_NAME("db.openRedis");
    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    if (!cb.IsEmpty())
        hr = acb_openRedis(v0, cb, args);
    else
        hr = ac_openRedis(v0, vr);

    METHOD_RETURN();
}
}
