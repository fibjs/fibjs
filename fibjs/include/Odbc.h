/*
 * Odbc.h
 *
 *  Created on: May 9, 2021
 *      Author: lion
 */

#pragma once

#include "ifs/Odbc.h"

namespace fibjs {

class Odbc : public Odbc_base {
public:
    Odbc()
    {
    }

    ~Odbc();

public:
    // DbConnection_base
    virtual result_t get_type(exlib::string& retVal);
    virtual result_t close(AsyncEvent* ac);
    virtual result_t begin(exlib::string point, AsyncEvent* ac);
    virtual result_t commit(exlib::string point, AsyncEvent* ac);
    virtual result_t rollback(exlib::string point, AsyncEvent* ac);
    virtual result_t trans(v8::Local<v8::Function> func, bool& retVal);
    virtual result_t trans(exlib::string point, v8::Local<v8::Function> func, bool& retVal);
    virtual result_t execute(exlib::string sql, OptArgs args, obj_ptr<NArray>& retVal, AsyncEvent* ac);
    virtual result_t createTable(v8::Local<v8::Object> opts, AsyncEvent* ac);
    virtual result_t dropTable(v8::Local<v8::Object> opts, AsyncEvent* ac);
    virtual result_t createIndex(v8::Local<v8::Object> opts, AsyncEvent* ac);
    virtual result_t dropIndex(v8::Local<v8::Object> opts, AsyncEvent* ac);
    virtual result_t insert(v8::Local<v8::Object> opts, double& retVal, AsyncEvent* ac);
    virtual result_t find(v8::Local<v8::Object> opts, obj_ptr<NArray>& retVal, AsyncEvent* ac);
    virtual result_t count(v8::Local<v8::Object> opts, int32_t& retVal, AsyncEvent* ac);
    virtual result_t update(v8::Local<v8::Object> opts, int32_t& retVal, AsyncEvent* ac);
    virtual result_t remove(v8::Local<v8::Object> opts, int32_t& retVal, AsyncEvent* ac);
    virtual result_t format(exlib::string method, v8::Local<v8::Object> opts, exlib::string& retVal);
    virtual result_t format(exlib::string sql, OptArgs args, exlib::string& retVal);

public:
    // Odbc_base
    virtual result_t use(exlib::string dbName, AsyncEvent* ac);

public:
    result_t connect(const char* driver, const char* host, int32_t port, const char* username,
        const char* password, const char* dbName);
    result_t execute(const char* sql, int32_t sLen, obj_ptr<NArray>& retVal);
    void close();
    exlib::string GetSQLError(int32_t handleType, void* handle);

public:
    void* m_env = NULL;
    void* m_conn = NULL;
};

} /* namespace fibjs */
