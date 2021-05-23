/*
 * mssql.cpp
 *
 *  Created on: Jul 20, 2012
 *      Author: lion
 */

#include "object.h"
#include "ifs/db.h"

#include "mssql.h"
#include "Url.h"

namespace fibjs {

result_t db_base::openMSSQL(exlib::string connString, obj_ptr<DbConnection_base>& retVal,
    AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_LONGSYNC);

    if (qstrcmp(connString.c_str(), "mssql:", 6))
        return CHECK_ERROR(CALL_E_INVALIDARG);

#ifdef _WIN32
    const char* driver = "SQL Server";
#else
    const char* driver = "libtdsodbc.so";
#endif
    int32_t port = 1433;
    obj_ptr<mssql> conn = new mssql();

    result_t hr = odbc_connect(connString, driver, port, conn->m_conn);
    if (hr < 0)
        return hr;

    retVal = conn;
    return 0;
}

} /* namespace fibjs */
