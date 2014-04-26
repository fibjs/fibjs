/*
 * SQLite.h
 *
 *  Created on: Aug 1, 2012
 *      Author: lion
 */

#include "ifs/SQLite.h"
#include <sqlite3.h>

#ifndef SQLITE_H_
#define SQLITE_H_

namespace fibjs
{

class SQLite: public SQLite_base
{
public:
    SQLite() :
        m_db(NULL), m_nCmdTimeout(5000)
    {
    }

    ~SQLite()
    {
        ac_close();
    }

public:
    // DbConnection_base
    virtual result_t close(exlib::AsyncEvent *ac);
    virtual result_t begin(exlib::AsyncEvent *ac);
    virtual result_t commit(exlib::AsyncEvent *ac);
    virtual result_t rollback(exlib::AsyncEvent *ac);
    virtual result_t execute(const char *sql, obj_ptr<DBResult_base> &retVal, exlib::AsyncEvent *ac);
    virtual result_t execute(const char *sql, const v8::FunctionCallbackInfo<v8::Value> &args, obj_ptr<DBResult_base> &retVal);
    virtual result_t format(const char *sql, const v8::FunctionCallbackInfo<v8::Value> &args, std::string &retVal);

public:
    // SQLite_base
    virtual result_t get_fileName(std::string &retVal);
    virtual result_t get_timeout(int32_t &retVal);
    virtual result_t set_timeout(int32_t newVal);
    virtual result_t backup(const char *fileName, exlib::AsyncEvent *ac);

public:
    result_t execute(const char *sql, int sLen, obj_ptr<DBResult_base> &retVal, exlib::AsyncEvent *ac);
    result_t open(const char *file, exlib::AsyncEvent *ac);

private:
    std::string m_file;
    sqlite3 *m_db;
    int m_nCmdTimeout;
};

} /* namespace fibjs */
#endif /* SQLITE_H_ */
