/*
 * Odbc.h
 *
 *  Created on: May 9, 2021
 *      Author: lion
 */

#pragma once

#include "ifs/DbConnection.h"
#include "../db_tmpl.h"

namespace fibjs {

class Odbc : public db_tmpl<DbConnection_base, false, true> {
public:
    ~Odbc();

public:
    // DbConnection_base
    virtual result_t get_type(exlib::string& retVal);
    virtual result_t close(AsyncEvent* ac);
    virtual result_t execute(exlib::string sql, obj_ptr<NArray>& retVal, AsyncEvent* ac);

public:
    result_t connect(const char* driver, const char* host, int32_t port, const char* username,
        const char* password, const char* dbName);
    void close();
    exlib::string GetSQLError(int32_t handleType, void* handle);

public:
    void* m_env = NULL;
};

} /* namespace fibjs */
