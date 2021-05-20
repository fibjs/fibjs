/*
 * Odbc.h
 *
 *  Created on: May 9, 2021
 *      Author: lion
 */

#pragma once

#include "ifs/Odbc.h"
#include "../db_tmpl.h"

namespace fibjs {

result_t odbc_connect(const char* driver, const char* host, int32_t port, const char* username,
    const char* password, const char* dbName, void*& conn);
result_t odbc_disconnect(void* conn);
result_t odbc_close(void*& conn, AsyncEvent* ac);
result_t odbc_execute(void* conn, exlib::string sql, obj_ptr<NArray>& retVal, AsyncEvent* ac, exlib::string codec);

class Odbc_tmpl : public Odbc_base {
public:
    Odbc_tmpl()
        : m_codec("utf8")
    {
    }

public:
    // Odbc_base
    virtual result_t get_codec(exlib::string& retVal)
    {
        retVal = m_codec;
        return 0;
    }

    virtual result_t set_codec(exlib::string newVal)
    {
        m_codec = newVal;
        return 0;
    }

protected:
    exlib::string m_codec;
};

class Odbc : public db_tmpl<Odbc_tmpl, Odbc> {
public:
    ~Odbc()
    {
        if (m_conn)
            asyncCall(odbc_disconnect, m_conn);
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
        return odbc_execute(m_conn, sql, retVal, ac, m_codec);
    }
};

} /* namespace fibjs */
