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
#include "ifs/URLSearchParams.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SQL_SS_VARIANT (-150)

namespace fibjs {

static exlib::string normalize_identifier(exlib::string str)
{
    // Some ODBC drivers return unquoted identifiers in ALL-UPPERCASE.
    // Normalize such ASCII identifiers to lowercase for consistency.
    bool has_alpha = false;
    bool has_lower = false;
    bool has_upper = false;
    for (size_t i = 0; i < str.length(); i++) {
        unsigned char ch = (unsigned char)str[i];
        if (ch >= 'a' && ch <= 'z') {
            has_alpha = true;
            has_lower = true;
        } else if (ch >= 'A' && ch <= 'Z') {
            has_alpha = true;
            has_upper = true;
        }
    }

    if (!has_alpha || has_lower || !has_upper)
        return str;

    for (size_t i = 0; i < str.length(); i++) {
        unsigned char ch = (unsigned char)str[i];
        if (ch >= 'A' && ch <= 'Z')
            str[i] = (char)(ch - 'A' + 'a');
    }

    return str;
}

static SQLLEN wstrlen_limited(const char16_t* str, SQLLEN maxChars)
{
    SQLLEN len = 0;
    while (len < maxChars && str[len])
        len++;
    return len;
}

void* g_odbc;

static const OdbcConnectOptions s_default_connect_options = { "Server", true, false };

result_t db_base::openOdbc(exlib::string connString, obj_ptr<DbConnection_base>& retVal,
    AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_LONGSYNC);

    if (qstrcmp(connString.c_str(), "odbc:", 5))
        return CHECK_ERROR(Runtime::setError(CALL_E_INVALIDARG, "openOdbc: connection string must start with 'odbc:'."));

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
        return CHECK_ERROR(Runtime::setError(CALL_E_INVALID_CALL, "ODBC: connection is closed."));

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

    SQLGetDiagFieldA(handleType, handle, 0, SQL_DIAG_NUMBER, &statusRecCount, SQL_IS_INTEGER, &len);
    for (int32_t i = 0; i < statusRecCount; i++) {
        char errorSQLState[14];
        char errorMessage[2048];
        SQLINTEGER native;
        hr = SQLGetDiagRecA(handleType, handle, (SQLSMALLINT)(i + 1), (SQLCHAR*)errorSQLState, &native,
            (SQLCHAR*)errorMessage, sizeof(errorMessage), &len);
        if (hr == SQL_NO_DATA || hr < 0)
            break;

        if (i > 1)
            result.append("\n    ", 5);
        result.append(errorMessage);
    }

    return result;
}

