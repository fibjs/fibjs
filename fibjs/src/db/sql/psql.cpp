/*
 * psql.cpp
 *
 *  Created on: Jul 20, 2012
 *      Author: lion
 */

#include "object.h"
#include "ifs/db.h"
#include "ifs/URLSearchParams.h"

#ifndef _WIN32
#include <unistd.h>
#endif

#include "psql.h"
#include "Url.h"

namespace fibjs {

#ifdef __APPLE__
static bool has_driver_override(exlib::string connString)
{
    obj_ptr<Url> u = new Url();
    if (u->parse(connString) < 0)
        return false;

    obj_ptr<URLSearchParams_base> q;
    u->get_searchParams(q);

    Variant v;
    result_t hr = q->first("Driver", v);
    if (hr == CALL_RETURN_NULL)
        hr = q->first("driver", v);

    return hr != CALL_RETURN_NULL;
}

static const char* resolve_default_driver(exlib::string connString)
{
    if (has_driver_override(connString))
        return "psqlodbcw.so";

    if (access("/opt/homebrew/opt/psqlodbc/lib/psqlodbcw.so", F_OK) == 0)
        return "/opt/homebrew/opt/psqlodbc/lib/psqlodbcw.so";

    if (access("/usr/local/opt/psqlodbc/lib/psqlodbcw.so", F_OK) == 0)
        return "/usr/local/opt/psqlodbc/lib/psqlodbcw.so";

    return "psqlodbcw.so";
}
#endif

result_t db_base::openPSQL(exlib::string connString, obj_ptr<DbConnection_base>& retVal,
    AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_LONGSYNC);

    if (qstrcmp(connString.c_str(), "psql:", 5))
        return CHECK_ERROR(Runtime::setError(CALL_E_INVALIDARG, "openPSQL: connection string must start with 'psql:'."));

#ifdef _WIN32
    const char* driver = "PostgreSQL UNICODE";
#elif defined(__APPLE__)
    const char* driver = resolve_default_driver(connString);
#else
    const char* driver = "psqlodbcw.so";
#endif
    int32_t port = 5432;
    obj_ptr<psql> conn = new psql();

    result_t hr = odbc_connect(connString, driver, port, conn->m_conn);
    if (hr < 0)
        return hr;

    retVal = conn;
    return 0;
}

} /* namespace fibjs */
