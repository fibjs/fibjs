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
#include "../db_api.h"
#include <stdio.h>
#include <stdlib.h>

namespace fibjs {

result_t db_base::openOdbc(exlib::string connString, obj_ptr<Odbc_base>& retVal,
    AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_LONGSYNC);

    if (qstrcmp(connString.c_str(), "odbc:", 5))
        return CHECK_ERROR(CALL_E_INVALIDARG);

    obj_ptr<Url> u = new Url();

    result_t hr = u->parse(connString);
    if (hr < 0)
        return hr;

    obj_ptr<HttpCollection_base> q;
    u->get_searchParams(q);
    Variant v;

    int32_t nPort = -1;
    if (u->m_port.length() > 0)
        nPort = atoi(u->m_port.c_str());

    hr = q->first("Driver", v);
    if (hr == CALL_RETURN_NULL)
        return CHECK_ERROR(Runtime::setError("odbc: no driver or dsn specified."));

    exlib::string driver = v.string();
    obj_ptr<Odbc> conn = new Odbc();

    hr = conn->connect(driver.c_str(), u->m_hostname.c_str(), nPort,
        u->m_username.c_str(), u->m_password.c_str(),
        u->m_pathname.length() > 0 ? u->m_pathname.c_str() + 1 : "");
    if (hr < 0)
        return hr;

    retVal = conn;
    return 0;
}

Odbc::~Odbc()
{
    close();
}

void Odbc::close()
{
    if (m_conn) {
        SQLFreeConnect(m_conn);
        m_conn = NULL;
    }

    if (m_env) {
        SQLFreeEnv(m_env);
        m_env = NULL;
    }
}

exlib::string Odbc::GetSQLError(int32_t handleType, void* handle)
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

    while (ch = str[len]) {
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
    while (ch = str[len]) {
        *ptr++ = ch;
        if (ch == '}')
            *ptr++ = ch;

        len++;
    }
    *ptr++ = '}';

    return new_str;
}

result_t Odbc::connect(const char* driver, const char* host, int32_t port, const char* username,
    const char* password, const char* dbName)
{
    if (m_conn)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    SQLRETURN hr;
    hr = SQLAllocEnv((SQLHENV*)&m_env);
    if (hr < 0)
        return CHECK_ERROR(Runtime::setError("odbc: unable to allocate environment."));

    hr = SQLAllocConnect(m_env, (SQLHDBC*)&m_conn);
    if (hr < 0) {
        close();
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

            sprintf(str_buf, "%d", port);
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

    hr = SQLDriverConnect(m_conn, NULL, (SQLCHAR*)conn_str.c_str(), conn_str.length(),
        NULL, 0, NULL, SQL_DRIVER_NOPROMPT);
    if (hr < 0) {
        exlib::string err = GetSQLError(SQL_HANDLE_DBC, m_conn);
        close();
        return CHECK_ERROR(Runtime::setError(err));
    }

    return 0;
}

result_t Odbc::get_type(exlib::string& retVal)
{
    retVal = "odbc";
    return 0;
}

result_t Odbc::close(AsyncEvent* ac)
{
    if (!m_conn)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_LONGSYNC);

    close();

    return 0;
}

result_t Odbc::use(exlib::string dbName, AsyncEvent* ac)
{
    if (!m_conn)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_LONGSYNC);

    obj_ptr<NArray> retVal;
    exlib::string s("USE ", 4);
    s.append(dbName);
    return execute(s.c_str(), (int32_t)s.length(), retVal);
}

