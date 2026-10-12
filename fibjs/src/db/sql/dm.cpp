/*
 * dm.cpp
 *
 *  Created on: Feb 11, 2026
 *      Author: lion
 */

#include "object.h"
#include "ifs/db.h"

#include "dm.h"
#include "Url.h"

namespace fibjs {

result_t db_base::openDM(exlib::string connString, obj_ptr<DbConnection_base>& retVal,
    AsyncHandle ac)
{
    if (ac.isSync())
        return result_t::longsync();

    if (qstrcmp(connString.c_str(), "dm:", 3))
        return result_t(result_t::make(CALL_E_EXCEPTION).with_message("openDM: connection string must start with 'dm:'.").with_result(CALL_E_INVALIDARG));

#ifdef _WIN32
    const char* driver = "DM8 ODBC DRIVER";
#else
    const char* driver = "libdodbc.so";
#endif
    int32_t port = 5236;
    obj_ptr<dm> conn = new dm();

    OdbcConnectOptions options = { "Server", true, true };
    result_t hr = odbc_connect(connString, driver, port, conn->m_conn, &options);
    if (hr.failed())
        return hr;

    retVal = conn;
    return 0;
}

} /* namespace fibjs */
