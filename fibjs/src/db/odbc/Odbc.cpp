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
#include "errhelp.h"

#define SQL_SS_VARIANT (-150)

namespace fibjs {

// The handle has been released: one helper so the call sites cannot drift
// apart, and so the code string is stated once.
static result_t db_closed()
{
    return errhelp::closed("ERR_INVALID_STATE", "ODBC: connection is closed.");
}

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
    AsyncHandle ac)
{
    if (ac.isSync())
        return result_t::longsync();

    if (qstrcmp(connString.c_str(), "odbc:", 5))
        return result_t::make(CALL_E_INVALID_DATA).with_type_name(errtype::kTypeError).with_code("ERR_INVALID_ARG_VALUE").with_message("openOdbc: connection string must start with 'odbc:'.").with_result(result_t::invalid_arg());

    obj_ptr<Odbc> conn = new Odbc();

    result_t hr = odbc_connect(connString, NULL, -1, conn->m_conn);
    if (hr.failed())
        return hr;

    retVal = conn;
    return result_t();
}

result_t odbc_disconnect(void* conn)
{
    if (conn) {
        SQLDisconnect(conn);
        SQLFreeConnect(conn);
    }

    return result_t();
}

result_t odbc_close(void*& conn, AsyncHandle ac)
{
    if (ac.isSync())
        return result_t::longsync();

    if (!conn)
        return db_closed();

    odbc_disconnect(conn);
    conn = NULL;

    return result_t();
}

struct OdbcDiagInfo {
    exlib::string message;
    exlib::string sqlstate;
    bool has_native = false;
    int32_t native = 0;
};

static OdbcDiagInfo odbc_error_info(int32_t handleType, void* handle)
{
    SQLRETURN hr;
    SQLSMALLINT len;
    SQLINTEGER statusRecCount = 0;
    OdbcDiagInfo info;

    // Without diagnostics there is nothing to describe: do not read an
    // uninitialized record count when the call itself fails.
    if (SQLGetDiagFieldA(handleType, handle, 0, SQL_DIAG_NUMBER, &statusRecCount, SQL_IS_INTEGER, &len) < 0)
        return info;

    for (int32_t i = 0; i < statusRecCount; i++) {
        char errorSQLState[14];
        char errorMessage[2048];
        SQLINTEGER native;
        hr = SQLGetDiagRecA(handleType, handle, (SQLSMALLINT)(i + 1), (SQLCHAR*)errorSQLState, &native,
            (SQLCHAR*)errorMessage, sizeof(errorMessage), &len);
        if (hr == SQL_NO_DATA || hr < 0)
            break;

        if (info.sqlstate.empty() || (info.sqlstate == "S1000" && strcmp(errorSQLState, "S1000") != 0))
            info.sqlstate = errorSQLState;

        if (!info.has_native && native != 0) {
            info.has_native = true;
            info.native = native;
        }

        if (!info.message.empty())
            info.message.append("\n    ", 5);
        info.message.append(errorMessage);
    }

    return info;
}

exlib::string odbc_error(int32_t handleType, void* handle)
{
    return odbc_error_info(handleType, handle).message;
}

static result_t odbc_set_error(int32_t handleType, void* handle)
{
    OdbcDiagInfo info = odbc_error_info(handleType, handle);

    result_t::builder b = result_t::make(CALL_E_INVALID_CALL);

    if (!info.sqlstate.empty())
        b = std::move(b).with_code(info.sqlstate);
    if (info.has_native)
        b = std::move(b).with_errno(info.native);

    // Keep a useful description when the driver exposes no record: an empty
    // payload message would let the JS boundary fall back to a bare number.
    if (info.message.empty())
        info.message = "odbc: the driver reported a failure without a diagnostic message.";

    return result_t(std::move(b).with_message(info.message));
}

result_t odbc_set_autocommit(void* conn, bool on)
{
    SQLRETURN hr = SQLSetConnectAttrW((SQLHDBC)conn, SQL_ATTR_AUTOCOMMIT,
        (SQLPOINTER)(SQLULEN)(on ? SQL_AUTOCOMMIT_ON : SQL_AUTOCOMMIT_OFF), 0);
    if (hr < 0)
        return odbc_set_error(SQL_HANDLE_DBC, conn);
    return result_t();
}

// The ODBC literal form: the value in braces, every '}' doubled.
static exlib::string quote_odbc_value(const char* str, int32_t len, int32_t esc_cnt)
{
    exlib::string new_str;
    new_str.resize(len + esc_cnt + 2);
    char* ptr = new_str.data();

    *ptr++ = '{';
    for (int32_t i = 0; i < len; i++) {
        *ptr++ = str[i];
        if (str[i] == '}')
            *ptr++ = '}';
    }
    *ptr++ = '}';

    return new_str;
}

// Quote a value the ODBC grammar would otherwise split or read as syntax: the
// ';' delimiter, a brace inside the value, and a value that starts with '{'
// (which is otherwise the opening of the bracket syntax).
exlib::string safe_conn_string(const char* str)
{
    bool need_quote = false;
    int32_t esc_cnt = 0;
    int32_t len = 0;

    while (str[len]) {
        if (str[len] == '}')
            esc_cnt++;
        else if (str[len] == ';')
            need_quote = true;
        len++;
    }

    if (len > 0 && str[0] == '{')
        need_quote = true;

    if (!need_quote)
        return str;

    return quote_odbc_value(str, len, esc_cnt);
}

// psqlODBC decodes Pwd as a form value (`+` -> space, `%XX` -> byte) unless
// the value is brace-quoted (dlg_specific.c: decode_or_remove_braces). The
// URL layer already decoded the credential once, so the password has to
// reach the driver byte for byte. Braces are the ODBC literal form and the
// drivers remove them (psqlODBC for Pwd, FreeTDS for every keyword); quoting
// also protects the attribute delimiter and leading/trailing whitespace.
static exlib::string safe_password(const char* str)
{
    bool need_quote = false;
    int32_t esc_cnt = 0;
    int32_t len = 0;

    while (str[len]) {
        char ch = str[len];

        if (ch == '}') {
            esc_cnt++;
            need_quote = true;
        } else if (ch == ';' || ch == '{' || ch == '%' || ch == '+')
            need_quote = true;
        len++;
    }

    if (len > 0 && (qisspace((unsigned char)str[0]) || qisspace((unsigned char)str[len - 1])))
        need_quote = true;

    if (!need_quote)
        return str;

    return quote_odbc_value(str, len, esc_cnt);
}

