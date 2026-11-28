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
#include "pg_escape.h"

namespace fibjs {

class psql : public db_tmpl<Odbc_tmpl, psql>, public pg_escape {
public:
    using pg_escape::escape_binary;
    using pg_escape::escape_date;

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
        // psql strings use SQL standard escaping (' → ''); binaries use \x hex
        static const OdbcEscape esc = { Odbc::escape_string, escape_binary,
            escape_bool, escape_date_value };
        return odbc_prepareStmt(db->m_conn, &db->m_activeStmt, sql, esc, retVal);
    }

public:
    // DbConnection_base
    virtual result_t get_type(exlib::string& retVal)
    {
        retVal = "psql";
        return result_t();
    }

    virtual result_t close(AsyncHandle ac)
    {
        return odbc_close(m_conn, std::move(ac));
    }

    virtual result_t execute(exlib::string sql, obj_ptr<NArray>& retVal, AsyncHandle ac)
    {
        return odbc_execute(m_conn, &m_activeStmt, sql, retVal, std::move(ac));
    }

    virtual result_t getTables(obj_ptr<NArray>& retVal, AsyncHandle ac)
    {
        return odbc_getTables(m_conn, retVal, std::move(ac));
    }

    virtual result_t getTableInfo(exlib::string tableName, obj_ptr<NArray>& retVal, AsyncHandle ac)
    {
        return odbc_getTableInfo(m_conn, tableName, retVal, std::move(ac));
    }
};

} /* namespace fibjs */
