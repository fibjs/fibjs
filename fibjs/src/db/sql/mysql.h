/*
 * mysql.h
 *
 *  Created on: Jul 20, 2012
 *      Author: lion
 */

#pragma once

#include "ifs/MySQL.h"
extern "C" {
#include <umysql/include/umysql.h>
}
#include <umysql/src/Connection.h>
#include "../db_tmpl.h"

namespace fibjs {

class MySQLStmtImpl;

class mysql : public db_tmpl<MySQL_base, mysql> {
public:
    virtual ~mysql();

public:
    // DbConnection_base
    virtual result_t get_type(exlib::string& retVal);
    virtual result_t close(AsyncEvent* ac);
    virtual result_t execute(exlib::string sql, obj_ptr<NArray>& retVal, AsyncEvent* ac);
    virtual result_t getTables(obj_ptr<NArray>& retVal, AsyncEvent* ac);
    virtual result_t getTableInfo(exlib::string tableName, obj_ptr<NArray>& retVal, AsyncEvent* ac);

public:
    // MySQL_base
    virtual result_t get_rxBufferSize(int32_t& retVal);
    virtual result_t set_rxBufferSize(int32_t newVal);
    virtual result_t get_txBufferSize(int32_t& retVal);
    virtual result_t set_txBufferSize(int32_t newVal);

public:
    result_t connect(const char* host, int32_t port, const char* username,
        const char* password, const char* dbName);

    // Engine implementation: create a prepared statement. v1 uses the text
    // protocol + client-side escaped binding (Variant args are escaped into the
    // SQL when the Statement opens, then driven through the streaming state
    // machine).
    static result_t prepareStmt(db_tmpl<MySQL_base, mysql>* db,
        exlib::string sql, obj_ptr<Statement_base>& retVal);

    // Column type conversion (shared by execute callbacks and Statement cursor)
    static void columnValue(const UMTypeInfo* ti, const UINT8* value,
        size_t cbValue, Variant& v);

public:
    static exlib::string escape_string(exlib::string v)
    {
        const char* str = v.c_str();
        int32_t sz = (int32_t)v.length();
        int32_t len, l;
        const char* src;
        char* bstr;
        char ch;
        exlib::string retVal;

        for (len = 0, src = str, l = sz; l > 0; len++, l--) {
            ch = (unsigned char)*src++;

            if (ch == '\'' || ch == '\\')
                len++;
        }

        retVal.resize(len + 2);

        bstr = retVal.data();
        *bstr++ = '\'';

        for (src = str, l = sz; l > 0; l--) {
            ch = (unsigned char)*src++;

            if (ch == '\'' || ch == '\\')
                *bstr++ = ch;
            *bstr++ = ch;
        }

        *bstr++ = '\'';

        return retVal;
    }

    static exlib::string escape_date(v8::Local<v8::Value>& v)
    {
        exlib::string retVal;
        exlib::string s;

        retVal.append("TIMESTAMP(\'", 11);

        date_t d = v;
        d.sqlString(s);
        retVal.append(s);

        retVal.append("\')", 2);

        return retVal;
    }

private:
    friend class MySQLStmtImpl;

    inline result_t error()
    {
        const char* errorMessage = NULL;
        int32_t errCode;
        int32_t errType;

        if (UMConnection_GetLastError(m_conn, &errorMessage, &errCode,
                &errType))
            return Runtime::setError(errorMessage);
        return Runtime::errNumber();
    }
};

} /* namespace fibjs */