result_t Odbc::execute(const char* sql, int32_t sLen, obj_ptr<NArray>& retVal)
{
    if (!m_conn)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    SQLRETURN hr;
    SQLHSTMT stmt;

    hr = SQLAllocStmt(m_conn, &stmt);
    if (hr < 0)
        return CHECK_ERROR(Runtime::setError(GetSQLError(SQL_HANDLE_DBC, m_conn)));

    do {
        obj_ptr<DBResult> res;
        std::vector<SQLLEN> types;
        SQLLEN len;
        SQLSMALLINT columns = 0;
        SQLLEN affected;

        hr = SQLExecDirect(stmt, (SQLCHAR*)sql, sLen);
        if (hr < 0)
            break;

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

            hr = SQLColAttribute(stmt, i + 1, SQL_DESC_NAME, buf, SQL_MAX_COLUMN_NAME_LEN, &buflen, NULL);
            if (hr < 0)
                break;

            hr = SQLColAttribute(stmt, i + 1, SQL_DESC_TYPE, NULL, 0, NULL, &types[i]);
            if (hr < 0)
                break;

            res->setField(i, exlib::string(buf, buflen));
        }
        if (hr < 0)
            break;

        while (true) {
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
                    v = value;
                    break;
                }
                case SQL_DATETIME:
                case SQL_TIMESTAMP: {
                    TIMESTAMP_STRUCT value;
                    date_t d;
                    hr = SQLGetData(stmt, i + 1, SQL_C_TIMESTAMP, &value, sizeof(value), &len);
                    d.create(value.year, value.month, value.day, value.hour, value.minute,
                        value.second, value.fraction / 1000000);
                    d.toUTC();
                    v = d;
                    break;
                }
                case SQL_BINARY:
                case SQL_VARBINARY:
                case SQL_LONGVARBINARY: {
                    exlib::string value;
                    hr = SQLGetData(stmt, i + 1, SQL_C_BINARY, value.c_buffer(), 0, &len);
                    if (hr < 0)
                        break;
                    value.resize(len);
                    hr = SQLGetData(stmt, i + 1, SQL_C_BINARY, value.c_buffer(), len, &len);
                    if (hr >= 0)
                        v = new Buffer(value);
                    break;
                }
                default: {
                    exlib::string value;
                    hr = SQLGetData(stmt, i + 1, SQL_C_TCHAR, value.c_buffer(), 0, &len);
                    if (hr < 0)
                        break;
                    value.resize(len);
                    hr = SQLGetData(stmt, i + 1, SQL_C_TCHAR, value.c_buffer(), len + 1, &len);
                    if (hr >= 0)
                        v = value;
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

        retVal = res;
    } while (0);

    if (hr < 0)
        hr = CHECK_ERROR(Runtime::setError(GetSQLError(SQL_HANDLE_STMT, stmt)));
    else
        hr = 0;

    SQLFreeStmt(stmt, SQL_CLOSE);

    return hr;
}

result_t Odbc::begin(exlib::string point, AsyncEvent* ac)
{
    return db_begin(this, point, ac, true);
}

result_t Odbc::commit(exlib::string point, AsyncEvent* ac)
{
    return db_commit(this, point, ac, true);
}

result_t Odbc::rollback(exlib::string point, AsyncEvent* ac)
{
    return db_rollback(this, point, ac, true);
}

result_t Odbc::trans(v8::Local<v8::Function> func, bool& retVal)
{
    return trans("", func, retVal);
}

result_t Odbc::trans(exlib::string point, v8::Local<v8::Function> func, bool& retVal)
{
    return db_trans(this, point, func, retVal);
}

result_t Odbc::execute(exlib::string sql, OptArgs args, obj_ptr<NArray>& retVal,
    AsyncEvent* ac)
{
    return db_execute(this, sql, args, retVal, ac);
}

result_t Odbc::createTable(v8::Local<v8::Object> opts, AsyncEvent* ac)
{
    return db_createTable(this, opts, ac);
}

result_t Odbc::dropTable(v8::Local<v8::Object> opts, AsyncEvent* ac)
{
    return db_dropTable(this, opts, ac);
}

result_t Odbc::createIndex(v8::Local<v8::Object> opts, AsyncEvent* ac)
{
    return db_createIndex(this, opts, ac);
}

result_t Odbc::dropIndex(v8::Local<v8::Object> opts, AsyncEvent* ac)
{
    return db_dropIndex(this, opts, ac);
}

result_t Odbc::insert(v8::Local<v8::Object> opts, double& retVal, AsyncEvent* ac)
{
    return db_insert(this, opts, retVal, ac);
}

result_t Odbc::find(v8::Local<v8::Object> opts, obj_ptr<NArray>& retVal, AsyncEvent* ac)
{
    return db_find(this, opts, retVal, ac);
}

result_t Odbc::count(v8::Local<v8::Object> opts, int32_t& retVal, AsyncEvent* ac)
{
    return db_count(this, opts, retVal, ac);
}

result_t Odbc::update(v8::Local<v8::Object> opts, int32_t& retVal, AsyncEvent* ac)
{
    return db_update(this, opts, retVal, ac);
}

result_t Odbc::remove(v8::Local<v8::Object> opts, int32_t& retVal, AsyncEvent* ac)
{
    return db_remove(this, opts, retVal, ac);
}

result_t Odbc::format(exlib::string method, v8::Local<v8::Object> opts, exlib::string& retVal)
{
    return db_base::format(method, opts, retVal);
}

result_t Odbc::format(exlib::string sql, OptArgs args, exlib::string& retVal)
{
    return db_base::format(sql, args, retVal);
}

} /* namespace fibjs */