result_t odbc_connect(const char* driver, const char* host, int32_t port, const char* username,
    const char* password, const char* dbName, void*& conn, const OdbcConnectOptions* options)
{
    if (conn)
        return result_t::invalid_call();

    if (options == NULL)
        options = &s_default_connect_options;

    // The connection parameters of a failed attempt: psql / mssql / dm / odbc
    // all share this entry, and the failing layer builds them into its own
    // value (there is no upper-layer merge). The password never enters the
    // payload, not even as a truncated summary.
    auto diag = [&](result_t::builder&& b) -> result_t {
        return result_t(std::move(b)
                .arg("host", host)
                .arg("port", port)
                .arg("database", dbName)
                .arg("user", username)
                .arg_redacted("password"));
    };

    // psqlODBC keeps the braces in Uid / Database / Servername, so the usual
    // ODBC escape for ';' does not work there: the server would receive
    // "{a;b}". The connection-string grammar has no other escape, so reject
    // the value instead of silently authenticating with a different name.
    if (options->psqlDriver) {
        const char* what = NULL;

        if (username && *username && strchr(username, ';'))
            what = "user name";
        else if (dbName && *dbName && strchr(dbName, ';'))
            what = "database name";

        if (what)
            return diag(result_t::make(CALL_E_INVALID_DATA).with_type_name(errtype::kTypeError).with_code("ERR_INVALID_ARG_VALUE").with_format("odbc: the PostgreSQL ODBC driver cannot carry a ';' in a %s.", what).with_result(result_t::invalid_arg()));
    }

    SQLRETURN hr;
    if (!g_odbc) {
        hr = SQLAllocEnv((SQLHENV*)&g_odbc);
        if (hr < 0)
            return diag(result_t::make(CALL_E_INTERNAL).with_type_name(errtype::kError).with_code("ERR_INTERNAL").with_message("odbc: unable to allocate environment."));
    }

    hr = SQLAllocConnect(g_odbc, (SQLHDBC*)&conn);
    if (hr < 0) {
        odbc_disconnect(conn);
        conn = NULL;
        return diag(result_t::make(CALL_E_INTERNAL).with_type_name(errtype::kError).with_code("ERR_INTERNAL").with_message("odbc: unable to allocate connection."));
    }

    if (*driver) {
        // The ODBC grammar lets a driver remove the braces around any keyword
        // value; psqlODBC does it for Pwd and a few others only. A driver of
        // the second kind must receive the value literally (which also means
        // a ';' has no representation there, see the check above).
        auto value = [&](const char* str) -> exlib::string {
            if (options->psqlDriver)
                return str;
            return safe_conn_string(str);
        };

        auto build_conn_str = [&](bool use_hostport) -> exlib::string {
            exlib::string conn_str;

            conn_str.append("Driver=");
            conn_str.append(safe_conn_string(driver));
            conn_str.append(1, ';');

            conn_str.append(options->serverAttr);
            conn_str.append(1, '=');
            conn_str.append(value(host));
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
                conn_str.append(value(dbName));
                conn_str.append(1, ';');
            }

            if (*username) {
                conn_str.append("Uid=");
                conn_str.append(value(username));
                conn_str.append(1, ';');
            }

            if (*password) {
                conn_str.append("Pwd=");
                // The URL decode above is the only decode the password may
                // take. psqlODBC decodes Pwd as form data unless the value is
                // brace-quoted, and removes those braces itself; the other
                // ODBC drivers take the value literally (DM rejects the
                // braces), so the forced quoting is a psql-only measure.
                if (options->psqlDriver)
                    conn_str.append(safe_password(password));
                else
                    conn_str.append(safe_conn_string(password));
                conn_str.append(1, ';');
            }

            // psqlODBC's Windows builds default LFConversion to on and rewrite
            // every LF in a text value to CRLF. The runtime returns the stored
            // value byte for byte, so turn the rewrite off (the option is
            // already off by default on the other platforms).
            if (options->psqlDriver)
                conn_str.append("LFConversion=0;");

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
            OdbcDiagInfo info = odbc_error_info(SQL_HANDLE_DBC, conn);
            if (!first_err.empty() && err != first_err) {
                err = first_err + "\n    " + err;
            }
            odbc_disconnect(conn);
            conn = NULL;

            result_t::builder b = result_t::make(CALL_E_INVALID_CALL);
            if (!info.sqlstate.empty())
                b = std::move(b).with_code(info.sqlstate);
            if (info.has_native)
                b = std::move(b).with_errno(info.native);
            return diag(std::move(b).with_message(err));
        }
    }

    return result_t();
}

result_t odbc_connect(exlib::string connString, const char* driver, int32_t port, void*& conn,
    const OdbcConnectOptions* options)
{
    obj_ptr<Url> u = new Url();

    result_t hr = u->parse(connString);
    if (hr.failed())
        return hr;

    obj_ptr<URLSearchParams_base> q;
    u->get_searchParams(q);
    Variant v;

    exlib::string port_ = u->port();
    if (port_.length() > 0)
        port = atoi(port_.c_str());

    exlib::string str;
    hr = q->first("Driver", v);
    if (hr.is_return_null())
        hr = q->first("driver", v);

    if (!hr.is_return_null()) {
        str = v.string();
        driver = str.c_str();
    } else if (driver == NULL)
        return result_t::make(CALL_E_INVALID_DATA).with_type_name(errtype::kTypeError).with_code("ERR_INVALID_ARG_VALUE").with_message("odbc: no driver specified.");

    exlib::string pathname = u->pathname();

    // The database name is a URL path component: decode the percent escapes
    // exactly once, like the userinfo above (node-postgres does the same).
    // A SQLite path is not a URL and stays literal in openSQLite().
    exlib::string dbName;
    if (pathname.length() > 1)
        Url::decodeURI(pathname.c_str() + 1, dbName);

    exlib::string username;
    exlib::string password;
    Url::decodeURI(u->username(), username);
    Url::decodeURI(u->password(), password);

    hr = odbc_connect(driver, u->hostname().c_str(), port,
        username.c_str(), password.c_str(), dbName.c_str(), conn, options);
    if (hr.failed())
        return hr;

    return result_t();
}

