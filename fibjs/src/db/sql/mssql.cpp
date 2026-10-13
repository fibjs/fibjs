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
    AsyncHandle ac)
{
    if (ac.isSync())
        return result_t::longsync();

    if (qstrcmp(connString.c_str(), "mssql:", 6))
        return result_t::make(CALL_E_EXCEPTION).with_message("openMSSQL: connection string must start with 'mssql:'.").with_result(result_t::invalid_arg());

#ifdef _WIN32
    const char* driver = "SQL Server";
#else
    const char* driver = "libtdsodbc.so";
#endif
    int32_t port = 1433;
    obj_ptr<mssql> conn = new mssql();

    result_t hr = odbc_connect(connString, driver, port, conn->m_conn);
    if (hr.failed())
        return hr;

    retVal = conn;
    return result_t();
}

} /* namespace fibjs */
