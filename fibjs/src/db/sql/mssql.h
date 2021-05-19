/*
 * mssql.h
 *
 *  Created on: Oct 29, 2016
 *      Author: lion
 */

#pragma once

#include "ifs/DbConnection.h"
#include "utf8.h"

#include "../db_tmpl.h"

namespace fibjs {

class mssql : public db_tmpl<DbConnection_base, false, true> {
public:
    virtual ~mssql();

public:
    // DbConnection_base
    virtual result_t get_type(exlib::string& retVal);
    virtual result_t close(AsyncEvent* ac);
    virtual result_t execute(exlib::string sql, obj_ptr<NArray>& retVal, AsyncEvent* ac);

public:
    result_t connect(const char* server, const char* username, const char* password, const char* dbName);
    result_t ado_error(HRESULT hr);
};

} /* namespace fibjs */
