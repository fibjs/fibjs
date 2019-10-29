/*
 * SQLite.h
 *
 *  Created on: Aug 1, 2012
 *      Author: lion
 */

#include "ifs/SQLite.h"
#include <sqlite/sqlite3.h>

#ifndef SQLITE_H_
#define SQLITE_H_

namespace fibjs {

class SQLite : public SQLite_base {
public:
    SQLite()
        : m_conn(NULL)
        , m_nCmdTimeout(10000)
    {
    }

    ~SQLite();

public:
    // DbConnection_base
    virtual result_t get_type(exlib::string& retVal);
    virtual result_t close(AsyncEvent* ac);
    virtual result_t begin(AsyncEvent* ac);
    virtual result_t commit(AsyncEvent* ac);
    virtual result_t rollback(AsyncEvent* ac);
    virtual result_t trans(v8::Local<v8::Function> func, bool& retVal);
    virtual result_t execute(exlib::string sql, OptArgs args, obj_ptr<NArray>& retVal, AsyncEvent* ac);
    virtual result_t insert(v8::Local<v8::Object> opts, double& retVal, AsyncEvent* ac);
    virtual result_t find(v8::Local<v8::Object> opts, obj_ptr<NArray>& retVal, AsyncEvent* ac);
    virtual result_t count(v8::Local<v8::Object> opts, int32_t& retVal, AsyncEvent* ac);
    virtual result_t update(v8::Local<v8::Object> opts, int32_t& retVal, AsyncEvent* ac);
    virtual result_t format(v8::Local<v8::Object> opts, exlib::string& retVal);
    virtual result_t remove(v8::Local<v8::Object> opts, int32_t& retVal, AsyncEvent* ac);
    virtual result_t format(exlib::string sql, OptArgs args, exlib::string& retVal);

public:
    // SQLite_base
    virtual result_t get_fileName(exlib::string& retVal);
    virtual result_t get_timeout(int32_t& retVal);
    virtual result_t set_timeout(int32_t newVal);
    virtual result_t backup(exlib::string fileName, AsyncEvent* ac);

public:
    result_t open(const char* file);
    result_t execute(const char* sql, int32_t sLen, obj_ptr<NArray>& retVal);
    result_t format(exlib::string method, v8::Local<v8::Object> opts, exlib::string& retVal);

public:
    sqlite3* m_conn;

private:
    exlib::string m_file;
    int32_t m_nCmdTimeout;
};

} /* namespace fibjs */
#endif /* SQLITE_H_ */