// Shared column value fetch (used by execute and Statement cursor;
// implemented later in this file)
static result_t odbc_fetchValue(SQLHSTMT stmt, int32_t col, SQLLEN type,
    const exlib::string& typeName, Variant& v);
static bool odbc_type_name_is(const exlib::string& typeName, const char* name);
static bool odbc_type_name_contains(const exlib::string& typeName, const char* text);

static void odbc_get_type_name(SQLHSTMT stmt, int32_t col, exlib::string& name)
{
    SQLSMALLINT buflen = 0;
    SQLWCHAR buf[SQL_MAX_COLUMN_NAME_LEN];
    SQLRETURN hr = SQLColAttributeW(stmt, col, SQL_DESC_TYPE_NAME, buf,
        SQL_MAX_COLUMN_NAME_LEN * sizeof(SQLWCHAR), &buflen, NULL);
    if (hr >= 0 && buflen > 0)
        name = utf16to8String((const char16_t*)buf, buflen / sizeof(SQLWCHAR));
}

result_t odbc_execute(void* conn, int32_t* activeStmt, exlib::string sql, obj_ptr<NArray>& retVal, AsyncHandle ac)
{
    if (ac.isSync())
        return result_t::longsync();

    if (!conn)
        return db_closed();

    if (activeStmt && *activeStmt)
        return db_stmt_busy_error();

    SQLRETURN hr;
    SQLHSTMT stmt;
    result_t result;

    hr = SQLAllocStmt(conn, &stmt);
    if (hr < 0)
        return odbc_set_error(SQL_HANDLE_DBC, conn);

    do {
        bool more = false;

        exlib::wstring wsql(utf8to16String(sql));
        hr = SQLExecDirectW(stmt, (SQLWCHAR*)wsql.c_str(), (SQLINTEGER)wsql.length());
        if (hr < 0)
            break;

        do {
            obj_ptr<DBResult> res;
            std::vector<SQLLEN> types;
            std::vector<exlib::string> typeNames;
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
            typeNames.resize(columns);
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

                odbc_get_type_name(stmt, i + 1, typeNames[i]);
                if (types[i] == SQL_VARCHAR
                    && (odbc_type_name_is(typeNames[i], "boolean")
                        || odbc_type_name_is(typeNames[i], "bool")))
                    types[i] = SQL_BIT;
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

                    result = odbc_fetchValue(stmt, i + 1, types[i], typeNames[i], v);
                    if (result.failed())
                        break;

                    res->rowValue(i, v);
                }
                if (result.failed())
                    break;

                res->endRow();
            }
            if (hr < 0 || result.failed())
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

    // Keep the diagnostic as a value: the SQLSTATE, the native code and the
    // driver text are already collected by odbc_set_error(), and rebuilding
    // a bare code here is what turned every server-side error into
    // "[20009] Invalid procedure call.".  A column fetch error wins because
    // it may describe the failing value more precisely than the handle.
    if (!result.failed() && hr < 0)
        result = odbc_set_error(SQL_HANDLE_STMT, stmt);

    SQLFreeStmt(stmt, SQL_DROP);

    return result;
}

// ---------------------------------------------------------------------------
// ODBC Statement cursor implementation (SQLPrepare + row-by-row SQLFetch)
// ---------------------------------------------------------------------------

// Fetch one column value by type (shared by execute and Statement cursor)
#ifndef SQL_SS_TIMESTAMPOFFSET
#define SQL_SS_TIMESTAMPOFFSET -155
#endif
#ifndef SQL_SS_TIME2
#define SQL_SS_TIME2 -154
#endif

static result_t odbc_fetch_text(SQLHSTMT stmt, int32_t col, Variant& v)
{
    char buf[4096];
    SQLLEN len = 0;
    SQLRETURN hr = SQLGetData(stmt, col, SQL_C_CHAR, buf, sizeof(buf), &len);
    if (hr < 0)
        return odbc_set_error(SQL_HANDLE_STMT, stmt);
    if (len == SQL_NULL_DATA)
        v.setNull();
    else if (len >= (SQLLEN)sizeof(buf))
        return result_t::make(CALL_E_INTERNAL).with_type_name(errtype::kError)
            .with_code("ERR_INTERNAL").with_message("odbc: text value is too large.");
    else
        v = exlib::string(buf, (int32_t)len);
    return result_t();
}

static bool odbc_type_name_is(const exlib::string& typeName, const char* name)
{
    return !qstricmp(typeName.c_str(), name);
}

static bool odbc_type_name_contains(const exlib::string& typeName, const char* text)
{
    return typeName.find(text) != exlib::string::npos;
}

static bool odbc_is_timezone_timestamp(const exlib::string& typeName)
{
    if (odbc_type_name_is(typeName, "timetz")
        || odbc_type_name_contains(typeName, "time with time zone"))
        return false;

    return odbc_type_name_is(typeName, "timestamptz")
        || odbc_type_name_is(typeName, "datetimeoffset")
        || odbc_type_name_contains(typeName, "with time zone")
        || odbc_type_name_contains(typeName, "tz");
}

