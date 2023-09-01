/*
 * odbc.cpp
 *
 *  Created on: May 22, 2021
 *      Author: lion
 */

#include "object.h"
#include "dl_odbc.h"
#include "Odbc.h"
#include "Buffer.h"
#include "ifs/db.h"
#include "DBResult.h"
#include "Url.h"
#include "encoding_iconv.h"
#include <stdio.h>
#include <stdlib.h>

#define SQL_SS_VARIANT (-150)

namespace fibjs {

void* g_odbc;

result_t db_base::openOdbc(exlib::string connString, obj_ptr<DbConnection_base>& retVal,
    AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_LONGSYNC);

    if (qstrcmp(connString.c_str(), "odbc:", 5))
        return CHECK_ERROR(CALL_E_INVALIDARG);

    obj_ptr<Odbc> conn = new Odbc();

    result_t hr = odbc_connect(connString, NULL, -1, conn->m_conn);
    if (hr < 0)
        return hr;

    retVal = conn;
    return 0;
}

result_t odbc_disconnect(void* conn)
{
    if (conn) {
        SQLDisconnect(conn);
        SQLFreeConnect(conn);
    }

    return 0;
}

result_t odbc_close(void*& conn, AsyncEvent* ac)
{
    if (!conn)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_LONGSYNC);

    odbc_disconnect(conn);
    conn = NULL;

    return 0;
}

exlib::string odbc_error(int32_t handleType, void* handle)
{
    SQLRETURN hr;
    SQLSMALLINT len;
    SQLINTEGER statusRecCount;
    exlib::string result;

    SQLGetDiagField(handleType, handle, 0, SQL_DIAG_NUMBER, &statusRecCount, SQL_IS_INTEGER, &len);
    for (int32_t i = 0; i < statusRecCount; i++) {
        char errorSQLState[14];
        char errorMessage[2048];
        SQLINTEGER native;
        hr = SQLGetDiagRec(handleType, handle, (SQLSMALLINT)(i + 1), (SQLTCHAR*)errorSQLState, &native,
            (SQLTCHAR*)errorMessage, sizeof(errorMessage), &len);
        if (hr == SQL_NO_DATA || hr < 0)
            break;

        if (i > 1)
            result.append("\n    ", 5);
        result.append(errorMessage);
    }

    return result;
}

exlib::string safe_conn_string(const char* str)
{
    bool b_safe = true;
    int32_t esc_cnt = 0;
    int32_t len = 0;
    char ch;

    while ((ch = str[len]) != 0) {
        if (ch == '}')
            esc_cnt++;
        else if (ch == ';')
            b_safe = false;
        len++;
    }

    if (b_safe)
        return str;

    exlib::string new_str;
    new_str.resize(len + esc_cnt + 2);
    char* ptr = new_str.c_buffer();
    len = 0;

    *ptr++ = '{';
    while ((ch = str[len]) != 0) {
        *ptr++ = ch;
        if (ch == '}')
            *ptr++ = ch;

        len++;
    }
    *ptr++ = '}';

    return new_str;
}

result_t odbc_connect(const char* driver, const char* host, int32_t port, const char* username,
    const char* password, const char* dbName, void*& conn)
{
    if (conn)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    SQLRETURN hr;
    if (!g_odbc) {
        hr = SQLAllocEnv((SQLHENV*)&g_odbc);
        if (hr < 0)
            return CHECK_ERROR(Runtime::setError("odbc: unable to allocate environment."));
    }

    hr = SQLAllocConnect(g_odbc, (SQLHDBC*)&conn);
    if (hr < 0) {
        odbc_disconnect(conn);
        conn = NULL;
        return CHECK_ERROR(Runtime::setError("odbc: unable to allocate connection."));
    }

    exlib::string conn_str;

    if (*driver) {
        conn_str.append("Driver=");
        conn_str.append(safe_conn_string(driver));
        conn_str.append(1, ';');

        conn_str.append("Server=");
        conn_str.append(safe_conn_string(host));
        conn_str.append(1, ';');

        if (port > 0) {
            char str_buf[32];

            snprintf(str_buf, sizeof(str_buf), "%d", port);
            conn_str.append("Port=");
            conn_str.append(safe_conn_string(str_buf));
            conn_str.append(1, ';');
        }

        if (*dbName) {
            conn_str.append("Database=");
            conn_str.append(safe_conn_string(dbName));
            conn_str.append(1, ';');
        }

        if (*username) {
            conn_str.append("Uid=");
            conn_str.append(safe_conn_string(username));
            conn_str.append(1, ';');
        }

        if (*password) {
            conn_str.append("Pwd=");
            conn_str.append(safe_conn_string(password));
            conn_str.append(1, ';');
        }
    }

    hr = SQLDriverConnect(conn, NULL, (SQLCHAR*)conn_str.c_str(), (SQLSMALLINT)conn_str.length(),
        NULL, 0, NULL, SQL_DRIVER_NOPROMPT);
    if (hr < 0) {
        exlib::string err = odbc_error(SQL_HANDLE_DBC, conn);
        odbc_disconnect(conn);
        conn = NULL;
        return CHECK_ERROR(Runtime::setError(err));
    }

    return 0;
}

