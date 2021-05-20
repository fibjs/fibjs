/*
 * mssql.h
 *
 *  Created on: Oct 29, 2016
 *      Author: lion
 */

#pragma once

#include "ifs/DbConnection.h"
#include "utf8.h"

#include "../odbc/Odbc.h"

namespace fibjs {

class mssql : public db_tmpl<Odbc_tmpl, mssql> {
public:
    ~mssql()
    {
        if (m_conn)
            asyncCall(odbc_disconnect, m_conn);
    }

public:
    // DbConnection_base
    virtual result_t get_type(exlib::string& retVal)
    {
        retVal = "mssql";
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

    virtual result_t begin(exlib::string point, AsyncEvent* ac)
    {
        if (!m_conn)
            return CHECK_ERROR(CALL_E_INVALID_CALL);

        if (ac->isSync())
            return CHECK_ERROR(CALL_E_LONGSYNC);

        obj_ptr<NArray> retVal;

        if (point.empty())
            return execute("BEGIN TRANSACTION", retVal, ac);

        exlib::string str("SAVE TRANSACTION " + point);
        return execute(str, retVal, ac);
    }

    virtual result_t commit(exlib::string point, AsyncEvent* ac)
    {
        if (!m_conn)
            return CHECK_ERROR(CALL_E_INVALID_CALL);

        if (ac->isSync())
            return CHECK_ERROR(CALL_E_LONGSYNC);

        obj_ptr<NArray> retVal;

        if (point.empty())
            return execute("COMMIT", retVal, ac);
        return 0;
    }

    virtual result_t rollback(exlib::string point, AsyncEvent* ac)
    {
        if (!m_conn)
            return CHECK_ERROR(CALL_E_INVALID_CALL);

        if (ac->isSync())
            return CHECK_ERROR(CALL_E_LONGSYNC);

        obj_ptr<NArray> retVal;

        if (point.empty())
            return execute("ROLLBACK", retVal, ac);

        exlib::string str("ROLLBACK TRANSACTION " + point);
        return execute(str, retVal, ac);
    }

public:
    static exlib::string escape_binary(Buffer_base* bin)
    {
        exlib::string retVal;
        exlib::string s;

        bin->hex(s);

        retVal.append("0x", 2);
        retVal.append(s);

        return retVal;
    }

    static exlib::string escape_date(v8::Local<v8::Value>& v)
    {
        exlib::string retVal;
        exlib::string s;

        retVal.append("CAST(\'", 6);

        date_t d = v;
        d.sqlString(s);
        retVal.append(s);

        retVal.append("\' AS datetime)", 14);

        return retVal;
    }

    static exlib::string escape_field(const char* str, int32_t sz,
        char quote_left = '[', char quote_right = ']')
    {
        return db_tmpl<DbConnection_base, mssql>::escape_field(str, sz, quote_left, quote_right);
    }

public:
    static const DataType& data_type()
    {
        static DataType _data_type = {
            "REAL",
            "VARCHAR(MAX)",
            "VARBINARY(MAX)",
            "IMAGE"
        };

        return _data_type;
    }
};

} /* namespace fibjs */