static result_t odbc_fetch_timezone_timestamp(SQLHSTMT stmt, int32_t col, Variant& v)
{
    exlib::string text;
    result_t hr = odbc_fetch_text(stmt, col, v);
    if (hr.failed() || v.type() == Variant::VT_Null)
        return hr;

    text = v.string();

    int32_t year, month, day, hour, minute, second;
    char sep;
    if (sscanf(text.c_str(), "%d-%d-%d%c%d:%d:%d", &year, &month, &day,
            &sep, &hour, &minute, &second)
        != 7)
        return result_t::make(CALL_E_INVALID_DATA).with_type_name(errtype::kError)
            .with_code("ERR_INVALID_DATA").with_format("odbc: invalid timestamp with time zone '%s'.", text.c_str());

    int32_t millisecond = 0;
    const char* p = text.c_str();
    const char* dot = strchr(p, '.');
    if (dot && dot > p) {
        const char* q = dot + 1;
        int32_t scale = 100;
        for (int32_t i = 0; i < 3 && qisdigit(*q); i++, q++) {
            millisecond += (*q - '0') * scale;
            scale /= 10;
        }
    }

    // ODBC text for a timezone-aware value normally carries an explicit
    // offset. psqlODBC prints the session timezone (UTC by default) without
    // one, so an offset-less value is treated as UTC rather than local.
    int32_t offsetMinutes = 0;
    const char* offset = strpbrk(p + 10, "+-Zz");
    if (offset) {
        if (*offset == 'Z' || *offset == 'z')
            offsetMinutes = 0;
        else {
            int32_t sign = *offset == '-' ? -1 : 1;
            int32_t hh = 0, mm = 0;
            if (sscanf(offset + 1, "%d:%d", &hh, &mm) >= 1)
                offsetMinutes = sign * (hh * 60 + mm);
        }
    }

    date_t d;
    d.create(year, month, day, hour, minute, second, millisecond);
    if (d.empty())
        return result_t::make(CALL_E_INVALID_DATA).with_type_name(errtype::kError)
            .with_code("ERR_INVALID_DATA").with_format("odbc: invalid timestamp with time zone '%s'.", text.c_str());

    v = date_t(d.date() - offsetMinutes * 60000.0);
    return result_t();
}

// ---------------------------------------------------------------------------
// Special logical types: JSON, UUID, arrays and intervals
//
// ODBC exposes no SQL type for JSON/arrays, only the backend type name from
// SQL_DESC_TYPE_NAME.  The helpers below normalize the textual representation
// reported by PostgreSQL/Kingbase (json, uuid, _int4, interval, ...).
// ---------------------------------------------------------------------------

static bool odbc_is_json_type(const exlib::string& typeName)
{
    return odbc_type_name_is(typeName, "json")
        || odbc_type_name_is(typeName, "jsonb");
}

static bool odbc_is_uuid_type(const exlib::string& typeName)
{
    return odbc_type_name_is(typeName, "uuid")
        || odbc_type_name_is(typeName, "uniqueidentifier");
}

static bool odbc_is_array_type(const exlib::string& typeName)
{
    return typeName.length() > 0
        && (typeName[0] == '_'
            || odbc_type_name_contains(typeName, "[]"));
}

static exlib::string odbc_array_element_type(const exlib::string& typeName)
{
    exlib::string type = typeName;

    if (type.length() > 1 && type[0] == '_')
        return type.substr(1);

    exlib::string::size_type pos = type.find("[]");
    if (pos != exlib::string::npos)
        return type.substr(0, pos);

    return type;
}

static void odbc_array_skip_space(const char*& p)
{
    while (*p && qisspace(*p))
        p++;
}

static void odbc_array_scalar(exlib::string token, bool quoted,
    const exlib::string& elementType, Variant& v)
{
    if (!quoted && !qstricmp(token.c_str(), "NULL")) {
        v.setNull();
        return;
    }

    if (odbc_is_json_type(elementType)) {
        v.setJSON(token);
        return;
    }

    if (odbc_is_uuid_type(elementType)) {
        exlib::qstrlwr(token);
        v = token;
        return;
    }

    if (odbc_type_name_is(elementType, "bool")
        || odbc_type_name_is(elementType, "boolean")) {
        v = !qstricmp(token.c_str(), "t")
            || !qstricmp(token.c_str(), "true")
            || !qstricmp(token.c_str(), "1");
        return;
    }

    if (odbc_type_name_is(elementType, "int8")
        || odbc_type_name_is(elementType, "bigint")) {
        v = token;
        return;
    }

    if (odbc_type_name_is(elementType, "int2")
        || odbc_type_name_is(elementType, "int4")
        || odbc_type_name_is(elementType, "smallint")
        || odbc_type_name_is(elementType, "integer")
        || odbc_type_name_is(elementType, "serial")) {
        v.parseNumber(token.c_str(), (int32_t)token.length());
        return;
    }

    if (odbc_type_name_is(elementType, "numeric")
        || odbc_type_name_is(elementType, "decimal")
        || odbc_type_name_is(elementType, "money")) {
        v = token;
        return;
    }

    if (odbc_type_name_is(elementType, "float4")
        || odbc_type_name_is(elementType, "float8")
        || odbc_type_name_is(elementType, "real")
        || odbc_type_name_is(elementType, "float")
        || odbc_type_name_is(elementType, "double precision")
        || odbc_type_name_is(elementType, "double")) {
        v.parseNumber(token.c_str(), (int32_t)token.length());
        return;
    }

    if (odbc_type_name_is(elementType, "date")
        || odbc_type_name_is(elementType, "time")
        || odbc_type_name_is(elementType, "timetz")) {
        v = token;
        return;
    }

    if (odbc_type_name_is(elementType, "datetime")
        || odbc_type_name_is(elementType, "timestamp")) {
        v.parseDate(token.c_str(), (int32_t)token.length());
        return;
    }

    // The PostgreSQL ODBC driver reports array columns as "unknown", losing
    // the element type.  Keep unquoted integer literals as numbers; quoted
    // and decimal values stay strings so no precision is lost silently.
    if ((elementType.empty()
            || odbc_type_name_is(elementType, "unknown"))
        && !quoted) {
        const char* p = token.c_str();
        bool integer = true;
        bool hasDigit = false;

        if (*p == '-' || *p == '+')
            p++;

        for (; *p; p++) {
            if (*p < '0' || *p > '9') {
                integer = false;
                break;
            }
            hasDigit = true;
        }

        if (integer && hasDigit && token.length() < 19) {
            v.parseNumber(token.c_str(), (int32_t)token.length());
            return;
        }
    }

    v = token;
}

