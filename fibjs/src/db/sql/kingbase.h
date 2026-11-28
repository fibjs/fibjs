/*
 * kingbase.h
 *
 * KingbaseES engine: the PostgreSQL dialect carried by the KingbaseES ODBC
 * driver (KODBC).
 */

#pragma once

#include "ifs/DbConnection.h"
#include "utf8.h"

#include "../odbc/Odbc.h"
#include "pg_escape.h"

namespace fibjs {

class kingbase : public db_tmpl<Odbc_tmpl, kingbase>, public pg_escape {
public:
    using pg_escape::escape_binary;
    using pg_escape::escape_date;

    ~kingbase()
    {
        if (m_conn)
            async([conn = m_conn]() {
                odbc_disconnect(conn);
            });
    }

    static result_t prepareStmt(db_tmpl<Odbc_tmpl, kingbase>* db,
        exlib::string sql, obj_ptr<Statement_base>& retVal)
    {
        static const OdbcEscape esc = { Odbc::escape_string, escape_binary,
            escape_bool, escape_date_value };
        return odbc_prepareStmt(db->m_conn, &db->m_activeStmt, sql, esc, retVal);
    }

public:
    // DbConnection_base
    virtual result_t get_type(exlib::string& retVal)
    {
        retVal = "kingbase";
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

    // KODBC runs with ODBC autocommit on, and the V9 driver does not keep an
    // explicit BEGIN open: turn autocommit off for the transaction instead
    // (the same model as the dm engine).
    virtual result_t begin(exlib::string point, AsyncHandle ac)
    {
        if (ac.isSync())
            return result_t::longsync();

        if (!m_conn)
            return result_t::invalid_call();

        if (m_activeStmt)
            return db_stmt_busy_error();

        if (point.empty())
            return odbc_set_autocommit(m_conn, false);

        obj_ptr<NArray> retVal;
        exlib::string str("SAVEPOINT " + point);
        return execute(str, retVal, std::move(ac));
    }

    virtual result_t commit(exlib::string point, AsyncHandle ac)
    {
        if (ac.isSync())
            return result_t::longsync();

        if (!m_conn)
            return result_t::invalid_call();

        if (m_activeStmt)
            return db_stmt_busy_error();

        obj_ptr<NArray> retVal;

        if (point.empty()) {
            result_t r = execute("COMMIT", retVal, std::move(ac));
            if (r.failed())
                return r;

            return odbc_set_autocommit(m_conn, true);
        }

        exlib::string str("RELEASE SAVEPOINT " + point);
        return execute(str, retVal, std::move(ac));
    }

    virtual result_t rollback(exlib::string point, AsyncHandle ac)
    {
        if (ac.isSync())
            return result_t::longsync();

        if (!m_conn)
            return result_t::invalid_call();

        if (m_activeStmt)
            return db_stmt_busy_error();

        obj_ptr<NArray> retVal;

        if (point.empty()) {
            result_t r = execute("ROLLBACK", retVal, std::move(ac));
            if (r.failed())
                return r;

            return odbc_set_autocommit(m_conn, true);
        }

        exlib::string str("ROLLBACK TO SAVEPOINT " + point);
        return execute(str, retVal, std::move(ac));
    }
};

} /* namespace fibjs */
