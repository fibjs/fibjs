/*
 * SQLite.h
 *
 *  Created on: Aug 1, 2012
 *      Author: lion
 */

#pragma once

#include "ifs/SQLite.h"
#include <sqlite/sqlite3.h>
#include "../db_tmpl.h"

namespace fibjs {

class SQLite : public db_tmpl<SQLite_base, SQLite> {
public:
    SQLite()
        : m_nCmdTimeout(10000)
    {
    }

    ~SQLite();

public:
    // DbConnection_base
    virtual result_t get_type(exlib::string& retVal);
    virtual result_t close(AsyncEvent* ac);
    virtual result_t execute(exlib::string sql, obj_ptr<NArray>& retVal, AsyncEvent* ac);

public:
    // SQLite_base
    virtual result_t get_fileName(exlib::string& retVal);
    virtual result_t get_timeout(int32_t& retVal);
    virtual result_t set_timeout(int32_t newVal);
    virtual result_t backup(exlib::string fileName, AsyncEvent* ac);

public:
    result_t open(const char* file);

private:
    exlib::string m_file;
    int32_t m_nCmdTimeout;
};

} /* namespace fibjs */
