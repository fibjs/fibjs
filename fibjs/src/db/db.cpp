/*
 * db.cpp
 *
 *  Created on: Jul 20, 2012
 *      Author: lion
 */

#include "object.h"
#include "ifs/db.h"
#include "ifs/Buffer.h"
#include "db_format.h"
#include "sql/SQLite.h"
#include "sql/mysql.h"
#include "sql/mssql.h"
#include "odbc/Odbc.h"

namespace fibjs {

DECLARE_MODULE(db);

result_t db_base::open(exlib::string connString, obj_ptr<object_base>& retVal, AsyncEvent* ac)
{
    if (!qstrcmp(connString.c_str(), "mysql:", 6))
        return openMySQL(connString, (obj_ptr<MySQL_base>&)retVal, ac);

    if (!qstrcmp(connString.c_str(), "mssql:", 6))
        return openMSSQL(connString, (obj_ptr<DbConnection_base>&)retVal, ac);

    if (!qstrcmp(connString.c_str(), "sqlite:", 7))
        return openSQLite(connString, (obj_ptr<SQLite_base>&)retVal, ac);

    if (!qstrcmp(connString.c_str(), "redis:", 6))
        return openRedis(connString, (obj_ptr<Redis_base>&)retVal, ac);

    if (!qstrcmp(connString.c_str(), "mongodb:", 8))
        return openMongoDB(connString, (obj_ptr<MongoDB_base>&)retVal, ac);

    if (!qstrcmp(connString.c_str(), "leveldb:", 8))
        return openLevelDB(connString, (obj_ptr<LevelDB_base>&)retVal, ac);

    return CHECK_ERROR(CALL_E_INVALIDARG);
}

result_t db_base::format(exlib::string sql, OptArgs args,
    exlib::string& retVal)
{
    return db_format<SQLite>::format(sql.c_str(), args, retVal);
}

result_t db_base::formatMySQL(exlib::string sql, OptArgs args,
    exlib::string& retVal)
{
    return db_format<mysql>::format(sql.c_str(), args, retVal);
}

result_t db_base::formatMSSQL(exlib::string sql, OptArgs args,
    exlib::string& retVal)
{
    return db_format<mssql>::format(sql.c_str(), args, retVal);
}

result_t db_base::format(exlib::string method, v8::Local<v8::Object> opts,
    exlib::string& retVal)
{
    return db_format<SQLite>::format(method, opts, retVal);
}

result_t db_base::formatMySQL(exlib::string method, v8::Local<v8::Object> opts,
    exlib::string& retVal)
{
    return db_format<mysql>::format(method, opts, retVal);
}

result_t db_base::formatMSSQL(exlib::string method, v8::Local<v8::Object> opts,
    exlib::string& retVal)
{
    return db_format<mssql>::format(method, opts, retVal);
}
}
