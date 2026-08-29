/*
 * SQLite.h
 *
 *  Created on: Aug 1, 2012
 *      Author: lion
 */

#pragma once

#include "ifs/SQLite.h"
#include "../Statement.h"
#include <sqlite/sqlite3.h>
#include <vector>
#include "../db_tmpl.h"

namespace fibjs {

class SQLiteStmtImpl;

class SQLite : public db_tmpl<SQLite_base, SQLite> {
public:
    SQLite()
        : m_nCmdTimeout(5000)
    {
    }

    ~SQLite();

public:
    // DbConnection_base
    virtual result_t get_type(exlib::string& retVal);
    virtual result_t close(AsyncEvent* ac);
    virtual result_t execute(exlib::string sql, obj_ptr<NArray>& retVal, AsyncEvent* ac);
    virtual result_t begin(exlib::string point, AsyncEvent* ac)
    {
        if (!m_conn)
            return CHECK_ERROR(CALL_E_INVALID_CALL);

        if (ac->isSync())
            return CHECK_ERROR(CALL_E_LONGSYNC);

        obj_ptr<NArray> retVal;

        // Write transactions use BEGIN IMMEDIATE: with deferred BEGIN in WAL mode,
        // a "read then write" lock upgrade bypasses the busy handler and fails
        // immediately with BUSY/BUSY_SNAPSHOT (SQLite recommends IMMEDIATE for
        // write transactions; the Django sqlite backend does the same).
        if (point.empty())
            return execute("BEGIN IMMEDIATE", retVal, ac);

        exlib::string str("SAVEPOINT " + point);
        return execute(str, retVal, ac);
    }
    virtual result_t getTables(obj_ptr<NArray>& retVal, AsyncEvent* ac);
    virtual result_t getTableInfo(exlib::string tableName, obj_ptr<NArray>& retVal, AsyncEvent* ac);

public:
    // SQLite_base
    virtual result_t get_fileName(exlib::string& retVal);
    virtual result_t get_timeout(int32_t& retVal);
    virtual result_t set_timeout(int32_t newVal);
    virtual result_t backup(exlib::string fileName, AsyncEvent* ac);

public:
    result_t open(const char* file);
    int vec_init();

    // Engine implementation: create a prepared statement (compilation happens
    // at prepare time)
    static result_t prepareStmt(db_tmpl<SQLite_base, SQLite>* db,
        exlib::string sql, obj_ptr<Statement_base>& retVal);

    // Column type conversion (shared by execute and the cursor)
    static void columnValue(sqlite3_stmt* stmt, int32_t i, Variant& v);

public:
    static exlib::string escape_binary(Buffer* bin)
    {
        exlib::string retVal;
        exlib::string s;

        bin->hex(s);

        retVal.append("x\'", 2);
        retVal.append(s);
        retVal += '\'';

        return retVal;
    }

private:
    friend class SQLiteStmtImpl;
    void finalizeStmt(sqlite3_stmt* stmt);

private:
    exlib::string m_file;
    int32_t m_nCmdTimeout;
    std::vector<SQLiteStmtImpl*> m_stmts; // active cursors (closed in cascade on connection close)
};

} /* namespace fibjs */
