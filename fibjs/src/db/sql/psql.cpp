/*
 * psql.cpp
 *
 *  Created on: Jul 20, 2012
 *      Author: lion
 */

#include "object.h"
#include "ifs/db.h"

#include "psql.h"
#include "Url.h"

namespace fibjs {

result_t db_base::openPSQL(exlib::string connString, obj_ptr<DbConnection_base>& retVal,
    AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_LONGSYNC);

    if (qstrcmp(connString.c_str(), "psql:", 5))
        return CHECK_ERROR(CALL_E_INVALIDARG);

    obj_ptr<Url> u = new Url();

    result_t hr = u->parse(connString);
    if (hr < 0)
        return hr;

    obj_ptr<HttpCollection_base> q;
    u->get_searchParams(q);
    Variant v;

    int32_t nPort = 5432;
    if (u->m_port.length() > 0)
        nPort = atoi(u->m_port.c_str());

    obj_ptr<psql> conn = new psql();

#ifdef _WIN32
    const char* driver = "PostgreSQL ANSI";
#else
    const char* driver = "psqlodbca.so";
#endif

    hr = odbc_connect(driver, u->m_hostname.c_str(), nPort,
        u->m_username.c_str(), u->m_password.c_str(),
        u->m_pathname.length() > 0 ? u->m_pathname.c_str() + 1 : "", conn->m_conn);
    if (hr < 0)
        return hr;

    retVal = conn;
    return 0;
}

} /* namespace fibjs */
