/*
 * psql.h
 *
 *  Created on: Oct 29, 2016
 *      Author: lion
 */

#pragma once

#include "ifs/DbConnection.h"
#include "utf8.h"

#include "../odbc/Odbc.h"

namespace fibjs {

class psql : public db_tmpl<Odbc_tmpl, psql> {
public:
    ~psql()
    {
        if (m_conn)
            async([conn = m_conn]() {
                odbc_disconnect(conn);
            });
    }

    static result_t prepareStmt(db_tmpl<Odbc_tmpl, psql>* db,
        exlib::string sql, obj_ptr<Statement_base>& retVal)
    {
        // psql 字符串用 SQL 标准转义（' → ''）；二进制用 \x hex
        static const OdbcEscape esc = { Odbc::escape_string, escape_binary };
        return odbc_prepareStmt(db->m_conn, &db->m_activeStmt, sql, esc, retVal);
    }

public:
    // DbConnection_base
    virtual result_t get_type(exlib::string& retVal)
    {
        retVal = "psql";
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

public:
    static exlib::string escape_binary(Buffer* bin)
    {
        exlib::string retVal;
        exlib::string s;

        bin->hex(s);

        retVal.append("\'\\x", 3);
        retVal.append(s);
        retVal.append("\'::BYTEA", 8);

        return retVal;
    }

    static exlib::string escape_date(v8::Local<v8::Value>& v)
    {
        exlib::string retVal;
        exlib::string s;

        retVal.append("TIMESTAMP \'", 11);

        date_t d = v;
        d.sqlString(s);
        retVal.append(s);

        retVal.append("\'", 1);

        return retVal;
    }
};

} /* namespace fibjs */
