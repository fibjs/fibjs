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
result_t odbc_execute(void* conn, exlib::string sql, obj_ptr<NArray>& retVal, AsyncEvent* ac);
result_t odbc_getTables(void* conn, obj_ptr<NArray>& retVal, AsyncEvent* ac);
result_t odbc_getTableInfo(void* conn, exlib::string tableName, obj_ptr<NArray>& retVal, AsyncEvent* ac);
result_t odbc_set_autocommit(void* conn, bool on);

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
        return odbc_execute(m_conn, sql, retVal, ac);
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