static result_t odbc_array_value(const char*& p, const exlib::string& elementType,
    Variant& v)
{
    odbc_array_skip_space(p);
    if (*p != '{')
        return result_t::make(CALL_E_INVALID_DATA).with_type_name(errtype::kError)
            .with_code("ERR_INVALID_DATA").with_message("odbc: invalid array literal.");

    p++;
    obj_ptr<NArray> arr = new NArray();

    while (true) {
        odbc_array_skip_space(p);
        if (!*p)
            return result_t::make(CALL_E_INVALID_DATA).with_type_name(errtype::kError)
                .with_code("ERR_INVALID_DATA").with_message("odbc: unterminated array literal.");

        if (*p == '}') {
            p++;
            break;
        }

        if (*p == '{') {
            Variant child;
            result_t hr = odbc_array_value(p, elementType, child);
            if (hr.failed())
                return hr;
            arr->append(child);
        } else {
            exlib::string token;
            bool quoted = false;

            if (*p == '"') {
                quoted = true;
                p++;
                while (*p && *p != '"') {
                    if (*p == '\\' && p[1])
                        p++;
                    token.append(1, *p++);
                }
                if (*p == '"')
                    p++;
            } else {
                const char* start = p;
                while (*p && *p != ',' && *p != '}')
                    p++;
                const char* end = p;
                while (end > start && qisspace(end[-1]))
                    end--;
                while (start < end && qisspace(*start))
                    start++;
                token = exlib::string(start, (int32_t)(end - start));
            }

            Variant item;
            odbc_array_scalar(token, quoted, elementType, item);
            arr->append(item);
        }

        odbc_array_skip_space(p);
        if (*p == ',') {
            p++;
            continue;
        }
        if (*p == '}') {
            p++;
            break;
        }

        return result_t::make(CALL_E_INVALID_DATA).with_type_name(errtype::kError)
            .with_code("ERR_INVALID_DATA").with_message("odbc: invalid array literal.");
    }

    v = arr;
    return result_t();
}

// PostgreSQL prints intervals as "1 year 2 mons 3 days 04:05:06.789".
// Convert that form to ISO 8601; leave already-ISO or unparseable text alone.
static bool odbc_interval_to_iso(const exlib::string& text, exlib::string& iso)
{
    if (text.empty())
        return false;

    const char* p = text.c_str();
    while (*p && qisspace(*p))
        p++;
    if (*p == 'P' || *p == 'p')
        return false;

    int32_t years = 0, months = 0, days = 0, hours = 0, minutes = 0;
    double seconds = 0;
    bool hasPart = false;

    while (*p) {
        while (*p && qisspace(*p))
            p++;
        if (!*p)
            break;

        if (*p == '@') {
            p++;
            continue;
        }

        const char* end = p;
        while (*end && !qisspace(*end))
            end++;

        exlib::string token(p, (int32_t)(end - p));

        if (strchr(token.c_str(), ':')) {
            int32_t h = 0, m = 0;
            double sec = 0;
            int32_t timeSign = token.c_str()[0] == '-' ? -1 : 1;
            int32_t n = sscanf(token.c_str(), "%d:%d:%lf", &h, &m, &sec);
            if (n < 2)
                return false;

            hours += timeSign * (h < 0 ? -h : h);
            minutes += timeSign * (m < 0 ? -m : m);
            if (n == 3)
                seconds += timeSign * (sec < 0 ? -sec : sec);
            hasPart = true;
            p = end;
            continue;
        }

        char* numberEnd = NULL;
        double value = strtod(p, &numberEnd);
        if (numberEnd == p)
            return false;

        p = numberEnd;
        while (*p && qisspace(*p))
            p++;

        const char* unitStart = p;
        while (*p && qisalpha(*p))
            p++;

        exlib::string unit(unitStart, (int32_t)(p - unitStart));
        int32_t amount = (int32_t)value;
        int32_t sign = value < 0 ? -1 : 1;
        if (amount < 0)
            amount = -amount;

        if (!qstricmp(unit.c_str(), "year") || !qstricmp(unit.c_str(), "years"))
            years += sign * amount;
        else if (!qstricmp(unit.c_str(), "mon") || !qstricmp(unit.c_str(), "mons")
            || !qstricmp(unit.c_str(), "month") || !qstricmp(unit.c_str(), "months"))
            months += sign * amount;
        else if (!qstricmp(unit.c_str(), "day") || !qstricmp(unit.c_str(), "days"))
            days += sign * amount;
        else if (!qstricmp(unit.c_str(), "hour") || !qstricmp(unit.c_str(), "hours"))
            hours += sign * amount;
        else if (!qstricmp(unit.c_str(), "min") || !qstricmp(unit.c_str(), "mins")
            || !qstricmp(unit.c_str(), "minute") || !qstricmp(unit.c_str(), "minutes"))
            minutes += sign * amount;
        else if (!qstricmp(unit.c_str(), "sec") || !qstricmp(unit.c_str(), "secs")
            || !qstricmp(unit.c_str(), "second") || !qstricmp(unit.c_str(), "seconds"))
            seconds += sign * amount;
        else
            return false;

        hasPart = true;
    }

    if (!hasPart || (years == 0 && months == 0 && days == 0 && hours == 0
            && minutes == 0 && seconds == 0)) {
        iso = "PT0S";
        return true;
    }

    // ISO 8601 has one sign for the whole duration.  Mixed signs cannot be
    // represented, so keep the backend text in that case.
    if ((years < 0 || months < 0 || days < 0 || hours < 0 || minutes < 0
            || seconds < 0)
        && (years > 0 || months > 0 || days > 0 || hours > 0 || minutes > 0
            || seconds > 0))
        return false;

    bool negative = years < 0 || months < 0 || days < 0 || hours < 0
        || minutes < 0 || seconds < 0;

    if (negative) {
        years = -years;
        months = -months;
        days = -days;
        hours = -hours;
        minutes = -minutes;
        seconds = -seconds;
    }

    iso.clear();
    if (negative)
        iso.append(1, '-');
    iso.append(1, 'P');
    if (years)
        iso.append(std::to_string(years)).append(1, 'Y');
    if (months)
        iso.append(std::to_string(months)).append(1, 'M');
    if (days)
        iso.append(std::to_string(days)).append(1, 'D');

    if (hours || minutes || seconds) {
        iso.append(1, 'T');
        if (hours)
            iso.append(std::to_string(hours)).append(1, 'H');
        if (minutes)
            iso.append(std::to_string(minutes)).append(1, 'M');
        if (seconds) {
            char buf[64];
            int32_t n = snprintf(buf, sizeof(buf), "%.6f", seconds);
            while (n > 0 && buf[n - 1] == '0')
                n--;
            if (n > 0 && buf[n - 1] == '.')
                n--;
            iso.append(buf, n).append(1, 'S');
        }
    }

    return true;
}

