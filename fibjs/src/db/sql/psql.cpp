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

#ifdef _WIN32
    const char* driver = "PostgreSQL UNICODE";
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
