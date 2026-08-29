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

#define SQLITE_OPEN_FLAGS SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_NOMUTEX

result_t db_base::openSQLite(exlib::string connString,
    obj_ptr<SQLite_base>& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_LONGSYNC);

    result_t hr;

    const char* c_str = connString.c_str();

    if (!qstrcmp(c_str, "sqlite:", 7)) {
        c_str += 7;
        if (c_str[0] == '/' && c_str[1] == '/')
            c_str += 2;
    }

    obj_ptr<SQLite> db = new SQLite();
    hr = db->open(c_str);
    if (hr < 0)
        return hr;

    retVal = db;

    return 0;
}

result_t SQLite::open(const char* file)
{
    if (sqlite3_open_v2(file, (sqlite3**)&m_conn, SQLITE_OPEN_FLAGS, 0)) {
        result_t hr = CHECK_ERROR(Runtime::setError("%s: \"%s\"", sqlite3_errmsg((sqlite3*)m_conn), file));
        sqlite3_close((sqlite3*)m_conn);
        m_conn = NULL;
        return hr;
    }

    // Lock waiting is delegated to SQLite's official busy handler: file-lock
    // contention between processes or between connections in the same process
    // retries with backoff for up to `timeout` ms (SQLite runs on a dedicated
    // thread, so the event loop is unaffected).
    // Shared cache is not used: in WAL mode reads never block writes, and
    // writer-vs-writer is serialized by the WAL write lock, so table-level
    // locks are unnecessary.
    sqlite3_busy_timeout((sqlite3*)m_conn, m_nCmdTimeout);

    vec_init();

    // Per-connection PRAGMA settings (these three have no C API, SQL only).
    // Errors are ignored to keep the previous behavior: e.g. when the
    // filesystem does not support WAL, it silently falls back to rollback mode.
    sqlite3_exec((sqlite3*)m_conn,
        "PRAGMA journal_mode=WAL; PRAGMA synchronous=normal; PRAGMA temp_store=memory;",
        0, 0, 0);

    m_file = file;

    return 0;
}

SQLite::~SQLite()
{
    if (m_conn)
        async([conn = (sqlite3*)m_conn]() {
            // v2: statements not yet finalized are closed automatically on release
            sqlite3_close_v2(conn);
        });
}

result_t SQLite::get_type(exlib::string& retVal)
{
    retVal = "SQLite";
    return 0;
}

// Column value conversion (shared by execute and the Statement cursor)
void SQLite::columnValue(sqlite3_stmt* stmt, int32_t i, Variant& v)
{
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
}

// ---------------------------------------------------------------------------
// Statement cursor implementation
// ---------------------------------------------------------------------------

class SQLiteStmtImpl : public Statement::impl {
public:
    SQLiteStmtImpl(SQLite* db, sqlite3_stmt* stmt)
        : m_db(db)
        , m_stmt(stmt)
        , m_columns(sqlite3_column_count(stmt))
    {
        m_names.resize(m_columns);
        for (int32_t i = 0; i < m_columns; i++)
            m_names[i] = sqlite3_column_name(stmt, i);

        m_db->m_stmts.push_back(this);
    }

    ~SQLiteStmtImpl()
    {
        close();
    }

    virtual result_t open(std::vector<Variant>& args, bool& hasResult)
    {
        if (!m_stmt)
            return CHECK_ERROR(CALL_E_CLOSED);

        sqlite3_reset(m_stmt);
        sqlite3_clear_bindings(m_stmt);

        for (size_t i = 0; i < args.size(); i++) {
            result_t hr = bindValue(m_stmt, (int32_t)i + 1, args[i]);
            if (hr < 0)
                return hr;
        }

        hasResult = (m_columns > 0);
        if (!hasResult) {
            // Statements without a result set (INSERT/UPDATE/DELETE/DDL): run
            // to completion immediately, otherwise sqlite3_changes cannot
            // report the affected count of this statement.
            int32_t r = sqlite3_step(m_stmt);
            if (r != SQLITE_DONE) {
                if (r == SQLITE_ERROR)
                    return CHECK_ERROR(Runtime::setError(
                        sqlite3_errmsg((sqlite3*)m_db->m_conn)));
                return CHECK_ERROR(Runtime::setError(
                    "SQLite: statement execution failed"));
            }
        }

        m_db->m_activeStmt = 1;
        return 0;
    }

    virtual result_t fetchRow(NObject* row, bool& done)
    {
        if (!m_stmt)
            return CHECK_ERROR(CALL_E_CLOSED);

        int32_t r = sqlite3_step(m_stmt);
        if (r == SQLITE_ROW) {
            for (int32_t i = 0; i < m_columns; i++) {
                Variant v;
                SQLite::columnValue(m_stmt, i, v);
                row->add(m_names[i], v);
            }
            done = false;
            return 0;
        }

        done = true;
        if (r == SQLITE_DONE) {
            reset(); // cursor reset: impl is retained, Statement can be reused
            return 0;
        }

        result_t hr = CHECK_ERROR(Runtime::setError(sqlite3_errmsg((sqlite3*)m_db->m_conn)));
        return hr;
    }

