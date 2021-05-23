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
#include "ifs/coroutine.h"

namespace fibjs {

#define TOKEN(s, n) pCtx1->xToken(pCtx1->pCtx, tflags, s, (int)(n - s), (int)(s - pToken) + iStart, (int)(n - pToken) + iStart)

struct MyCtx {
    void* pCtx;
    int (*xToken)(void*, int, const char*, int, int, int);
};

static int new_Token_cb(void* pCtx, int tflags, const char* pToken, int nToken, int iStart, int iEnd)
{
    MyCtx* pCtx1 = (MyCtx*)pCtx;

    const char* pStart = pToken;
    const char* pEnd = pToken + nToken;
    const char* pAscii = pStart;

    while (pStart < pEnd) {
        const char* p = pStart;
        exlib::wchar32 ch = utf_getchar(pStart, pEnd);

        if (ch >= 0x4e00 && ch <= 0xa000) {
            if (p > pAscii)
                TOKEN(pAscii, p);
            TOKEN(p, pStart);

            pAscii = pStart;
        }
    }
    if (pEnd > pAscii)
        TOKEN(pAscii, pEnd);

    return 0;
}

static int (*old_tokenize)(Fts5Tokenizer*, void* pCtx, int flags, const char* pText, int nText,
    int (*xToken)(void*, int, const char*, int, int, int));
static int new_tokenize(Fts5Tokenizer* pTokenizer, void* pCtx, int iUnused, const char* pText, int nText,
    int (*xToken)(void*, int, const char*, int nToken, int iStart, int iEnd))
{
    MyCtx ctx = { pCtx, xToken };
    return old_tokenize(pTokenizer, &ctx, iUnused, pText, nText, new_Token_cb);
}

static int (*old_fts5FindTokenizer)(fts5_api* pApi, const char* zName, void** ppUserData, fts5_tokenizer* pTokenizer);
static int new_fts5FindTokenizer(fts5_api* pApi, const char* zName, void** ppUserData, fts5_tokenizer* pTokenizer)
{
    int rc = old_fts5FindTokenizer(pApi, zName, ppUserData, pTokenizer);
    if (rc == 0 && !qstrcmp(zName, "unicode61")) {
        if (old_tokenize == NULL)
            old_tokenize = pTokenizer->xTokenize;
        pTokenizer->xTokenize = new_tokenize;
    }

    return rc;
}

static void hook_fts5_api(sqlite3* db)
{
    fts5_api* pApi = 0;
    sqlite3_stmt* pStmt = 0;

    if (SQLITE_OK == sqlite3_prepare(db, "SELECT fts5(?)", -1, &pStmt, 0)) {
        sqlite3_bind_pointer(pStmt, 1, (void*)&pApi, "fts5_api_ptr", NULL);
        sqlite3_step(pStmt);

        if (old_fts5FindTokenizer == NULL)
            old_fts5FindTokenizer = pApi->xFindTokenizer;
        pApi->xFindTokenizer = new_fts5FindTokenizer;

        sqlite3_finalize(pStmt);
    }
}

#define SQLITE_OPEN_FLAGS \
    SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_SHAREDCACHE | SQLITE_OPEN_NOMUTEX

result_t db_base::openSQLite(exlib::string connString,
    obj_ptr<SQLite_base>& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_LONGSYNC);

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
    if (sqlite3_open_v2(file, (sqlite3**)&m_conn, SQLITE_OPEN_FLAGS, 0)) {
        result_t hr = CHECK_ERROR(Runtime::setError(sqlite3_errmsg((sqlite3*)m_conn)));
        sqlite3_close((sqlite3*)m_conn);
        m_conn = NULL;
        return hr;
    }

    obj_ptr<NArray> retVal;
    cc_execute("PRAGMA journal_mode=WAL;", retVal);

    hook_fts5_api((sqlite3*)m_conn);

    m_file = file;

    return 0;
}

SQLite::~SQLite()
{
    if (m_conn)
        asyncCall(sqlite3_close, (sqlite3*)m_conn);
}

result_t SQLite::get_type(exlib::string& retVal)
{
    retVal = "SQLite";
    return 0;
}

result_t SQLite::close(AsyncEvent* ac)
{
    if (!m_conn)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_LONGSYNC);

    sqlite3_close((sqlite3*)m_conn);
    m_conn = NULL;

    return 0;
}

inline int32_t _busy(int32_t ms, int32_t count)
{
    static const int32_t delays[] = { 1, 2, 5, 10, 15, 20, 25, 25, 25, 50, 50, 100 };

    int32_t delay;

    if (count < (int32_t)ARRAYSIZE(delays))
        delay = delays[count];
    else
        delay = delays[ARRAYSIZE(delays) - 1];

    if (delay > ms)
        delay = ms;

    coroutine_base::cc_sleep(delay);
    return delay;
}