static bool odbc_text_looks_interval(const exlib::string& text)
{
    exlib::string lower = text;
    exlib::qstrlwr(lower);

    return lower.find(" year") != exlib::string::npos
        || lower.find(" mon") != exlib::string::npos
        || lower.find(" day") != exlib::string::npos
        || lower.find(" hour") != exlib::string::npos
        || lower.find(" min") != exlib::string::npos
        || lower.find(" sec") != exlib::string::npos;
}

static result_t odbc_normalize_text_value(const exlib::string& typeName, Variant& v)
{
    if (v.type() != Variant::VT_String)
        return result_t();

    bool unknown = typeName.empty()
        || odbc_type_name_is(typeName, "unknown");
    const exlib::string& text = v.string();

    bool looksJson = false;
    bool looksArray = false;
    if (unknown) {
        const char* p = text.c_str();
        while (*p && qisspace(*p))
            p++;

        if (*p == '[')
            looksJson = true;
        else if (*p == '{') {
            bool inQuote = false;
            bool escaped = false;
            const char* q = p + 1;

            for (; *q; q++) {
                if (inQuote) {
                    if (escaped)
                        escaped = false;
                    else if (*q == '\\')
                        escaped = true;
                    else if (*q == '"')
                        inQuote = false;
                } else if (*q == '"')
                    inQuote = true;
                else if (*q == ':') {
                    looksJson = true;
                    break;
                }
            }

            if (!looksJson)
                looksArray = true;
        }
    }

    if (odbc_is_json_type(typeName) || looksJson) {
        exlib::string json = text;
        v.setJSON(json);
    } else if (odbc_is_uuid_type(typeName)) {
        exlib::string uuid = text;
        exlib::qstrlwr(uuid);
        v = uuid;
    } else if (odbc_is_array_type(typeName) || looksArray) {
        exlib::string arrayText = text;
        const char* p = arrayText.c_str();
        Variant array;

        result_t hr = odbc_array_value(
            p, odbc_array_element_type(typeName), array);
        if (hr.failed())
            return hr;

        odbc_array_skip_space(p);
        if (*p)
            return result_t::make(CALL_E_INVALID_DATA).with_type_name(errtype::kError)
                .with_code("ERR_INVALID_DATA").with_message("odbc: invalid array literal.");

        v = array;
    } else if (odbc_type_name_is(typeName, "interval")
        || (unknown && odbc_text_looks_interval(text))) {
        exlib::string iso;
        if (odbc_interval_to_iso(text, iso))
            v = iso;
    }

    return result_t();
}

static result_t odbc_fetch_time(SQLHSTMT stmt, int32_t col, Variant& v)
{
    TIMESTAMP_STRUCT value;
    SQLLEN len = 0;
    SQLRETURN hr = SQLGetData(stmt, col, SQL_C_TIMESTAMP, &value, sizeof(value), &len);
    if (hr >= 0) {
        if (len == SQL_NULL_DATA) {
            v.setNull();
            return result_t();
        }

        char buf[40];
        int32_t n = snprintf(buf, sizeof(buf), "%02d:%02d:%02d",
            value.hour, value.minute, value.second);
        if (value.fraction > 0) {
            int32_t micro = value.fraction / 1000;
            int32_t digits = 6;
            while (digits > 0 && micro % 10 == 0) {
                micro /= 10;
                digits--;
            }
            if (digits > 0)
                n += snprintf(buf + n, sizeof(buf) - n, ".%0*d", digits, micro);
        }

        v = exlib::string(buf, n);
        return result_t();
    }

    return odbc_fetch_text(stmt, col, v);
}

// Fetch one column value by type (shared by execute and Statement cursor)
static result_t odbc_fetch_timestamp_local(SQLHSTMT stmt, int32_t col, Variant& v)
{
    TIMESTAMP_STRUCT value;
    SQLLEN len = 0;
    SQLRETURN hr = SQLGetData(stmt, col, SQL_C_TIMESTAMP, &value, sizeof(value), &len);
    if (hr < 0)
        return odbc_set_error(SQL_HANDLE_STMT, stmt);
    if (len == SQL_NULL_DATA)
        v.setNull();
    else {
        date_t d;
        d.create(value.year, value.month, value.day, value.hour, value.minute,
            value.second, value.fraction / 1000000);
        d.toUTC();
        v = d;
    }
    return result_t();
}

