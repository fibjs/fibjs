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
        : m_nCmdTimeout(10000)
    {
    }

    ~SQLite();

public:
    // DbConnection_base
    virtual result_t get_type(exlib::string& retVal);
    virtual result_t close(AsyncEvent* ac);
    virtual result_t execute(exlib::string sql, obj_ptr<NArray>& retVal, AsyncEvent* ac);
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

    // 引擎实现：创建预编译语句（编译在 prepare 时完成）
    static result_t prepareStmt(db_tmpl<SQLite_base, SQLite>* db,
        exlib::string sql, obj_ptr<Statement_base>& retVal);

    // 列类型转换（execute 与游标共用）
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
    std::vector<SQLiteStmtImpl*> m_stmts; // 活跃游标（连接关闭时级联 close）
};

} /* namespace fibjs */
