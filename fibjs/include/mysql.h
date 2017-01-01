/*
 * mysql.h
 *
 *  Created on: Jul 20, 2012
 *      Author: lion
 */

#ifndef MYSQL_H_
#define MYSQL_H_

#include "ifs/MySQL.h"
extern "C"
{
#include <umysql/include/umysql.h>
}

namespace fibjs
{

class mysql: public MySQL_base
{
public:
    mysql() :
        m_conn(NULL)
    {
    }

    virtual ~mysql();

public:
    // DbConnection_base
    virtual result_t get_type(exlib::string& retVal);
    virtual result_t close(AsyncEvent *ac);
    virtual result_t begin(AsyncEvent *ac);
    virtual result_t commit(AsyncEvent *ac);
    virtual result_t rollback(AsyncEvent *ac);
    virtual result_t execute(exlib::string sql, obj_ptr<DBResult_base> &retVal, AsyncEvent *ac);
    virtual result_t execute(exlib::string sql, const v8::FunctionCallbackInfo<v8::Value> &args, obj_ptr<DBResult_base> &retVal);
    virtual result_t format(exlib::string sql, const v8::FunctionCallbackInfo<v8::Value> &args, exlib::string &retVal);

public:
    // MySQL_base
    virtual result_t use(exlib::string dbName, AsyncEvent *ac);
    virtual result_t get_rxBufferSize(int32_t &retVal);
    virtual result_t set_rxBufferSize(int32_t newVal);
    virtual result_t get_txBufferSize(int32_t &retVal);
    virtual result_t set_txBufferSize(int32_t newVal);

public:
    result_t connect(const char *host, int32_t port, const char *username,
                     const char *password, const char *dbName);
    result_t execute(const char *sql, int32_t sLen, obj_ptr<DBResult_base> &retVal);

private:
    inline result_t error()
    {
        const char *errorMessage = NULL;
        int32_t errCode;
        int32_t errType;

        if (UMConnection_GetLastError(m_conn, &errorMessage, &errCode,
                                      &errType))
            return Runtime::setError(errorMessage);
        return Runtime::errNumber();
    }

private:
    UMConnection m_conn;
};

} /* namespace fibjs */
#endif /* MYSQL_H_ */