result_t odbc_connect(exlib::string connString, const char* driver, int32_t port, void*& conn)
{
    obj_ptr<Url> u = new Url();

    result_t hr = u->parse(connString);
    if (hr < 0)
        return hr;

    obj_ptr<HttpCollection_base> q;
    u->get_searchParams(q);
    Variant v;

    if (u->m_port.length() > 0)
        port = atoi(u->m_port.c_str());

    exlib::string str;
    hr = q->first("Driver", v);
    if (hr != CALL_RETURN_NULL) {
        str = v.string();
        driver = str.c_str();
    } else if (driver == NULL)
        return CHECK_ERROR(Runtime::setError("odbc: no driver specified."));

    return odbc_connect(driver, u->m_hostname.c_str(), port,
        u->m_username.c_str(), u->m_password.c_str(),
        u->m_pathname.length() > 0 ? u->m_pathname.c_str() + 1 : "", conn);
}

result_t odbc_execute(void* conn, exlib::string sql, obj_ptr<NArray>& retVal, AsyncEvent* ac, exlib::string codec)
{
    if (!conn)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_LONGSYNC);

    SQLRETURN hr;
    SQLHSTMT stmt;

    hr = SQLAllocStmt(conn, &stmt);
    if (hr < 0)
        return CHECK_ERROR(Runtime::setError(odbc_error(SQL_HANDLE_DBC, conn)));

    do {
        bool more = false;

        if (codec == "utf8" || codec == "utf-8") {
            hr = SQLExecDirect(stmt, (SQLCHAR*)sql.c_str(), (SQLINTEGER)sql.length());
        } else {
            exlib::wstring wsql(utf8to16String(sql));
            hr = SQLExecDirectW(stmt, (SQLWCHAR*)wsql.c_str(), (SQLINTEGER)wsql.length());
        }
        if (hr < 0)
            break;

        do {
            obj_ptr<DBResult> res;
            std::vector<SQLLEN> types;
            SQLLEN len;
            SQLSMALLINT columns = 0;
            SQLLEN affected;

            hr = SQLNumResultCols(stmt, &columns);
            if (hr < 0)
                break;

            hr = SQLRowCount(stmt, &affected);
            if (hr < 0)
                break;

            types.resize(columns);
            res = new DBResult(columns, affected);
            for (int32_t i = 0; i < columns; i++) {
                char buf[SQL_MAX_COLUMN_NAME_LEN];
                SQLSMALLINT buflen;

                hr = SQLColAttributes(stmt, i + 1, SQL_DESC_NAME, buf, SQL_MAX_COLUMN_NAME_LEN, &buflen, NULL);
                if (hr < 0)
                    break;

                hr = SQLColAttributes(stmt, i + 1, SQL_DESC_TYPE, NULL, 0, NULL, &types[i]);
                if (hr < 0)
                    break;

                res->setField(i, exlib::string(buf, buflen));
            }
            if (hr < 0)
                break;

            while (columns > 0) {
                hr = SQLFetch(stmt);
                if (hr < 0 || hr == SQL_NO_DATA)
                    break;

                res->beginRow();

                for (int32_t i = 0; i < columns; i++) {
                    Variant v;

                    switch (types[i]) {
                    case SQL_INTEGER:
                    case SQL_SMALLINT:
                    case SQL_TINYINT: {
                        int32_t value = 0;
                        hr = SQLGetData(stmt, i + 1, SQL_C_SLONG, &value, sizeof(value), &len);
                        if (len == SQL_NULL_DATA)
                            v.setNull();
                        else
                            v = value;
                        break;
                    }
                    case SQL_NUMERIC:
                    case SQL_DECIMAL:
                    case SQL_BIGINT:
                    case SQL_FLOAT:
                    case SQL_REAL:
                    case SQL_DOUBLE: {
                        double value;
                        hr = SQLGetData(stmt, i + 1, SQL_C_DOUBLE, &value, sizeof(value), &len);
                        if (len == SQL_NULL_DATA)
                            v.setNull();
                        else
                            v = value;
                        break;
                    }
                    case SQL_DATETIME:
                    case SQL_TIMESTAMP: {
                        TIMESTAMP_STRUCT value;
                        hr = SQLGetData(stmt, i + 1, SQL_C_TIMESTAMP, &value, sizeof(value), &len);
                        if (len == SQL_NULL_DATA)
                            v.setNull();
                        else {
                            date_t d;
                            d.create(value.year, value.month, value.day, value.hour, value.minute,
                                value.second, value.fraction / 1000000);
                            d.toUTC();
                            v = d;
                        }
                        break;
                    }
                    case SQL_BINARY:
                    case SQL_VARBINARY:
                    case SQL_LONGVARBINARY: {
                        exlib::string value;
                        hr = SQLGetData(stmt, i + 1, SQL_C_BINARY, value.c_buffer(), 0, &len);
                        if (hr < 0)
                            break;
                        if (len == SQL_NULL_DATA)
                            v.setNull();
                        else {
                            value.resize(len);
                            hr = SQLGetData(stmt, i + 1, SQL_C_BINARY, value.c_buffer(), len, &len);
                            if (hr >= 0)
                                v = new Buffer(value.c_str(), value.length());
                        }
                        break;
                    }
                    case SQL_CHAR:
                    case SQL_VARCHAR:
                    case SQL_LONGVARCHAR: {
                        exlib::string value;
                        hr = SQLGetData(stmt, i + 1, SQL_C_CHAR, value.c_buffer(), 1, &len);
                        if (hr < 0)
                            break;
                        if (len == SQL_NULL_DATA)
                            v.setNull();
                        else {
                            value.resize(len);
                            hr = SQLGetData(stmt, i + 1, SQL_C_CHAR, value.c_buffer(), len + 1, &len);
                            if (hr >= 0) {
                                if (codec == "utf8" || codec == "utf-8") {
                                    v = value;
                                } else {
                                    exlib::string value1;
                                    encoding_iconv(codec).decode(value, value1);
                                    v = value1;
                                }
                            }
                        }
                        break;
                    }
                    case SQL_WCHAR:
                    case SQL_WVARCHAR:
                    case SQL_WLONGVARCHAR:
                    case SQL_SS_VARIANT: {
                        exlib::wstring value;
                        hr = SQLGetData(stmt, i + 1, SQL_C_WCHAR, value.c_buffer(), 2, &len);
                        if (hr < 0)
                            break;
                        if (len == SQL_NULL_DATA)
                            v.setNull();
                        else {
                            value.resize(len / 2);
                            hr = SQLGetData(stmt, i + 1, SQL_C_WCHAR, value.c_buffer(), len + 2, &len);
                            if (hr >= 0)
                                v = utf16to8String(value);
                        }
                        break;
                    }
                    default: {
                        exlib::string value;
                        hr = SQLGetData(stmt, i + 1, SQL_C_BINARY, value.c_buffer(), 0, &len);
                        if (hr < 0)
                            break;
                        if (len == SQL_NULL_DATA)
                            v.setNull();
                        else {
                            value.resize(len);
                            hr = SQLGetData(stmt, i + 1, SQL_C_BINARY, value.c_buffer(), len, &len);
                            if (hr >= 0) {
                                if (codec == "utf8" || codec == "utf-8") {
                                    v = value;
                                } else {
                                    exlib::string value1;
                                    encoding_iconv(codec).decode(value, value1);
                                    v = value1;
                                }
                            }
                        }
                        break;
                    }
                    }
                    if (hr < 0)
                        break;

                    res->rowValue(i, v);
                }
                if (hr < 0)
                    break;

                res->endRow();
            }
            if (hr < 0)
                break;

            more = SQLMoreResults(stmt) == SQL_SUCCESS;

            if (!more && retVal == NULL) {
                retVal = res;
            } else {
                if (retVal == NULL)
                    retVal = new NArray();

                retVal->append(res);
            }
        } while (more);
    } while (0);

    if (hr < 0)
        hr = CHECK_ERROR(Runtime::setError(odbc_error(SQL_HANDLE_STMT, stmt)));
    else
        hr = 0;

    SQLFreeStmt(stmt, SQL_DROP);

    return hr;
}

} /* namespace fibjs */