int32_t sqlite3_step_sleep(sqlite3_stmt* stmt, int32_t ms)
{
    int32_t count = 0;
    while (true) {
        int32_t r = sqlite3_step(stmt);
        if ((r != SQLITE_LOCKED && r != SQLITE_BUSY) || ms <= 0)
            return r;
        ms -= _busy(ms, count++);
    }
}

int32_t sqlite3_prepare_sleep(sqlite3* db, const char* zSql, int nByte,
    sqlite3_stmt** ppStmt, const char** pzTail, int32_t ms)
{
    int32_t count = 0;
    while (true) {
        int32_t r = sqlite3_prepare_v2(db, zSql, nByte, ppStmt, pzTail);
        if ((r != SQLITE_LOCKED && r != SQLITE_BUSY) || ms <= 0)
            return r;
        ms -= _busy(ms, count++);
    }
}

result_t SQLite::execute(exlib::string sql, obj_ptr<NArray>& retVal, AsyncEvent* ac)
{
    if (!m_conn)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_LONGSYNC);

    const char* pStr = sql.c_str();
    int32_t sLen = (int32_t)sql.length();
    const char* pStr1;

    do {
        sqlite3_stmt* stmt = 0;

        if (sqlite3_prepare_sleep((sqlite3*)m_conn, pStr, sLen, &stmt, &pStr1, m_nCmdTimeout)) {
            result_t hr = CHECK_ERROR(Runtime::setError(sqlite3_errmsg((sqlite3*)m_conn)));
            if (stmt)
                sqlite3_finalize(stmt);
            return hr;
        }

        if (!stmt)
            return CHECK_ERROR(Runtime::setError("SQLite: Query was empty"));

        sLen -= (int32_t)(pStr1 - pStr);

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
                int32_t r = sqlite3_step_sleep(stmt, m_nCmdTimeout);
                if (r == SQLITE_ROW) {
                    res->beginRow();
                    for (i = 0; i < columns; i++) {
                        Variant v;

                        switch (sqlite3_column_type(stmt, i)) {
                        case SQLITE_NULL:
                            v.setNull();
                            break;

                        case SQLITE_INTEGER:
                            v = (double)sqlite3_column_int64(stmt, i);
                            break;

                        case SQLITE_FLOAT:
                            v = sqlite3_column_double(stmt, i);
                            break;

                        case SQLITE_BLOB: {
                            const char* data = (const char*)sqlite3_column_blob(stmt, i);
                            int32_t size = sqlite3_column_bytes(stmt, i);

                            v = new Buffer(data, size);
                            break;
                        }

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
                                       || !qstricmp(type, "timestamp")
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
                    return CHECK_ERROR(Runtime::setError(sqlite3_errmsg((sqlite3*)m_conn)));
                }
            }
        } else {
            int32_t r = sqlite3_step_sleep(stmt, m_nCmdTimeout);
            if (r == SQLITE_DONE)
                res = new DBResult(0, sqlite3_changes((sqlite3*)m_conn),
                    sqlite3_last_insert_rowid((sqlite3*)m_conn));
            else {
                sqlite3_finalize(stmt);
                return CHECK_ERROR(Runtime::setError(sqlite3_errmsg((sqlite3*)m_conn)));
            }
        }

        sqlite3_finalize(stmt);

        while (qisspace(*pStr1)) {
            pStr1++;
            sLen--;
        }

        if (!*pStr1 && retVal == NULL) {
            retVal = res;
        } else {
            if (retVal == NULL)
                retVal = new NArray();

            retVal->append(res);

            pStr = pStr1;
        }
    } while (*pStr1);

    return 0;
}

result_t SQLite::get_fileName(exlib::string& retVal)
{
    if (!m_conn)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    retVal = m_file;
    return 0;
}

result_t SQLite::get_timeout(int32_t& retVal)
{
    if (!m_conn)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    retVal = m_nCmdTimeout;
    return 0;
}

result_t SQLite::set_timeout(int32_t newVal)
{
    if (!m_conn)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    m_nCmdTimeout = newVal;
    return 0;
}

result_t SQLite::backup(exlib::string fileName, AsyncEvent* ac)
{
    if (!m_conn)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_LONGSYNC);

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

    pBackup = sqlite3_backup_init(db2, "main", (sqlite3*)m_conn, "main");
    if (pBackup) {
        do {
            rc = sqlite3_backup_step(pBackup, 5);
            if (rc == SQLITE_LOCKED)
                coroutine_base::cc_sleep(1);
        } while (rc == SQLITE_OK || rc == SQLITE_BUSY || rc == SQLITE_LOCKED);

        sqlite3_backup_finish(pBackup);
    } else {
        result_t hr = CHECK_ERROR(Runtime::setError(sqlite3_errmsg((sqlite3*)m_conn)));
        sqlite3_close(db2);
        return hr;
    }

    sqlite3_close(db2);

    return 0;
}

} /* namespace fibjs */