result_t odbc_set_autocommit(void* conn, bool on)
{
    SQLRETURN hr = SQLSetConnectAttrW((SQLHDBC)conn, SQL_ATTR_AUTOCOMMIT,
        (SQLPOINTER)(SQLULEN)(on ? SQL_AUTOCOMMIT_ON : SQL_AUTOCOMMIT_OFF), 0);
    if (hr < 0)
        return CHECK_ERROR(Runtime::setError(odbc_error(SQL_HANDLE_DBC, conn)));
    return 0;
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
    char* ptr = new_str.data();
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
    const char* password, const char* dbName, void*& conn, const OdbcConnectOptions* options)
{
    if (conn)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (options == NULL)
        options = &s_default_connect_options;

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

    if (*driver) {
        auto build_conn_str = [&](bool use_hostport) -> exlib::string {
            exlib::string conn_str;

            conn_str.append("Driver=");
            conn_str.append(safe_conn_string(driver));
            conn_str.append(1, ';');

            conn_str.append(options->serverAttr);
            conn_str.append(1, '=');
            conn_str.append(safe_conn_string(host));
            if (use_hostport && port > 0) {
                char str_buf[32];
                snprintf(str_buf, sizeof(str_buf), ":%d", port);
                conn_str.append(safe_conn_string(str_buf));
            }
            conn_str.append(1, ';');

            if (!use_hostport && port > 0) {
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

            if (options->trustCertificate)
                conn_str.append("TrustServerCertificate=Yes;");
            return conn_str;
        };

        auto try_connect = [&](const exlib::string& conn_str) -> SQLRETURN {
            exlib::wstring wstr(utf8to16String(conn_str));
            return SQLDriverConnectW(conn, NULL, (SQLWCHAR*)wstr.c_str(), (SQLSMALLINT)wstr.length(),
                NULL, 0, NULL, SQL_DRIVER_NOPROMPT);
        };

        // Prefer the more compact "host:port" style, but fall back to "Server=host;Port=port"
        // for drivers that do not accept host:port in the Server attribute.
        exlib::string first_err;
        exlib::string conn_str1 = build_conn_str(options->useHostPort);
        hr = try_connect(conn_str1);
        if (hr < 0) {
            first_err = odbc_error(SQL_HANDLE_DBC, conn);
            if (port > 0 && options->useHostPort) {
                exlib::string conn_str2 = build_conn_str(false);
                hr = try_connect(conn_str2);
            }
        }

        if (hr < 0) {
            exlib::string err = odbc_error(SQL_HANDLE_DBC, conn);
            if (!first_err.empty() && err != first_err) {
                err = first_err + "\n    " + err;
            }
            odbc_disconnect(conn);
            conn = NULL;
            return CHECK_ERROR(Runtime::setError(err));
        }
    }

    return 0;
}

result_t odbc_connect(exlib::string connString, const char* driver, int32_t port, void*& conn,
    const OdbcConnectOptions* options)
{
    obj_ptr<Url> u = new Url();

    result_t hr = u->parse(connString);
    if (hr < 0)
        return hr;

    obj_ptr<URLSearchParams_base> q;
    u->get_searchParams(q);
    Variant v;

    exlib::string port_ = u->port();
    if (port_.length() > 0)
        port = atoi(port_.c_str());

    exlib::string str;
    hr = q->first("Driver", v);
    if (hr == CALL_RETURN_NULL)
        hr = q->first("driver", v);

    if (hr != CALL_RETURN_NULL) {
        str = v.string();
        driver = str.c_str();
    } else if (driver == NULL)
        return CHECK_ERROR(Runtime::setError("odbc: no driver specified."));

    exlib::string pathname = u->pathname();
    
    exlib::string username;
    exlib::string password;
    Url::decodeURI(u->username(), username);
    Url::decodeURI(u->password(), password);

    return odbc_connect(driver, u->hostname().c_str(), port,
        username.c_str(), password.c_str(),
        pathname.length() > 0 ? pathname.c_str() + 1 : "", conn, options);
}

// Shared column value fetch (used by execute and Statement cursor;
// implemented later in this file)
static result_t odbc_fetchValue(SQLHSTMT stmt, int32_t col, SQLLEN type,
    Variant& v);

result_t odbc_execute(void* conn, int32_t* activeStmt, exlib::string sql, obj_ptr<NArray>& retVal, AsyncEvent* ac)
{
    if (!conn)
        return CHECK_ERROR(Runtime::setError(CALL_E_INVALID_CALL, "ODBC: connection is closed."));

    if (activeStmt && *activeStmt)
        return db_stmt_busy_error();

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_LONGSYNC);

    SQLRETURN hr;
    SQLHSTMT stmt;

    hr = SQLAllocStmt(conn, &stmt);
    if (hr < 0)
        return CHECK_ERROR(Runtime::setError(odbc_error(SQL_HANDLE_DBC, conn)));

    do {
        bool more = false;

        exlib::wstring wsql(utf8to16String(sql));
        hr = SQLExecDirectW(stmt, (SQLWCHAR*)wsql.c_str(), (SQLINTEGER)wsql.length());
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
                SQLSMALLINT buflen;
                SQLWCHAR buf[SQL_MAX_COLUMN_NAME_LEN];
                hr = SQLColAttributeW(stmt, i + 1, SQL_DESC_NAME, buf, SQL_MAX_COLUMN_NAME_LEN * sizeof(SQLWCHAR), &buflen, NULL);
                if (hr < 0)
                    break;

                exlib::string fieldName = utf16to8String((const char16_t*)buf, buflen / sizeof(SQLWCHAR));
                res->setField(i, normalize_identifier(fieldName));

                hr = SQLColAttributeW(stmt, i + 1, SQL_DESC_TYPE, NULL, 0, NULL, &types[i]);
                if (hr < 0)
                    break;

                if (types[i] == SQL_VARCHAR) {
                    SQLSMALLINT buflen;
                    SQLWCHAR buf[SQL_MAX_COLUMN_NAME_LEN];

                    hr = SQLColAttributeW(stmt, i + 1, SQL_DESC_TYPE_NAME, buf,
                        SQL_MAX_COLUMN_NAME_LEN * sizeof(SQLWCHAR), &buflen, NULL);
                    if (hr < 0)
                        break;

                    exlib::string typeName = utf16to8String((const char16_t*)buf, buflen / sizeof(SQLWCHAR));
                    if (typeName == "boolean" || typeName == "bool")
                        types[i] = SQL_BIT;
                }
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

                    hr = odbc_fetchValue(stmt, i + 1, types[i], v);
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

// ---------------------------------------------------------------------------
// ODBC Statement cursor implementation (SQLPrepare + row-by-row SQLFetch)
// ---------------------------------------------------------------------------

// Fetch one column value by type (shared by execute and Statement cursor)
static result_t odbc_fetchValue(SQLHSTMT stmt, int32_t col, SQLLEN type,
    Variant& v)
{
    SQLRETURN hr;
    SQLLEN len;

    switch (type) {
    case SQL_INTEGER:
    case SQL_SMALLINT:
    case SQL_TINYINT: {
        int32_t value = 0;
        hr = SQLGetData(stmt, col, SQL_C_SLONG, &value, sizeof(value), &len);
        if (hr < 0)
            return CHECK_ERROR(Runtime::setError(odbc_error(SQL_HANDLE_STMT, stmt)));
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
        hr = SQLGetData(stmt, col, SQL_C_DOUBLE, &value, sizeof(value), &len);
        if (hr < 0)
            return CHECK_ERROR(Runtime::setError(odbc_error(SQL_HANDLE_STMT, stmt)));
        if (len == SQL_NULL_DATA)
            v.setNull();
        else
            v = value;
        break;
    }
    case SQL_BIT: {
        char value;
        hr = SQLGetData(stmt, col, SQL_C_BIT, &value, sizeof(value), &len);
        if (hr < 0)
            return CHECK_ERROR(Runtime::setError(odbc_error(SQL_HANDLE_STMT, stmt)));
        if (len == SQL_NULL_DATA)
            v.setNull();
        else
            v = value != 0;
        break;
    }
    case SQL_DATETIME:
    case SQL_TIMESTAMP: {
        TIMESTAMP_STRUCT value;
        hr = SQLGetData(stmt, col, SQL_C_TIMESTAMP, &value, sizeof(value), &len);
        if (hr < 0)
            return CHECK_ERROR(Runtime::setError(odbc_error(SQL_HANDLE_STMT, stmt)));
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
        hr = SQLGetData(stmt, col, SQL_C_BINARY, value.data(), 0, &len);
        if (hr < 0)
            return CHECK_ERROR(Runtime::setError(odbc_error(SQL_HANDLE_STMT, stmt)));
        if (len == SQL_NULL_DATA)
            v.setNull();
        else {
            value.resize(len);
            hr = SQLGetData(stmt, col, SQL_C_BINARY, value.data(), len, &len);
            if (hr < 0)
                return CHECK_ERROR(Runtime::setError(odbc_error(SQL_HANDLE_STMT, stmt)));
            v = new Buffer(value.c_str(), value.length());
        }
        break;
    }
    default: {
        // Large text fetched in 4096-character chunks
        exlib::wstring value;
        exlib::wstring chunk;
        const SQLLEN chunkChars = 4096;
        SQLLEN valueChars = 0;

        chunk.resize(chunkChars + 1);
        do {
            hr = SQLGetData(stmt, col, SQL_C_WCHAR, chunk.data(),
                (chunkChars + 1) * sizeof(SQLWCHAR), &len);
            if (hr < 0)
                return CHECK_ERROR(Runtime::setError(odbc_error(SQL_HANDLE_STMT, stmt)));
            if (len == SQL_NULL_DATA) {
                v.setNull();
                break;
            }

            SQLLEN copiedChars = len == SQL_NO_TOTAL
                ? wstrlen_limited(chunk.data(), chunkChars)
                : (hr == SQL_SUCCESS_WITH_INFO ? chunkChars : len / sizeof(SQLWCHAR));
            if (copiedChars > chunkChars)
                copiedChars = chunkChars;

            if (copiedChars > 0) {
                SQLLEN oldChars = valueChars;
                valueChars += copiedChars;
                value.resize(valueChars);
                memcpy(value.data() + oldChars, chunk.data(), copiedChars * sizeof(SQLWCHAR));
            }
        } while (hr == SQL_SUCCESS_WITH_INFO);

        if (len != SQL_NULL_DATA)
            v = utf16to8String(value);
        break;
    }
    }

    return 0;
}

// Engine escape callbacks: Variant → SQL literal (each engine has different
// escape rules). OdbcEscape is defined in Odbc.h

// Generic escaping (SQL standard: ' → ''; binary → 0x hex)
static exlib::string odbcEscapeString(exlib::string v)
{
    exlib::string retVal;
    const char* str = v.c_str();
    int32_t sz = (int32_t)v.length();

    retVal.append(1, '\'');
    for (int32_t i = 0; i < sz; i++) {
        if (str[i] == '\'')
            retVal.append(1, '\'');
        retVal.append(1, str[i]);
    }
    retVal.append(1, '\'');
    return retVal;
}

static exlib::string odbcEscapeBinary(Buffer* bin)
{
    exlib::string retVal;
    exlib::string s;

    bin->hex(s);
    retVal.append("0x", 2);
    retVal.append(s);
    return retVal;
}

// Assemble Variant args into SQL using literal semantics (aligned with
// db_format)
static void odbcFormatValue(exlib::string& str, Variant& v,
    const OdbcEscape& esc)
{
    switch (v.type()) {
    case Variant::VT_Null:
    case Variant::VT_Undefined:
        str.append("NULL", 4);
        break;

    case Variant::VT_Boolean:
        str.append(v.boolVal() ? "1" : "0", 1);
        break;

    case Variant::VT_Integer:
    case Variant::VT_Long:
    case Variant::VT_Number: {
        exlib::string s;
        v.toString(s);
        str.append(s);
        break;
    }

    case Variant::VT_Object: {
        object_base* obj = v.object();
        if (obj && obj->class_info().isInstance(Buffer_base::class_info())) {
            obj_ptr<Buffer> buf = (Buffer*)obj;
            str.append(esc.binary(buf));
        } else {
            exlib::string s;
            v.toString(s);
            str.append(esc.string(s));
        }
        break;
    }

    default: {
        exlib::string s;
        v.toString(s);
        str.append(esc.string(s));
        break;
    }
    }
}

static result_t odbcFormatSQL(const char* sql, std::vector<Variant>& args,
    const OdbcEscape& esc, exlib::string& retVal)
{
    exlib::string str;
    const char *p, *p1;
    size_t cnt = 0;

    while (*sql) {
        p = p1 = sql;
        while (*p1 && *p1 != '?')
            p1++;

        str.append(p, p1 - p);

        if (*p1) {
            p1++;

            if (cnt < args.size())
                odbcFormatValue(str, args[cnt], esc);
            else
                str.append(1, '?');

            cnt++;
        }

        sql = p1;
    }

    retVal = str;
    return 0;
}

class OdbcStmtImpl : public Statement::impl {
public:
    OdbcStmtImpl(void* conn, int32_t* activeStmt, exlib::string sql,
        const OdbcEscape& esc)
        : m_conn(conn)
        , m_activeStmt(activeStmt)
        , m_sql(sql)
        , m_esc(esc)
        , m_stmt(NULL)
        , m_columns(0)
    {
    }

    virtual ~OdbcStmtImpl()
    {
        close();
    }

    virtual result_t open(std::vector<Variant>& args, bool& hasResult)
    {
        reset(); // defensive: previous cursor was not released

        exlib::string full;
        result_t hr = odbcFormatSQL(m_sql.c_str(), args, m_esc, full);
        if (hr < 0)
            return hr;

        SQLRETURN sqlhr = SQLAllocStmt(m_conn, &m_stmt);
        if (sqlhr < 0)
            return CHECK_ERROR(Runtime::setError(odbc_error(SQL_HANDLE_DBC, m_conn)));

        exlib::wstring wsql(utf8to16String(full));
        sqlhr = SQLPrepareW(m_stmt, (SQLWCHAR*)wsql.c_str(),
            (SQLINTEGER)wsql.length());
        if (sqlhr < 0) {
            exlib::string err = odbc_error(SQL_HANDLE_STMT, m_stmt);
            SQLFreeStmt(m_stmt, SQL_DROP);
            m_stmt = NULL;
            return CHECK_ERROR(Runtime::setError(err));
        }

        sqlhr = SQLExecute(m_stmt);
        if (sqlhr < 0) {
            exlib::string err = odbc_error(SQL_HANDLE_STMT, m_stmt);
            SQLFreeStmt(m_stmt, SQL_DROP);
            m_stmt = NULL;
            return CHECK_ERROR(Runtime::setError(err));
        }

        // Column metadata (available after execution)
        SQLSMALLINT columns = 0;
        sqlhr = SQLNumResultCols(m_stmt, &columns);
        if (sqlhr < 0) {
            exlib::string err = odbc_error(SQL_HANDLE_STMT, m_stmt);
            SQLFreeStmt(m_stmt, SQL_DROP);
            m_stmt = NULL;
            return CHECK_ERROR(Runtime::setError(err));
        }

        m_columns = columns;
        m_types.resize(m_columns);
        m_names.resize(m_columns);

        for (int32_t i = 0; i < m_columns; i++) {
            SQLSMALLINT buflen;
            SQLWCHAR buf[SQL_MAX_COLUMN_NAME_LEN];
            sqlhr = SQLColAttributeW(m_stmt, i + 1, SQL_DESC_NAME, buf,
                SQL_MAX_COLUMN_NAME_LEN * sizeof(SQLWCHAR), &buflen, NULL);
            if (sqlhr < 0) {
                exlib::string err = odbc_error(SQL_HANDLE_STMT, m_stmt);
                SQLFreeStmt(m_stmt, SQL_DROP);
                m_stmt = NULL;
                return CHECK_ERROR(Runtime::setError(err));
            }

            m_names[i] = normalize_identifier(
                utf16to8String((const char16_t*)buf, buflen / sizeof(SQLWCHAR)));

            sqlhr = SQLColAttributeW(m_stmt, i + 1, SQL_DESC_TYPE, NULL, 0,
                NULL, &m_types[i]);
            if (sqlhr < 0) {
                exlib::string err = odbc_error(SQL_HANDLE_STMT, m_stmt);
                SQLFreeStmt(m_stmt, SQL_DROP);
                m_stmt = NULL;
                return CHECK_ERROR(Runtime::setError(err));
            }

            if (m_types[i] == SQL_VARCHAR) {
                SQLSMALLINT blen;
                SQLWCHAR bbuf[SQL_MAX_COLUMN_NAME_LEN];
                sqlhr = SQLColAttributeW(m_stmt, i + 1, SQL_DESC_TYPE_NAME,
                    bbuf, SQL_MAX_COLUMN_NAME_LEN * sizeof(SQLWCHAR), &blen, NULL);
                if (sqlhr >= 0) {
                    exlib::string typeName = utf16to8String(
                        (const char16_t*)bbuf, blen / sizeof(SQLWCHAR));
                    if (typeName == "boolean" || typeName == "bool")
                        m_types[i] = SQL_BIT;
                }
            }
        }

        hasResult = (m_columns > 0);
        if (m_activeStmt)
            *m_activeStmt = 1;
        return 0;
    }

    virtual result_t fetchRow(NObject* row, bool& done)
    {
        if (!m_stmt)
            return CHECK_ERROR(CALL_E_CLOSED);

        SQLRETURN hr = SQLFetch(m_stmt);
        if (hr < 0)
            return CHECK_ERROR(Runtime::setError(odbc_error(SQL_HANDLE_STMT, m_stmt)));
        if (hr == SQL_NO_DATA) {
            done = true;
            return 0;
        }

        for (int32_t i = 0; i < m_columns; i++) {
            Variant v;
            result_t hr2 = odbc_fetchValue(m_stmt, i + 1, m_types[i], v);
            if (hr2 < 0)
                return hr2;
            row->add(m_names[i], v);
        }
        return 0;
    }

    virtual result_t runResult(int64_t& changes, int64_t& lastInsertId)
    {
        if (!m_stmt)
            return CHECK_ERROR(CALL_E_CLOSED);

        SQLLEN affected = 0;
        SQLRowCount(m_stmt, &affected);
        changes = (int64_t)affected;
        return 0;
    }

    virtual result_t columns(obj_ptr<NArray>& retVal)
    {
        return CHECK_ERROR(Runtime::setError(CALL_E_INVALID_CALL,
            "ODBC: columns() requires a prepared statement (v2)"));
    }

    // Cursor reset: release the HSTMT (connection stays usable), impl is
    // retained for reuse
    virtual void reset()
    {
        if (m_stmt) {
            SQLFreeStmt(m_stmt, SQL_CLOSE);
            SQLFreeStmt(m_stmt, SQL_DROP);
            m_stmt = NULL;
        }
        m_columns = 0;
        m_types.clear();
        m_names.clear();
        if (m_activeStmt)
            *m_activeStmt = 0;
    }

    // Full release (when the Statement is destroyed)
    virtual void close()
    {
        reset();
    }

private:
    void* m_conn;
    int32_t* m_activeStmt;
    exlib::string m_sql;
    OdbcEscape m_esc;
    SQLHSTMT m_stmt;
    SQLSMALLINT m_columns;
    std::vector<SQLLEN> m_types;
    std::vector<exlib::string> m_names;
};

// Generic escape callbacks (odbc engine default)
static const OdbcEscape odbcEscape = { odbcEscapeString, odbcEscapeBinary };

result_t odbc_prepareStmt(void* conn, int32_t* activeStmt, exlib::string sql,
    const OdbcEscape& esc, obj_ptr<Statement_base>& retVal)
{
    if (!conn)
        return CHECK_ERROR(Runtime::setError(CALL_E_INVALID_CALL,
            "ODBC: database is closed."));

    obj_ptr<Statement> stmt = new Statement(sql,
        new OdbcStmtImpl(conn, activeStmt, sql, esc));
    retVal = stmt;
    return 0;
}

result_t odbc_getTables(void* conn, obj_ptr<NArray>& retVal, AsyncEvent* ac)
{
    if (!conn)
        return CHECK_ERROR(Runtime::setError(CALL_E_INVALID_CALL, "ODBC: connection is closed."));

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_LONGSYNC);

    SQLRETURN hr;
    SQLHSTMT stmt;

    hr = SQLAllocStmt(conn, &stmt);
    if (hr < 0)
        return CHECK_ERROR(Runtime::setError(odbc_error(SQL_HANDLE_DBC, conn)));

    // Use ODBC SQLTables to get table list - more compatible across different databases
    hr = SQLTablesA(stmt, NULL, 0, NULL, 0, NULL, 0, (SQLCHAR*)"TABLE", SQL_NTS);
    if (hr < 0) {
        SQLFreeStmt(stmt, SQL_DROP);
        return CHECK_ERROR(Runtime::setError(odbc_error(SQL_HANDLE_STMT, stmt)));
    }

    obj_ptr<DBResult> res = new DBResult(1);
    res->setField(0, "name"); // Match SQLite format with 'name' field

    while (true) {
        hr = SQLFetch(stmt);
        if (hr == SQL_NO_DATA)
            break;
        if (hr < 0) {
            SQLFreeStmt(stmt, SQL_DROP);
            return CHECK_ERROR(Runtime::setError(odbc_error(SQL_HANDLE_STMT, stmt)));
        }

        SQLLEN len;
        char tableName[256];
        hr = SQLGetData(stmt, 3, SQL_C_CHAR, tableName, sizeof(tableName), &len); // Column 3 is TABLE_NAME
        if (hr >= 0) {
            res->beginRow();
            exlib::string name = exlib::string(tableName, len == SQL_NULL_DATA ? 0 : len);
            Variant v = normalize_identifier(name);
            res->rowValue(0, v);
            res->endRow();
        }
    }

    SQLFreeStmt(stmt, SQL_DROP);
    retVal = res;
    return 0;
}

result_t odbc_getTableInfo(void* conn, exlib::string tableName, obj_ptr<NArray>& retVal, AsyncEvent* ac)
{
    if (!conn)
        return CHECK_ERROR(Runtime::setError(CALL_E_INVALID_CALL, "ODBC: connection is closed."));

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_LONGSYNC);

    SQLRETURN hr;
    SQLHSTMT stmt;

    hr = SQLAllocStmt(conn, &stmt);
    if (hr < 0)
        return CHECK_ERROR(Runtime::setError(odbc_error(SQL_HANDLE_DBC, conn)));

    obj_ptr<DBResult> res = new DBResult(5);
    res->setField(0, "column_name");
    res->setField(1, "data_type");
    res->setField(2, "character_maximum_length");
    res->setField(3, "is_nullable");
    res->setField(4, "column_default");

    auto fetchColumns = [&](const exlib::string& tn, int32_t& rows) -> result_t {
        SQLRETURN hr2;

        SQLFreeStmt(stmt, SQL_CLOSE);
        hr2 = SQLColumnsA(stmt, NULL, 0, NULL, 0, (SQLCHAR*)tn.c_str(), SQL_NTS, NULL, 0);
        if (hr2 < 0)
            return CHECK_ERROR(Runtime::setError(odbc_error(SQL_HANDLE_STMT, stmt)));

        while (true) {
            hr2 = SQLFetch(stmt);
            if (hr2 == SQL_NO_DATA)
                break;
            if (hr2 < 0)
                return CHECK_ERROR(Runtime::setError(odbc_error(SQL_HANDLE_STMT, stmt)));

            res->beginRow();

            // Column 4: COLUMN_NAME
            SQLLEN len;
            char columnName[256];
            hr2 = SQLGetData(stmt, 4, SQL_C_CHAR, columnName, sizeof(columnName), &len);
            exlib::string col = (hr2 >= 0 && len != SQL_NULL_DATA) ? exlib::string(columnName, len) : exlib::string("");
            Variant v1 = normalize_identifier(col);
            res->rowValue(0, v1);

            // Column 6: TYPE_NAME
            char typeName[256];
            hr2 = SQLGetData(stmt, 6, SQL_C_CHAR, typeName, sizeof(typeName), &len);
            Variant v2 = (hr2 >= 0 && len != SQL_NULL_DATA) ? exlib::string(typeName, len) : exlib::string("");
            res->rowValue(1, v2);

            // Column 7: COLUMN_SIZE
            int32_t columnSize;
            hr2 = SQLGetData(stmt, 7, SQL_C_LONG, &columnSize, sizeof(columnSize), &len);
            Variant v3;
            if (hr2 >= 0 && len != SQL_NULL_DATA)
                v3 = columnSize;
            else
                v3.setNull();
            res->rowValue(2, v3);

            // Column 11: NULLABLE
            int32_t nullable;
            hr2 = SQLGetData(stmt, 11, SQL_C_LONG, &nullable, sizeof(nullable), &len);
            Variant v4 = (hr2 >= 0 && len != SQL_NULL_DATA && nullable == SQL_NULLABLE) ? exlib::string("YES") : exlib::string("NO");
            res->rowValue(3, v4);

            // Column 13: COLUMN_DEF
            char columnDefault[256];
            hr2 = SQLGetData(stmt, 13, SQL_C_CHAR, columnDefault, sizeof(columnDefault), &len);
            Variant v5;
            if (hr2 >= 0 && len != SQL_NULL_DATA)
                v5 = exlib::string(columnDefault, len);
            else
                v5.setNull();
            res->rowValue(4, v5);

            res->endRow();
            rows++;
        }

        return 0;
    };

    int32_t rows = 0;
    result_t r = fetchColumns(tableName, rows);
    if (r < 0) {
        SQLFreeStmt(stmt, SQL_DROP);
        return r;
    }

    // Retry with uppercased table name when metadata is stored in uppercase (e.g. DM)
    if (rows == 0 && tableName.length() > 0) {
        bool has_lower = false;
        bool has_upper = false;
        for (size_t i = 0; i < tableName.length(); i++) {
            unsigned char ch = (unsigned char)tableName[i];
            if (ch >= 'a' && ch <= 'z')
                has_lower = true;
            else if (ch >= 'A' && ch <= 'Z')
                has_upper = true;
        }

        if (has_lower && !has_upper) {
            exlib::string tn = tableName;
            for (size_t i = 0; i < tn.length(); i++) {
                unsigned char ch = (unsigned char)tn[i];
                if (ch >= 'a' && ch <= 'z')
                    tn[i] = (char)(ch - 'a' + 'A');
            }

            r = fetchColumns(tn, rows);
            if (r < 0) {
                SQLFreeStmt(stmt, SQL_DROP);
                return r;
            }
        }
    }

    SQLFreeStmt(stmt, SQL_DROP);
    retVal = res;
    return 0;
}

} /* namespace fibjs */
