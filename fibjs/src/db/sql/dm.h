/*
 * dm.h
 *
 *  Created on: Feb 11, 2026
 *      Author: lion
 */

#pragma once

#include "ifs/DbConnection.h"
#include "utf8.h"

#include "../odbc/Odbc.h"

namespace fibjs {

class dm : public db_tmpl<Odbc_tmpl, dm> {
public:
    ~dm()
    {
        if (m_conn)
            async([conn = m_conn]() {
                odbc_disconnect(conn);
            });
    }

    static result_t prepareStmt(db_tmpl<Odbc_tmpl, dm>* db,
        exlib::string sql, obj_ptr<Statement_base>& retVal)
    {
        static const OdbcEscape esc = { Odbc::escape_string, Odbc::escape_binary };
        return odbc_prepareStmt(db->m_conn, &db->m_activeStmt, sql, esc, retVal);
    }

public:
    // DbConnection_base
    virtual result_t get_type(exlib::string& retVal)
    {
        retVal = "dm";
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

    virtual result_t begin(exlib::string point, AsyncEvent* ac)
    {
        if (!m_conn)
            return CHECK_ERROR(CALL_E_INVALID_CALL);

        if (ac->isSync())
            return CHECK_ERROR(CALL_E_LONGSYNC);

        obj_ptr<NArray> retVal;

        if (point.empty())
            return odbc_set_autocommit(m_conn, false);

        exlib::string str("SAVEPOINT " + point);
        return execute(str, retVal, ac);
    }

    virtual result_t commit(exlib::string point, AsyncEvent* ac)
    {
        if (!m_conn)
            return CHECK_ERROR(CALL_E_INVALID_CALL);

        if (ac->isSync())
            return CHECK_ERROR(CALL_E_LONGSYNC);

        obj_ptr<NArray> retVal;

        if (point.empty()) {
            result_t r = execute("COMMIT", retVal, ac);
            odbc_set_autocommit(m_conn, true);
            return r;
        }

        // DM does not support RELEASE SAVEPOINT, just ignore it
        return 0;
    }

    virtual result_t rollback(exlib::string point, AsyncEvent* ac)
    {
        if (!m_conn)
            return CHECK_ERROR(CALL_E_INVALID_CALL);

        if (ac->isSync())
            return CHECK_ERROR(CALL_E_LONGSYNC);

        obj_ptr<NArray> retVal;

        if (point.empty()) {
            result_t r = execute("ROLLBACK", retVal, ac);
            odbc_set_autocommit(m_conn, true);
            return r;
        }

        exlib::string str("ROLLBACK TO SAVEPOINT " + point);
        return execute(str, retVal, ac);
    }

    static exlib::string escape_date(v8::Local<v8::Value>& v)
    {
        exlib::string retVal;
        exlib::string s;

        retVal.append("CAST('", 6);

        date_t d = v;
        d.sqlString(s);
        retVal.append(s);

        retVal.append("' AS DATETIME)", 14);

        return retVal;
    }
};

} /* namespace fibjs */
