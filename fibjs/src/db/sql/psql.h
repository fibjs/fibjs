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
            asyncCall(odbc_disconnect, m_conn);
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
        return odbc_execute(m_conn, sql, retVal, ac, m_codec);
    }

public:
    static exlib::string escape_binary(Buffer_base* bin)
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

    static exlib::string escape_field(const char* str, int32_t sz,
        char quote_left = '\"', char quote_right = '\"')
    {
        return db_tmpl<DbConnection_base, psql>::escape_field(str, sz, quote_left, quote_right);
    }

public:
    static const DataType& data_type()
    {
        static DataType _data_type = {
            "REAL",
            "FLOAT",
            "TIMESTAMP",
            "TEXT",
            "BYTEA",
            "BYTEA"
        };

        return _data_type;
    }
};

} /* namespace fibjs */