    virtual result_t runResult(int64_t& changes, int64_t& lastInsertId)
    {
        changes = sqlite3_changes((sqlite3*)m_db->m_conn);
        lastInsertId = sqlite3_last_insert_rowid((sqlite3*)m_db->m_conn);
        return 0;
    }

    virtual result_t columns(obj_ptr<NArray>& retVal)
    {
        if (!m_stmt)
            return CHECK_ERROR(CALL_E_CLOSED);

        obj_ptr<NArray> arr = new NArray();
        for (int32_t i = 0; i < m_columns; i++) {
            obj_ptr<NObject> col = new NObject();
            col->add("name", m_names[i]);
            const char* type = sqlite3_column_decltype(m_stmt, i);
            col->add("type", type ? exlib::string(type) : exlib::string(""));
            arr->append(col);
        }

        retVal = arr;
        return 0;
    }

    // Cursor reset: called after each execute/iteration; the impl is retained
    // and can be open()ed again
    virtual void reset()
    {
        if (m_stmt)
            sqlite3_reset(m_stmt);
        m_db->m_activeStmt = 0;
    }

    // Full release (only called when the Statement is destroyed)
    virtual void close()
    {
        if (m_stmt) {
            sqlite3_finalize(m_stmt);
            m_stmt = NULL;
        }

        if (m_db) {
            std::vector<SQLiteStmtImpl*>& v = m_db->m_stmts;
            for (auto it = v.begin(); it != v.end(); ++it) {
                if (*it == this) {
                    v.erase(it);
                    break;
                }
            }

            m_db->m_activeStmt = 0;
            m_db = NULL;
        }
    }

private:
    static result_t bindValue(sqlite3_stmt* stmt, int32_t idx, Variant& v)
    {
        // Variant was already converted from v8 on the main thread (safe in a
        // fiber, no v8 access)
        switch (v.type()) {
        case Variant::VT_Integer:
            sqlite3_bind_int64(stmt, idx, v.intVal());
            break;

        case Variant::VT_Long:
            sqlite3_bind_int64(stmt, idx, v.longVal());
            break;

        case Variant::VT_Number: {
            double d = v.dblVal();
            int64_t i64 = (int64_t)d;
            if (d == (double)i64)
                sqlite3_bind_int64(stmt, idx, i64);
            else
                sqlite3_bind_double(stmt, idx, d);
            break;
        }

        case Variant::VT_Boolean:
            sqlite3_bind_int(stmt, idx, v.boolVal() ? 1 : 0);
            break;

        case Variant::VT_Undefined:
        case Variant::VT_Null:
            sqlite3_bind_null(stmt, idx);
            break;

        case Variant::VT_Date: {
            // stashArgs already converted Date to a SQL string on the main
            // thread; this is a defensive fallback
            exlib::string s;
            v.toString(s);
            sqlite3_bind_text(stmt, idx, s.c_str(), (int32_t)s.length(),
                SQLITE_TRANSIENT);
            break;
        }

        case Variant::VT_Object: {
            object_base* obj = v.object();
            if (obj && obj->class_info().isInstance(Buffer_base::class_info())) {
                obj_ptr<Buffer> buf = (Buffer*)obj;
                sqlite3_bind_blob(stmt, idx, buf->data(), (int32_t)buf->length(),
                    SQLITE_TRANSIENT);
                break;
            }
            exlib::string s;
            v.toString(s);
            sqlite3_bind_text(stmt, idx, s.c_str(), (int32_t)s.length(),
                SQLITE_TRANSIENT);
            break;
        }

        default: {
            exlib::string s;
            v.toString(s);
            sqlite3_bind_text(stmt, idx, s.c_str(), (int32_t)s.length(),
                SQLITE_TRANSIENT);
            break;
        }
        }

        return 0;
    }

private:
    SQLite* m_db;
    sqlite3_stmt* m_stmt;
    int32_t m_columns;
    std::vector<exlib::string> m_names;
};

result_t SQLite::prepareStmt(db_tmpl<SQLite_base, SQLite>* db,
    exlib::string sql, obj_ptr<Statement_base>& retVal)
{
    if (!db->m_conn)
        return CHECK_ERROR(Runtime::setError(CALL_E_INVALID_CALL, "SQLite: database is closed."));

    sqlite3_stmt* stmt = NULL;
    const char* pTail = NULL;

    if (sqlite3_prepare_v2((sqlite3*)db->m_conn, sql.c_str(),
            (int32_t)sql.length(), &stmt, &pTail)) {
        result_t hr = CHECK_ERROR(Runtime::setError(sqlite3_errmsg((sqlite3*)db->m_conn)));
        if (stmt)
            sqlite3_finalize(stmt);
        return hr;
    }

    if (!stmt)
        return CHECK_ERROR(Runtime::setError("SQLite: Query was empty"));

    // prepare accepts a single statement only
    while (qisspace(*pTail))
        pTail++;
    if (*pTail) {
        sqlite3_finalize(stmt);
        return CHECK_ERROR(Runtime::setError(CALL_E_INVALIDARG,
            "SQLite: prepare accepts a single statement only"));
    }

    obj_ptr<Statement> st = new Statement(sql, new SQLiteStmtImpl((SQLite*)db, stmt));
    retVal = st;
    return 0;
}