// Fetch one column value by type (shared by execute and Statement cursor)
static result_t odbc_fetchValue(SQLHSTMT stmt, int32_t col, SQLLEN type,
    const exlib::string& typeName, Variant& v)
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
            return odbc_set_error(SQL_HANDLE_STMT, stmt);
        if (len == SQL_NULL_DATA)
            v.setNull();
        else
            v = value;
        break;
    }
    case SQL_NUMERIC:
    case SQL_DECIMAL:
    case SQL_BIGINT:
        return odbc_fetch_text(stmt, col, v);
    case SQL_FLOAT:
    case SQL_REAL:
    case SQL_DOUBLE: {
        double value;
        hr = SQLGetData(stmt, col, SQL_C_DOUBLE, &value, sizeof(value), &len);
        if (hr < 0)
            return odbc_set_error(SQL_HANDLE_STMT, stmt);
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
            return odbc_set_error(SQL_HANDLE_STMT, stmt);
        if (len == SQL_NULL_DATA)
            v.setNull();
        else
            v = value != 0;
        break;
    }
    case SQL_TYPE_DATE:
        return odbc_fetch_text(stmt, col, v);
    case SQL_TYPE_TIME:
        if (odbc_type_name_is(typeName, "timetz")
            || odbc_type_name_contains(typeName, "time with time zone"))
            return odbc_fetch_text(stmt, col, v);
        return odbc_fetch_time(stmt, col, v);
    case SQL_TYPE_TIMESTAMP:
    case SQL_TIMESTAMP:
    case SQL_SS_TIMESTAMPOFFSET:
        if (odbc_is_timezone_timestamp(typeName) || type == SQL_SS_TIMESTAMPOFFSET)
            return odbc_fetch_timezone_timestamp(stmt, col, v);
        return odbc_fetch_timestamp_local(stmt, col, v);
    case SQL_DATE:
    case SQL_TIME:
    case SQL_SS_TIME2:
        if (odbc_type_name_is(typeName, "date"))
            return odbc_fetch_text(stmt, col, v);
        if (odbc_type_name_is(typeName, "timetz")
            || odbc_type_name_contains(typeName, "time with time zone"))
            return odbc_fetch_text(stmt, col, v);
        if (odbc_type_name_is(typeName, "time")
            || type == SQL_TIME || type == SQL_SS_TIME2)
            return odbc_fetch_time(stmt, col, v);
        if (odbc_is_timezone_timestamp(typeName))
            return odbc_fetch_timezone_timestamp(stmt, col, v);
        if (odbc_type_name_contains(typeName, "datetime")
            || odbc_type_name_contains(typeName, "timestamp"))
            return odbc_fetch_timestamp_local(stmt, col, v);
        return odbc_fetch_text(stmt, col, v);
    case SQL_BINARY:
    case SQL_VARBINARY:
    case SQL_LONGVARBINARY: {
        exlib::string value;
        hr = SQLGetData(stmt, col, SQL_C_BINARY, value.data(), 0, &len);
        if (hr < 0)
            return odbc_set_error(SQL_HANDLE_STMT, stmt);
        if (len == SQL_NULL_DATA)
            v.setNull();
        else {
            value.resize(len);
            hr = SQLGetData(stmt, col, SQL_C_BINARY, value.data(), len, &len);
            if (hr < 0)
                return odbc_set_error(SQL_HANDLE_STMT, stmt);
            v = new Buffer(value.c_str(), value.length());
        }
        break;
    }
    default: {
        if (odbc_type_name_is(typeName, "date"))
            return odbc_fetch_text(stmt, col, v);
        if (odbc_type_name_is(typeName, "time")
            || odbc_type_name_is(typeName, "timetz"))
            return odbc_fetch_text(stmt, col, v);
        if (odbc_is_timezone_timestamp(typeName))
            return odbc_fetch_timezone_timestamp(stmt, col, v);

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
                return odbc_set_error(SQL_HANDLE_STMT, stmt);
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

        if (len != SQL_NULL_DATA) {
            v = utf16to8String(value);

            result_t hr = odbc_normalize_text_value(typeName, v);
            if (hr.failed())
                return hr;
        }
        break;
    }
    }

    return result_t();
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
        if (esc.boolean) {
            exlib::string s = esc.boolean(v.boolVal());
            str.append(s);
        } else
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

    case Variant::VT_Date:
        if (esc.date) {
            exlib::string s = esc.date(v.dateValue());
            str.append(s);
        } else {
            exlib::string s;
            v.toString(s);
            str.append(esc.string(s));
        }
        break;

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
    return result_t();
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
        if (hr.failed())
            return hr;

        SQLRETURN sqlhr = SQLAllocStmt(m_conn, &m_stmt);
        if (sqlhr < 0)
            return odbc_set_error(SQL_HANDLE_DBC, m_conn);

        exlib::wstring wsql(utf8to16String(full));
        sqlhr = SQLPrepareW(m_stmt, (SQLWCHAR*)wsql.c_str(),
            (SQLINTEGER)wsql.length());
        if (sqlhr < 0) {
            result_t hr = odbc_set_error(SQL_HANDLE_STMT, m_stmt);
            SQLFreeStmt(m_stmt, SQL_DROP);
            m_stmt = NULL;
            return hr;
        }

        sqlhr = SQLExecute(m_stmt);
        if (sqlhr < 0) {
            result_t hr = odbc_set_error(SQL_HANDLE_STMT, m_stmt);
            SQLFreeStmt(m_stmt, SQL_DROP);
            m_stmt = NULL;
            return hr;
        }

        // Column metadata (available after execution)
        SQLSMALLINT columns = 0;
        sqlhr = SQLNumResultCols(m_stmt, &columns);
        if (sqlhr < 0) {
            result_t hr = odbc_set_error(SQL_HANDLE_STMT, m_stmt);
            SQLFreeStmt(m_stmt, SQL_DROP);
            m_stmt = NULL;
            return hr;
        }

        m_columns = columns;
        m_types.resize(m_columns);
        m_typeNames.resize(m_columns);
        m_names.resize(m_columns);

        for (int32_t i = 0; i < m_columns; i++) {
            SQLSMALLINT buflen;
            SQLWCHAR buf[SQL_MAX_COLUMN_NAME_LEN];
            sqlhr = SQLColAttributeW(m_stmt, i + 1, SQL_DESC_NAME, buf,
                SQL_MAX_COLUMN_NAME_LEN * sizeof(SQLWCHAR), &buflen, NULL);
            if (sqlhr < 0) {
                result_t hr = odbc_set_error(SQL_HANDLE_STMT, m_stmt);
                SQLFreeStmt(m_stmt, SQL_DROP);
                m_stmt = NULL;
                return hr;
            }

            m_names[i] = normalize_identifier(
                utf16to8String((const char16_t*)buf, buflen / sizeof(SQLWCHAR)));

            sqlhr = SQLColAttributeW(m_stmt, i + 1, SQL_DESC_TYPE, NULL, 0,
                NULL, &m_types[i]);
            if (sqlhr < 0) {
                result_t hr = odbc_set_error(SQL_HANDLE_STMT, m_stmt);
                SQLFreeStmt(m_stmt, SQL_DROP);
                m_stmt = NULL;
                return hr;
            }

            odbc_get_type_name(m_stmt, i + 1, m_typeNames[i]);
            if (m_types[i] == SQL_VARCHAR
                && (odbc_type_name_is(m_typeNames[i], "boolean")
                    || odbc_type_name_is(m_typeNames[i], "bool")))
                m_types[i] = SQL_BIT;
        }

        hasResult = (m_columns > 0);
        if (m_activeStmt)
            *m_activeStmt = 1;
        return result_t();
    }

    virtual result_t fetchRow(Statement::RowSink* row, bool& done)
    {
        if (!m_stmt)
            return result_t::closed();

        SQLRETURN hr = SQLFetch(m_stmt);
        if (hr < 0)
            return odbc_set_error(SQL_HANDLE_STMT, m_stmt);
        if (hr == SQL_NO_DATA) {
            done = true;
            return result_t();
        }

        for (int32_t i = 0; i < m_columns; i++) {
            Variant v;
            result_t hr2 = odbc_fetchValue(m_stmt, i + 1, m_types[i], m_typeNames[i], v);
            if (hr2.failed())
                return hr2;
            row->add(m_names[i], v);
        }
        return result_t();
    }

    virtual result_t runResult(int64_t& changes, int64_t& lastInsertId)
    {
        if (!m_stmt)
            return result_t::closed();

        SQLLEN affected = 0;
        SQLRowCount(m_stmt, &affected);
        changes = (int64_t)affected;
        return result_t();
    }

    virtual result_t columns(std::vector<obj_ptr<Statement_base::ColumnsType>>& retVal)
    {
        return result_t::make(CALL_E_INVALID_CALL).with_type_name(errtype::kError).with_code("ERR_NOT_SUPPORTED").with_message("ODBC: columns() requires a prepared statement (v2)");
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
        m_typeNames.clear();
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
    std::vector<exlib::string> m_typeNames;
    std::vector<exlib::string> m_names;
};

