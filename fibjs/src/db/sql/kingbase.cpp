/*
 * kingbase.cpp
 */

#include "object.h"
#include "ifs/db.h"
#include "ifs/URLSearchParams.h"

#include "kingbase.h"
#include "Url.h"

namespace fibjs {

#ifdef _WIN32
// KODBC registers itself under a versioned name on Windows. Try the names
// used by the current releases; a '?Driver=' override wins and is not
// retried.
static const char* s_win_drivers[] = {
    "KingbaseES 9 ODBC Driver Unicode",
    "KingbaseES 9 ODBC Driver ANSI",
    "KingbaseES 8.6 ODBC Driver Unicode",
    "KingbaseES 8.6 ODBC Driver ANSI",
    "KingbaseES 8 ODBC Driver",
};

static bool has_driver_override(exlib::string connString)
{
    obj_ptr<Url> u = new Url();
    if (u->parse(connString).failed())
        return false;

    obj_ptr<URLSearchParams_base> q;
    u->get_searchParams(q);

    Variant v;
    result_t hr = q->first("Driver", v);
    if (hr.is_return_null())
        hr = q->first("driver", v);

    return !hr.is_return_null();
}
#endif

result_t db_base::openKingbase(exlib::string connString, obj_ptr<DbConnection_base>& retVal,
    AsyncHandle ac)
{
    if (ac.isSync())
        return result_t::longsync();

    if (qstrcmp(connString.c_str(), "kingbase:", 9))
        return result_t::make(CALL_E_INVALID_DATA).with_type_name(errtype::kTypeError).with_code("ERR_INVALID_ARG_VALUE").with_message("openKingbase: connection string must start with 'kingbase:'.").with_result(result_t::invalid_arg());

    int32_t port = 54321;
    obj_ptr<kingbase> conn = new kingbase();

    // KODBC is derived from psqlODBC and keeps the same connection-string
    // rules: the password is decoded as form data unless brace-quoted, the
    // other values are taken literally, and the LF rewrite must be disabled.
    OdbcConnectOptions options = { "Servername", false, false, true };

    result_t hr;
#ifdef _WIN32
    if (has_driver_override(connString)) {
        hr = odbc_connect(connString, s_win_drivers[0], port, conn->m_conn, &options);
    } else {
        for (size_t i = 0; i < sizeof(s_win_drivers) / sizeof(s_win_drivers[0]); i++) {
            hr = odbc_connect(connString, s_win_drivers[i], port, conn->m_conn, &options);
            if (hr.ok())
                break;

            // Only a missing driver name has another candidate; a server or
            // authentication failure must be reported as it is.
            const ErrorPayload* p = hr.payload();
            if (!p || p->code != "IM002")
                break;
        }
    }
#else
    hr = odbc_connect(connString, "kdbodbcw.so", port, conn->m_conn, &options);
#endif
    if (hr.failed())
        return hr;

    retVal = conn;
    return result_t();
}

} /* namespace fibjs */
