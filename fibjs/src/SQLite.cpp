/*
 * SQLite.cpp
 *
 *  Created on: Aug 1, 2012
 *      Author: lion
 */

#include "SQLite.h"
#include "ifs/db.h"
#include "DBResult.h"
#include "Buffer.h"

namespace fibjs
{

#define SQLITE_OPEN_FLAGS \
    SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | \
    SQLITE_OPEN_SHAREDCACHE | SQLITE_OPEN_NOMUTEX

result_t db_base::openSQLite(const char *connString,
                             obj_ptr<SQLite_base> &retVal, exlib::AsyncEvent *ac)
{
    if (switchToAsync(ac))
        return CALL_E_NOSYNC;

    result_t hr;

    if (!qstrcmp(connString, "sqlite:", 7))
        connString += 7;

    obj_ptr<SQLite> db = new SQLite();
    hr = db->open(connString);
    if (hr < 0)
        return hr;

    retVal = db;

    return 0;
}

result_t SQLite::open(const char *file)
{
    if (sqlite3_open_v2(file, &m_db, SQLITE_OPEN_FLAGS, 0))
    {
        result_t hr = Runtime::setError(sqlite3_errmsg(m_db));
        sqlite3_close(m_db);
        m_db = NULL;
        return hr;
    }

    m_file = file;

    return 0;
}

result_t SQLite::close(exlib::AsyncEvent *ac)
{
    if (!m_db)
        return CALL_E_INVALID_CALL;

    if (switchToAsync(ac))
        return CALL_E_NOSYNC;

    sqlite3_close(m_db);
    m_db = NULL;

    return 0;
}

result_t SQLite::begin(exlib::AsyncEvent *ac)
{
    if (!m_db)
        return CALL_E_INVALID_CALL;

    if (switchToAsync(ac))
        return CALL_E_NOSYNC;

    obj_ptr<DBResult_base> retVal;
    return execute("BEGIN", 5, retVal);
}

result_t SQLite::commit(exlib::AsyncEvent *ac)
{
    if (!m_db)
        return CALL_E_INVALID_CALL;

    if (switchToAsync(ac))
        return CALL_E_NOSYNC;

    obj_ptr<DBResult_base> retVal;
    return execute("COMMIT", 6, retVal);
}

result_t SQLite::rollback(exlib::AsyncEvent *ac)
{
    if (!m_db)
        return CALL_E_INVALID_CALL;

    if (switchToAsync(ac))
        return CALL_E_NOSYNC;

    obj_ptr<DBResult_base> retVal;
    return execute("ROLLBACK", 8, retVal);
}

int sqlite3_step_sleep(sqlite3_stmt *stmt, int ms)
{
    while (true)
    {
        int r = sqlite3_step(stmt);
        if (r != SQLITE_LOCKED || ms <= 0)
            return r;

        sqlite3_sleep(1);
        ms --;
    }
}

#define SQLITE_SLEEP_TIME   10000

result_t SQLite::execute(const char *sql, int sLen,
                         obj_ptr<DBResult_base> &retVal)
{
    if (!m_db)
        return CALL_E_INVALID_CALL;

    sqlite3_stmt *stmt = 0;
    const char *pStr1;

    if (sqlite3_prepare_v2(m_db, sql, sLen, &stmt, &pStr1))
    {
        result_t hr = Runtime::setError(sqlite3_errmsg(m_db));
        if (stmt)
            sqlite3_finalize(stmt);
        return hr;
    }

    if (!stmt)
        return Runtime::setError("Query was empty");

    int columns = sqlite3_column_count(stmt);
    obj_ptr<DBResult> res;

    if (columns > 0)
    {
        int i;
        res = new DBResult(columns);

        for (i = 0; i < columns; i++)
        {
            std::string s = sqlite3_column_name(stmt, i);
            res->setField(i, s);
        }

        while (true)
        {
            int r = sqlite3_step_sleep(stmt, SQLITE_SLEEP_TIME);
            if (r == SQLITE_ROW)
            {
                res->beginRow();
                for (i = 0; i < columns; i++)
                {
                    Variant v;

                    switch (sqlite3_column_type(stmt, i))
                    {
                    case SQLITE_NULL:
                        break;

                    case SQLITE_INTEGER:
                        v = (int64_t) sqlite3_column_int64(stmt, i);
                        break;

                    case SQLITE_FLOAT:
                        v = sqlite3_column_double(stmt, i);
                        break;

                    default:
                        const char *type = sqlite3_column_decltype(stmt, i);
                        if (type
                                && (!qstricmp(type, "blob")
                                    || !qstricmp(type, "tinyblob")
                                    || !qstricmp(type, "mediumblob")
                                    || !qstricmp(type, "longblob")))
                        {
                            const char *data =
                                (const char *) sqlite3_column_blob(stmt, i);
                            int size = sqlite3_column_bytes(stmt, i);

                            v = new Buffer(std::string(data, size));
                        }
                        else if (type
                                 && (!qstricmp(type, "datetime")
                                     || !qstricmp(type, "date")
                                     || !qstricmp(type, "time")))
                        {
                            const char *data =
                                (const char *) sqlite3_column_text(stmt, i);
                            int size = sqlite3_column_bytes(stmt, i);

                            v.parseDate(data, size);
                        }
                        else
                        {
                            const char *data =
                                (const char *) sqlite3_column_text(stmt, i);
                            int size = sqlite3_column_bytes(stmt, i);

                            v = std::string(data, size);
                        }
                        break;

                    }

                    res->rowValue(i, v);
                }
                res->endRow();
            }
            else if (r == SQLITE_DONE)
                break;
            else
            {
                sqlite3_finalize(stmt);
                return Runtime::setError(sqlite3_errmsg(m_db));
            }
        }
    }
    else
    {
        int r = sqlite3_step_sleep(stmt, SQLITE_SLEEP_TIME);
        if (r == SQLITE_DONE)
            res = new DBResult(sqlite3_changes(m_db),
                               sqlite3_last_insert_rowid(m_db));
        else
        {
            sqlite3_finalize(stmt);
            return Runtime::setError(sqlite3_errmsg(m_db));
        }
    }

    sqlite3_finalize(stmt);

    retVal = res;

    return 0;
}

result_t SQLite::execute(const char *sql, obj_ptr<DBResult_base> &retVal, exlib::AsyncEvent *ac)
{
    if (!m_db)
        return CALL_E_INVALID_CALL;

    if (switchToAsync(ac))
        return CALL_E_NOSYNC;

    return execute(sql, (int) qstrlen(sql), retVal);
}

result_t SQLite::execute(const char *sql, const v8::FunctionCallbackInfo<v8::Value> &args,
                         obj_ptr<DBResult_base> &retVal)
{
    std::string str;
    result_t hr = format(sql, args, str);
    if (hr < 0)
        return hr;

    v8::Local<v8::Value> v = args[args.Length() - 1];

    return ac_execute(str.c_str(), retVal);
}

result_t SQLite::format(const char *sql, const v8::FunctionCallbackInfo<v8::Value> &args,
                        std::string &retVal)
{
    return db_base::format(sql, args, retVal);
}

result_t SQLite::get_fileName(std::string &retVal)
{
    if (!m_db)
        return CALL_E_INVALID_CALL;

    retVal = m_file;
    return 0;
}

result_t SQLite::get_timeout(int32_t &retVal)
{
    if (!m_db)
        return CALL_E_INVALID_CALL;

    retVal = m_nCmdTimeout;
    return 0;
}

result_t SQLite::set_timeout(int32_t newVal)
{
    if (!m_db)
        return CALL_E_INVALID_CALL;

    m_nCmdTimeout = newVal;
    sqlite3_busy_timeout(m_db, m_nCmdTimeout);
    return 0;
}

result_t SQLite::backup(const char *fileName, exlib::AsyncEvent *ac)
{
    if (!m_db)
        return CALL_E_INVALID_CALL;

    if (!ac)
        return CALL_E_NOSYNC;

    int rc;
    struct sqlite3 *db2 = NULL;
    sqlite3_backup *pBackup;

    if (sqlite3_open_v2(fileName, &db2, SQLITE_OPEN_FLAGS, 0))
    {
        result_t hr = Runtime::setError(sqlite3_errmsg(db2));
        sqlite3_close(m_db);
        return hr;
    }

    pBackup = sqlite3_backup_init(db2, "main", m_db, "main");
    if (pBackup)
    {
        do
        {
            rc = sqlite3_backup_step(pBackup, 5);
            if (rc == SQLITE_LOCKED)
                sqlite3_sleep(1);
        }
        while (rc == SQLITE_OK || rc == SQLITE_BUSY || rc == SQLITE_LOCKED);

        sqlite3_backup_finish(pBackup);
    }
    else
    {
        result_t hr = Runtime::setError(sqlite3_errmsg(db2));
        sqlite3_close(m_db);
        return hr;
    }

    sqlite3_close(m_db);

    return 0;
}

} /* namespace fibjs */