// Generic escape callbacks (odbc engine default)
static const OdbcEscape odbcEscape = { odbcEscapeString, odbcEscapeBinary };

result_t odbc_prepareStmt(void* conn, int32_t* activeStmt, exlib::string sql,
    const OdbcEscape& esc, obj_ptr<Statement_base>& retVal)
{
    if (!conn)
        return errhelp::closed("ERR_INVALID_STATE", "ODBC: database is closed.");

    obj_ptr<Statement> stmt = new Statement(sql,
        new OdbcStmtImpl(conn, activeStmt, sql, esc));
    retVal = stmt;
    return result_t();
}

result_t odbc_getTables(void* conn, obj_ptr<NArray>& retVal, AsyncHandle ac)
{
    if (ac.isSync())
        return result_t::longsync();

    if (!conn)
        return db_closed();

    SQLRETURN hr;
    SQLHSTMT stmt;

    hr = SQLAllocStmt(conn, &stmt);
    if (hr < 0)
        return odbc_set_error(SQL_HANDLE_DBC, conn);

    // Use ODBC SQLTables to get table list - more compatible across different databases
    hr = SQLTablesA(stmt, NULL, 0, NULL, 0, NULL, 0, (SQLCHAR*)"TABLE", SQL_NTS);
    if (hr < 0) {
        SQLFreeStmt(stmt, SQL_DROP);
        return odbc_set_error(SQL_HANDLE_STMT, stmt);
    }

    obj_ptr<DBResult> res = new DBResult(1);
    res->setField(0, "name"); // Match SQLite format with 'name' field

    while (true) {
        hr = SQLFetch(stmt);
        if (hr == SQL_NO_DATA)
            break;
        if (hr < 0) {
            SQLFreeStmt(stmt, SQL_DROP);
            return odbc_set_error(SQL_HANDLE_STMT, stmt);
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
    return result_t();
}

result_t odbc_getTableInfo(void* conn, exlib::string tableName, obj_ptr<NArray>& retVal, AsyncHandle ac)
{
    if (ac.isSync())
        return result_t::longsync();

    if (!conn)
        return db_closed();

    SQLRETURN hr;
    SQLHSTMT stmt;

    hr = SQLAllocStmt(conn, &stmt);
    if (hr < 0)
        return odbc_set_error(SQL_HANDLE_DBC, conn);

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
            return odbc_set_error(SQL_HANDLE_STMT, stmt);

        while (true) {
            hr2 = SQLFetch(stmt);
            if (hr2 == SQL_NO_DATA)
                break;
            if (hr2 < 0)
                return result_t::make(CALL_E_INVALID_CALL).with_type_name(errtype::kError).with_code("ERR_SQL_EXECUTE").with_message(odbc_error(SQL_HANDLE_STMT, stmt));

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

        return result_t();
    };

    int32_t rows = 0;
    result_t r = fetchColumns(tableName, rows);
    if (r.failed()) {
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
            if (r.failed()) {
                SQLFreeStmt(stmt, SQL_DROP);
                return r;
            }
        }
    }

    SQLFreeStmt(stmt, SQL_DROP);
    retVal = res;
    return result_t();
}

} /* namespace fibjs */