result_t SQLite::close(AsyncEvent* ac)
{
    if (!m_conn)
        return CHECK_ERROR(Runtime::setError(CALL_E_INVALID_CALL, "SQLite: database is closed."));

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_LONGSYNC);

    // Cascade-close active cursors (finalize statements not yet released) to
    // keep sqlite3_close from failing
    std::vector<SQLiteStmtImpl*> stmts;
    stmts.swap(m_stmts);
    for (size_t i = 0; i < stmts.size(); i++)
        stmts[i]->close();

    sqlite3_close((sqlite3*)m_conn);
    m_conn = NULL;
    m_activeStmt = 0;

    return 0;
}


result_t SQLite::execute(exlib::string sql, obj_ptr<NArray>& retVal, AsyncEvent* ac)
{
    if (!m_conn)
        return CHECK_ERROR(Runtime::setError(CALL_E_INVALID_CALL, "SQLite: database is closed."));

    if (m_activeStmt)
        return db_stmt_busy_error();

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_LONGSYNC);

    const char* pStr = sql.c_str();
    int32_t sLen = (int32_t)sql.length();
    const char* pStr1;

    do {
        sqlite3_stmt* stmt = 0;

        if (sqlite3_prepare_v2((sqlite3*)m_conn, pStr, sLen, &stmt, &pStr1)) {
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
                int32_t r = sqlite3_step(stmt);
                if (r == SQLITE_ROW) {
                    res->beginRow();
                    for (i = 0; i < columns; i++) {
                        Variant v;

                        columnValue(stmt, i, v);

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
            int32_t r = sqlite3_step(stmt);
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
        return CHECK_ERROR(Runtime::setError(CALL_E_INVALID_CALL, "SQLite: database is closed."));

    retVal = m_file;
    return 0;
}

result_t SQLite::get_timeout(int32_t& retVal)
{
    if (!m_conn)
        return CHECK_ERROR(Runtime::setError(CALL_E_INVALID_CALL, "SQLite: database is closed."));

    retVal = m_nCmdTimeout;
    return 0;
}

result_t SQLite::set_timeout(int32_t newVal)
{
    if (!m_conn)
        return CHECK_ERROR(Runtime::setError(CALL_E_INVALID_CALL, "SQLite: database is closed."));

    m_nCmdTimeout = newVal;

    // Sync the busy handler wait budget (0 means no waiting, immediate BUSY)
    sqlite3_busy_timeout((sqlite3*)m_conn, newVal);

    return 0;
}

result_t SQLite::backup(exlib::string fileName, AsyncEvent* ac)
{
    if (!m_conn)
        return CHECK_ERROR(Runtime::setError(CALL_E_INVALID_CALL, "SQLite: database is closed."));

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_LONGSYNC);

    int32_t rc;
    struct sqlite3* db2 = NULL;
    sqlite3_backup* pBackup;

    const char* c_str = fileName.c_str();
    if (!qstrcmp(c_str, "sqlite:", 7)) {
        c_str += 7;
        if (c_str[0] == '/' && c_str[1] == '/')
            c_str += 2;
    }

    if (sqlite3_open_v2(c_str, &db2, SQLITE_OPEN_FLAGS, 0)) {
        result_t hr = CHECK_ERROR(Runtime::setError("%s: \"%s\"", sqlite3_errmsg(db2), c_str));
        return hr;
    }

    // backup_step's file-lock waiting goes through the destination connection's
    // busy handler
    sqlite3_busy_timeout(db2, m_nCmdTimeout);

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

result_t SQLite::getTables(obj_ptr<NArray>& retVal, AsyncEvent* ac)
{
    if (!m_conn)
        return CHECK_ERROR(Runtime::setError(CALL_E_INVALID_CALL, "SQLite: database is closed."));

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_LONGSYNC);

    // Query SQLite system table to get all user tables
    exlib::string sql = "SELECT name FROM sqlite_master WHERE type='table' AND name NOT LIKE 'sqlite_%' ORDER BY name";
    return execute(sql, retVal, ac);
}

result_t SQLite::getTableInfo(exlib::string tableName, obj_ptr<NArray>& retVal, AsyncEvent* ac)
{
    if (!m_conn)
        return CHECK_ERROR(Runtime::setError(CALL_E_INVALID_CALL, "SQLite: database is closed."));

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_LONGSYNC);

    // Query SQLite table structure and format to match information_schema style
    exlib::string escapedTableName = escape_string(tableName);
    exlib::string sql = "SELECT "
                        "name AS column_name, "
                        "type AS data_type, "
                        "NULL AS character_maximum_length, "
                        "CASE WHEN \"notnull\" = 0 THEN 'YES' ELSE 'NO' END AS is_nullable, "
                        "dflt_value AS column_default "
                        "FROM pragma_table_info("
        + escapedTableName + ") "
                             "ORDER BY cid";
    return execute(sql, retVal, ac);
}

} /* namespace fibjs */
