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
    AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_LONGSYNC);

    if (qstrcmp(connString.c_str(), "dm:", 3))
        return CHECK_ERROR(CALL_E_INVALIDARG);

#ifdef _WIN32
    const char* driver = "DM8 ODBC DRIVER";
#else
    const char* driver = "libdodbc.so";
#endif
    int32_t port = 5236;
    obj_ptr<dm> conn = new dm();

    result_t hr = odbc_connect(connString, driver, port, conn->m_conn);
    if (hr < 0)
        return hr;

    retVal = conn;
    return 0;
}

} /* namespace fibjs */
