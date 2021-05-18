/*
 * mysql.h
 *
 *  Created on: Jul 20, 2012
 *      Author: lion
 */

#pragma once

#include "ifs/MySQL.h"
extern "C" {
#include <umysql/include/umysql.h>
}
#include "../db_tmpl.h"

namespace fibjs {

class mysql : public db_tmpl<MySQL_base, true, false> {
public:
    virtual ~mysql();

public:
    // DbConnection_base
    virtual result_t get_type(exlib::string& retVal);
    virtual result_t close(AsyncEvent* ac);
    virtual result_t execute(exlib::string sql, obj_ptr<NArray>& retVal, AsyncEvent* ac);

public:
    // MySQL_base
    virtual result_t get_rxBufferSize(int32_t& retVal);
    virtual result_t set_rxBufferSize(int32_t newVal);
    virtual result_t get_txBufferSize(int32_t& retVal);
    virtual result_t set_txBufferSize(int32_t newVal);

public:
    result_t connect(const char* host, int32_t port, const char* username,
        const char* password, const char* dbName);

private:
    inline result_t error()
    {
        const char* errorMessage = NULL;
        int32_t errCode;
        int32_t errType;

        if (UMConnection_GetLastError(m_conn, &errorMessage, &errCode,
                &errType))
            return Runtime::setError(errorMessage);
        return Runtime::errNumber();
    }
};

} /* namespace fibjs */
