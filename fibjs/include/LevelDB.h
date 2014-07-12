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
    LevelDB() : m_db(NULL), m_batch(NULL)
    {
    }

    ~LevelDB()
    {
        if (m_batch)
        {
            asyncEvent ac;
            close(&ac);
        }
        else if (m_db)
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
    virtual result_t begin(obj_ptr<LevelDB_base> &retVal);
    virtual result_t commit();
    virtual result_t close(exlib::AsyncEvent *ac);

public:
    result_t open(const char *connString);

private:
    result_t _commit(leveldb::WriteBatch *batch, exlib::AsyncEvent *ac);
    ASYNC_MEMBER1(LevelDB, _commit, leveldb::WriteBatch *);
    leveldb::DB *db()
    {
        if (m_base)
            return m_base->m_db;
        return m_db;
    }

    leveldb::Status Put(const leveldb::Slice &key, const leveldb::Slice &value)
    {
        if (m_batch)
        {
            m_batch->Put(key, value);
            return leveldb::Status::OK();
        }

        return m_db->Put(leveldb::WriteOptions(), key, value);
    }

    leveldb::Status Delete(const leveldb::Slice &key)
    {
        if (m_batch)
        {
            m_batch->Delete(key);
            return leveldb::Status::OK();
        }

        return m_db->Delete(leveldb::WriteOptions(), key);
    }

private:
    leveldb::DB *m_db;
    obj_ptr<LevelDB> m_base;
    leveldb::WriteBatch *m_batch;
};

} /* namespace fibjs */
#endif /* LEVELDB_H_ */
