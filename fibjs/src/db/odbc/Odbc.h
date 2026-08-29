/*
 * Odbc.h
 *
 *  Created on: May 9, 2021
 *      Author: lion
 */

#pragma once

#include "ifs/DbConnection.h"
#include "../db_tmpl.h"

namespace fibjs {

struct OdbcConnectOptions {
    const char* serverAttr;
    bool useHostPort;
    bool trustCertificate;
};

result_t odbc_connect(exlib::string connString, const char* driver, int32_t port, void*& conn,
    const OdbcConnectOptions* options = NULL);
result_t odbc_disconnect(void* conn);
result_t odbc_close(void*& conn, AsyncEvent* ac);
result_t odbc_execute(void* conn, int32_t* activeStmt, exlib::string sql, obj_ptr<NArray>& retVal, AsyncEvent* ac);
result_t odbc_getTables(void* conn, obj_ptr<NArray>& retVal, AsyncEvent* ac);
result_t odbc_getTableInfo(void* conn, exlib::string tableName, obj_ptr<NArray>& retVal, AsyncEvent* ac);
result_t odbc_set_autocommit(void* conn, bool on);

// ODBC Statement: engine escape callbacks (each engine has different SQL
// literal rules)
struct OdbcEscape {
    exlib::string (*string)(exlib::string);
    exlib::string (*binary)(Buffer*);
};

// Create an ODBC prepared statement (v1: SQLPrepare + client-side escaped
// binding, no server-side parameter binding)
result_t odbc_prepareStmt(void* conn, int32_t* activeStmt, exlib::string sql,
    const OdbcEscape& esc, obj_ptr<Statement_base>& retVal);

class Odbc_tmpl : public DbConnection_base {
public:
    Odbc_tmpl()
    {
    }
};

class Odbc : public db_tmpl<Odbc_tmpl, Odbc> {
public:
    ~Odbc()
    {
        async([conn = m_conn]() {
            odbc_disconnect(conn);
        });
    }

    // Generic SQL literal escaping (' → ''; binary → 0x hex), shared by engines
    static exlib::string escape_string(exlib::string v)
    {
        exlib::string retVal;
        const char* str = v.c_str();
        int32_t sz = (int32_t)v.length();

        retVal.append(1, '\'');
        for (int32_t i = 0; i < sz; i++) {
            if (str[i] == '\'')
                retVal.append(1, '\'');
            retVal.append(1, str[i]);
        }
        retVal.append(1, '\'');
        return retVal;
    }

    static exlib::string escape_binary(Buffer* bin)
    {
        exlib::string retVal;
        exlib::string s;

        bin->hex(s);
        retVal.append("0x", 2);
        retVal.append(s);
        return retVal;
    }

    static result_t prepareStmt(db_tmpl<Odbc_tmpl, Odbc>* db,
        exlib::string sql, obj_ptr<Statement_base>& retVal)
    {
        static const OdbcEscape esc = { escape_string, escape_binary };
        return odbc_prepareStmt(db->m_conn, &db->m_activeStmt, sql, esc, retVal);
    }

public:
    // DbConnection_base
    virtual result_t get_type(exlib::string& retVal)
    {
        retVal = "odbc";
        return 0;
    }

    virtual result_t close(AsyncEvent* ac)
    {
        return odbc_close(m_conn, ac);
    }

    virtual result_t execute(exlib::string sql, obj_ptr<NArray>& retVal, AsyncEvent* ac)
    {
        return odbc_execute(m_conn, &m_activeStmt, sql, retVal, ac);
    }

    virtual result_t getTables(obj_ptr<NArray>& retVal, AsyncEvent* ac)
    {
        return odbc_getTables(m_conn, retVal, ac);
    }

    virtual result_t getTableInfo(exlib::string tableName, obj_ptr<NArray>& retVal, AsyncEvent* ac)
    {
        return odbc_getTableInfo(m_conn, tableName, retVal, ac);
    }
};

} /* namespace fibjs */
