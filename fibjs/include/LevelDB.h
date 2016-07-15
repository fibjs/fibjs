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

    ~LevelDB();

public:
    // LevelDB_base
    virtual result_t has(Buffer_base *key, bool &retVal, AsyncEvent *ac);
    virtual result_t get(Buffer_base *key, obj_ptr<Buffer_base> &retVal, AsyncEvent *ac);
    virtual result_t mget(v8::Local<v8::Array> keys, obj_ptr<List_base> &retVal);
    virtual result_t set(Buffer_base *key, Buffer_base *value, AsyncEvent *ac);
    virtual result_t mset(v8::Local<v8::Object> map);
    virtual result_t mremove(v8::Local<v8::Array> keys);
    virtual result_t remove(Buffer_base *key, AsyncEvent *ac);
    virtual result_t forEach(v8::Local<v8::Function> func);
    virtual result_t between(Buffer_base *from, Buffer_base *to, v8::Local<v8::Function> func);
    virtual result_t begin(obj_ptr<LevelDB_base> &retVal);
    virtual result_t commit();
    virtual result_t close(AsyncEvent *ac);

public:
    result_t open(const char *connString);

    static result_t getValue(v8::Local<v8::Value> v, exlib::string &out)
    {
        obj_ptr<Buffer_base> bKey = Buffer_base::getInstance(v);
        if (bKey)
            return bKey->toString(out);

        return GetArgumentValue(v, out);
    }

private:
    result_t _commit(leveldb::WriteBatch *batch, AsyncEvent *ac);
    ASYNC_MEMBER1(LevelDB, _commit, leveldb::WriteBatch *);

    result_t _mget(std::vector<exlib::string> *keys, obj_ptr<List_base> &retVal, AsyncEvent *ac);
    ASYNC_MEMBERVALUE2(LevelDB, _mget, std::vector<exlib::string> *, obj_ptr<List_base>);

    leveldb::DB *db()
    {
        if (m_base)
            return m_base->m_db;
        return m_db;
    }

    leveldb::Status Set(const leveldb::Slice &key, const leveldb::Slice &value)
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

#define ITER_BLOCK_SIZE     32

    class Iter : public object_base
    {
    public:
        Iter(leveldb::DB *db) : m_count(0), m_first(true), m_end(false)
        {
            m_it = db->NewIterator(leveldb::ReadOptions());
        }

        ~Iter()
        {
            delete m_it;
        }

        result_t _iter(AsyncEvent *ac);
        ASYNC_MEMBER0(LevelDB::Iter, _iter);

        result_t iter(Isolate* isolate, v8::Local<v8::Function> func);

        result_t getValue(Buffer_base *from, Buffer_base *to)
        {
            from->toString(m_from);
            to->toString(m_to);
            return 0;
        }

    public:
        obj_ptr<Buffer_base> m_kvs[ITER_BLOCK_SIZE * 2];
        leveldb::Iterator *m_it;
        int32_t m_count;
        bool m_first;
        bool m_end;
        exlib::string m_from, m_to;
    };

private:
    leveldb::DB *m_db;
    obj_ptr<LevelDB> m_base;
    leveldb::WriteBatch *m_batch;
};

} /* namespace fibjs */
#endif /* LEVELDB_H_ */
