/*
 * LevelDB.h
 *
 *  Created on: Jul 10, 2014
 *      Author: lion
 */

#ifndef LEVELDB_H_
#define LEVELDB_H_

#include "ifs/LevelDB.h"
#include <leveldb/db.h>
#include <leveldb/write_batch.h>

namespace fibjs
{

class LevelDB: public LevelDB_base
{
    FIBER_FREE();

public:
    LevelDB() : m_db(NULL)
    {
    }

    ~LevelDB()
    {
        if (m_db)
        {
            if (exlib::Service::hasService())
                ac_close();
            else
            {
                asyncEvent ac;
                close(&ac);
            }
        }
    }

public:
    // LevelDB_base
    virtual result_t has(Buffer_base *key, bool &retVal, exlib::AsyncEvent *ac);
    virtual result_t has(const char *key, bool &retVal, exlib::AsyncEvent *ac);
    virtual result_t get(Buffer_base *key, obj_ptr<Buffer_base> &retVal, exlib::AsyncEvent *ac);
    virtual result_t get(const char *key, obj_ptr<Buffer_base> &retVal, exlib::AsyncEvent *ac);
    virtual result_t put(v8::Local<v8::Object> map);
    virtual result_t put(Buffer_base *key, Buffer_base *value, exlib::AsyncEvent *ac);
    virtual result_t put(const char *key, Buffer_base *value, exlib::AsyncEvent *ac);
    virtual result_t remove(Buffer_base *key, exlib::AsyncEvent *ac);
    virtual result_t remove(const char *key, exlib::AsyncEvent *ac);
    virtual result_t close(exlib::AsyncEvent *ac);

public:
    result_t open(const char *connString);

private:
    result_t _commit(leveldb::WriteBatch *batch, exlib::AsyncEvent *ac);
    ASYNC_MEMBER1(LevelDB, _commit, leveldb::WriteBatch *);

private:
    leveldb::DB *m_db;
};

} /* namespace fibjs */
#endif /* LEVELDB_H_ */
