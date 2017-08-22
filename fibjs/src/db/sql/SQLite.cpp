/*
 * SQLite.cpp
 *
 *  Created on: Aug 1, 2012
 *      Author: lion
 */

#include "object.h"
#include "SQLite.h"
#include "ifs/db.h"
#include "DBResult.h"
#include "Buffer.h"
#include "trans.h"

namespace fibjs {

#define SQLITE_OPEN_FLAGS \
    SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_SHAREDCACHE | SQLITE_OPEN_NOMUTEX

result_t db_base::openSQLite(exlib::string connString,
    obj_ptr<SQLite_base>& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    result_t hr;

    const char* c_str = connString.c_str();

    if (!qstrcmp(c_str, "sqlite:", 7))
        c_str += 7;

    obj_ptr<SQLite> db = new SQLite();
    hr = db->open(c_str);
    if (hr < 0)
        return hr;

    retVal = db;

    return 0;
}

result_t SQLite::open(const char* file)
{
    sqlite3_enable_shared_cache(1);
    if (sqlite3_open_v2(file, &m_db, SQLITE_OPEN_FLAGS, 0)) {
        result_t hr = CHECK_ERROR(Runtime::setError(sqlite3_errmsg(m_db)));
        sqlite3_close(m_db);
        m_db = NULL;
        return hr;
    }

    obj_ptr<object_base> retVal;
    execute("PRAGMA journal_mode=WAL;", 24, retVal);

    m_file = file;

    return 0;
}

SQLite::~SQLite()
{
    if (m_db)
        asyncCall(sqlite3_close, m_db);
}

result_t SQLite::get_type(exlib::string& retVal)
{
    retVal = "SQLite";
    return 0;
}

result_t SQLite::close(AsyncEvent* ac)
{
    if (!m_db)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    sqlite3_close(m_db);
    m_db = NULL;

    return 0;
}

result_t SQLite::begin(AsyncEvent* ac)
{
    if (!m_db)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    obj_ptr<object_base> retVal;
    return execute("BEGIN", 5, retVal);
}

result_t SQLite::commit(AsyncEvent* ac)
{
    if (!m_db)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    obj_ptr<object_base> retVal;
    return execute("COMMIT", 6, retVal);
}

result_t SQLite::rollback(AsyncEvent* ac)
{
    if (!m_db)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    obj_ptr<object_base> retVal;
    return execute("ROLLBACK", 8, retVal);
}

result_t SQLite::trans(v8::Local<v8::Function> func)
{
    return _trans(this, func);
}

int32_t sqlite3_step_sleep(sqlite3_stmt* stmt, int32_t ms)
{
    while (true) {
        int32_t r = sqlite3_step(stmt);
        if ((r != SQLITE_LOCKED && r != SQLITE_BUSY) || ms <= 0)
            return r;

        sqlite3_sleep(1);
        ms--;
    }
}

int32_t sqlite3_prepare_sleep(sqlite3* db, const char* zSql, int nByte,
    sqlite3_stmt** ppStmt, const char** pzTail, int32_t ms)
{
    while (true) {
        int32_t r = sqlite3_prepare_v2(db, zSql, nByte, ppStmt, pzTail);
        if ((r != SQLITE_LOCKED && r != SQLITE_BUSY) || ms <= 0)
            return r;

        sqlite3_sleep(1);
        ms--;
    }
}

#define SQLITE_SLEEP_TIME 10000

result_t SQLite::execute(const char* sql, int32_t sLen,
    obj_ptr<object_base>& retVal)
{
    if (!m_db)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    sqlite3_stmt* stmt = 0;
    const char* pStr1;

    if (sqlite3_prepare_sleep(m_db, sql, sLen, &stmt, &pStr1, SQLITE_SLEEP_TIME)) {
        result_t hr = CHECK_ERROR(Runtime::setError(sqlite3_errmsg(m_db)));
        if (stmt)
            sqlite3_finalize(stmt);
        return hr;
    }

    if (!stmt)
        return CHECK_ERROR(Runtime::setError("SQLite: Query was empty"));

    int32_t columns = sqlite3_column_count(stmt);
    obj_ptr<DBResult> res;

    if (columns > 0) {
        int32_t i;
        res = new DBResult(columns);

        for (i = 0; i < columns; i++) {
            exlib::string s = sqlite3_column_name(stmt, i);
            res->setField(i, s);
        }

        while (true) {
            int32_t r = sqlite3_step_sleep(stmt, SQLITE_SLEEP_TIME);
            if (r == SQLITE_ROW) {
                res->beginRow();
                for (i = 0; i < columns; i++) {
                    Variant v;

                    switch (sqlite3_column_type(stmt, i)) {
                    case SQLITE_NULL:
                        break;

                    case SQLITE_INTEGER:
                        v = (int64_t)sqlite3_column_int64(stmt, i);
                        break;

                    case SQLITE_FLOAT:
                        v = sqlite3_column_double(stmt, i);
                        break;

                    default:
                        const char* type = sqlite3_column_decltype(stmt, i);
                        if (type
                            && (!qstricmp(type, "blob", 4)
                                   || !qstricmp(type, "tinyblob", 8)
                                   || !qstricmp(type, "mediumblob", 10)
                                   || !qstricmp(type, "longblob", 8)
                                   || !qstricmp(type, "binary", 6)
                                   || !qstricmp(type, "varbinary", 9))) {
                            const char* data = (const char*)sqlite3_column_blob(stmt, i);
                            int32_t size = sqlite3_column_bytes(stmt, i);

                            v = new Buffer(data, size);
                        } else if (type
                            && (!qstricmp(type, "datetime")
                                   || !qstricmp(type, "date")
                                   || !qstricmp(type, "time"))) {
                            const char* data = (const char*)sqlite3_column_text(stmt, i);
                            int32_t size = sqlite3_column_bytes(stmt, i);

                            v.parseDate(data, size);
                        } else {
                            const char* data = (const char*)sqlite3_column_text(stmt, i);
                            int32_t size = sqlite3_column_bytes(stmt, i);

                            v = exlib::string(data, size);
                        }
                        break;
                    }

                    res->rowValue(i, v);
                }
                res->endRow();
            } else if (r == SQLITE_DONE)
                break;
            else {
                sqlite3_finalize(stmt);
                return CHECK_ERROR(Runtime::setError(sqlite3_errmsg(m_db)));
            }
        }
    } else {
        int32_t r = sqlite3_step_sleep(stmt, SQLITE_SLEEP_TIME);
        if (r == SQLITE_DONE)
            res = new DBResult(0, sqlite3_changes(m_db),
                sqlite3_last_insert_rowid(m_db));
        else {
            sqlite3_finalize(stmt);
            return CHECK_ERROR(Runtime::setError(sqlite3_errmsg(m_db)));
        }
    }

    sqlite3_finalize(stmt);

    retVal = res;

    return 0;
}

result_t SQLite::execute(exlib::string sql, obj_ptr<object_base>& retVal, AsyncEvent* ac)
{
    if (!m_db)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return execute(sql.c_str(), (int32_t)sql.length(), retVal);
}

result_t SQLite::execute(exlib::string sql, OptArgs args, obj_ptr<object_base>& retVal,
    AsyncEvent* ac)
{
    if (!m_db)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (ac->isSync()) {
        exlib::string str;
        result_t hr = format(sql, args, str);
        if (hr < 0)
            return hr;

        ac->m_ctx.resize(1);
        ac->m_ctx[0] = str;

        return CHECK_ERROR(CALL_E_NOSYNC);
    }

    exlib::string str = ac->m_ctx[0].string();
    return execute(str, retVal, ac);
}

result_t SQLite::format(exlib::string sql, OptArgs args,
    exlib::string& retVal)
{
    return db_base::format(sql, args, retVal);
}

result_t SQLite::get_fileName(exlib::string& retVal)
{
    if (!m_db)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    retVal = m_file;
    return 0;
}

result_t SQLite::get_timeout(int32_t& retVal)
{
    if (!m_db)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    retVal = m_nCmdTimeout;
    return 0;
}

result_t SQLite::set_timeout(int32_t newVal)
{
    if (!m_db)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    m_nCmdTimeout = newVal;
    sqlite3_busy_timeout(m_db, m_nCmdTimeout);
    return 0;
}

result_t SQLite::backup(exlib::string fileName, AsyncEvent* ac)
{
    if (!m_db)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    int32_t rc;
    struct sqlite3* db2 = NULL;
    sqlite3_backup* pBackup;

    const char* c_str = fileName.c_str();

    if (!qstrcmp(c_str, "sqlite:", 7))
        c_str += 7;

    if (sqlite3_open_v2(c_str, &db2, SQLITE_OPEN_FLAGS, 0)) {
        result_t hr = CHECK_ERROR(Runtime::setError(sqlite3_errmsg(db2)));
        return hr;
    }

    pBackup = sqlite3_backup_init(db2, "main", m_db, "main");
    if (pBackup) {
        do {
            rc = sqlite3_backup_step(pBackup, 5);
            if (rc == SQLITE_LOCKED)
                sqlite3_sleep(1);
        } while (rc == SQLITE_OK || rc == SQLITE_BUSY || rc == SQLITE_LOCKED);

        sqlite3_backup_finish(pBackup);
    } else {
        result_t hr = CHECK_ERROR(Runtime::setError(sqlite3_errmsg(m_db)));
        sqlite3_close(db2);
        return hr;
    }

    sqlite3_close(db2);

    return 0;
}

} /* namespace fibjs */
