/*
 * LevelDB.cpp
 *
 *  Created on: Jul 10, 2014
 *      Author: lion
 */

#include "LevelDB.h"
#include "ifs/db.h"
#include "Buffer.h"

namespace fibjs
{

result_t db_base::openLevelDB(const char *connString,
                              obj_ptr<LevelDB_base> &retVal, exlib::AsyncEvent *ac)
{
    if (switchToAsync(ac))
        return CALL_E_NOSYNC;

    result_t hr;

    if (!qstrcmp(connString, "leveldb:", 8))
        connString += 8;

    obj_ptr<LevelDB> db = new LevelDB();
    hr = db->open(connString);
    if (hr < 0)
        return hr;

    retVal = db;

    return 0;
}

result_t LevelDB::open(const char *connString)
{
    leveldb::Options options;
    options.create_if_missing = true;
    leveldb::Status s = leveldb::DB::Open(options, connString, &m_db);
    if (!s.ok())
        return Runtime::setError(s.ToString());

    return 0;
}

result_t LevelDB::has(Buffer_base *key, bool &retVal, exlib::AsyncEvent *ac)
{
    if (!db())
        return CALL_E_INVALID_CALL;

    if (switchToAsync(ac))
        return CALL_E_NOSYNC;

    std::string key1;
    key->toString(key1);

    leveldb::Slice sKey = key1;

    std::string value;
    leveldb::Status s = db()->Get(leveldb::ReadOptions(), sKey, &value);
    if (s.IsNotFound())
    {
        retVal = false;
        return 0;
    }
    else if (!s.ok())
        return Runtime::setError(s.ToString());

    retVal = true;

    return 0;
}

result_t LevelDB::has(const char *key, bool &retVal, exlib::AsyncEvent *ac)
{
    if (!db())
        return CALL_E_INVALID_CALL;

    if (switchToAsync(ac))
        return CALL_E_NOSYNC;

    leveldb::Slice sKey = key;

    std::string value;
    leveldb::Status s = db()->Get(leveldb::ReadOptions(), sKey, &value);
    if (s.IsNotFound())
    {
        retVal = false;
        return 0;
    }
    else if (!s.ok())
        return Runtime::setError(s.ToString());

    retVal = true;

    return 0;
}

result_t LevelDB::get(Buffer_base *key, obj_ptr<Buffer_base> &retVal, exlib::AsyncEvent *ac)
{
    if (!db())
        return CALL_E_INVALID_CALL;

    if (switchToAsync(ac))
        return CALL_E_NOSYNC;

    std::string key1;
    key->toString(key1);

    leveldb::Slice sKey = key1;

    std::string value;
    leveldb::Status s = db()->Get(leveldb::ReadOptions(), sKey, &value);
    if (s.IsNotFound())
        return CALL_RETURN_NULL;
    else if (!s.ok())
        return Runtime::setError(s.ToString());

    retVal = new Buffer(value);

    return 0;
}

result_t LevelDB::get(const char *key, obj_ptr<Buffer_base> &retVal, exlib::AsyncEvent *ac)
{
    if (!db())
        return CALL_E_INVALID_CALL;

    if (switchToAsync(ac))
        return CALL_E_NOSYNC;

    leveldb::Slice sKey = key;

    std::string value;
    leveldb::Status s = db()->Get(leveldb::ReadOptions(), sKey, &value);
    if (s.IsNotFound())
        return CALL_RETURN_NULL;
    else if (!s.ok())
        return Runtime::setError(s.ToString());

    retVal = new Buffer(value);

    return 0;
}

result_t LevelDB::_commit(leveldb::WriteBatch *batch, exlib::AsyncEvent *ac)
{
    if (switchToAsync(ac))
        return CALL_E_NOSYNC;

    leveldb::Status s = db()->Write(leveldb::WriteOptions(), batch);
    if (!s.ok())
        return Runtime::setError(s.ToString());

    return 0;
}

result_t LevelDB::put(v8::Local<v8::Object> map)
{
    if (!db())
        return CALL_E_INVALID_CALL;

    leveldb::WriteBatch batch;
    leveldb::WriteBatch *batch_ = m_batch ? m_batch : &batch;

    v8::Local<v8::Array> ks = map->GetPropertyNames();
    int len = ks->Length();
    int i;

    for (i = 0; i < len; i++)
    {
        v8::Local<v8::Value> k = ks->Get(i);
        v8::String::Utf8Value uk(k);
        std::string key(*uk, uk.length());

        obj_ptr<Buffer_base> buf = Buffer_base::getInstance(map->Get(k));

        if (!buf)
            return CALL_E_TYPEMISMATCH;

        std::string value1;
        buf->toString(value1);

        leveldb::Slice sKey = key;
        leveldb::Slice sValue = value1;

        batch_->Put(sKey, sValue);
    }

    if (m_batch)
        return 0;

    return ac__commit(&batch);
}

result_t LevelDB::put(Buffer_base *key, Buffer_base *value, exlib::AsyncEvent *ac)
{
    if (!db())
        return CALL_E_INVALID_CALL;

    if (switchToAsync(ac))
        return CALL_E_NOSYNC;

    std::string key1;
    key->toString(key1);

    std::string value1;
    value->toString(value1);

    leveldb::Slice sKey = key1;
    leveldb::Slice sValue = value1;

    leveldb::Status s = Put(sKey, sValue);
    if (!s.ok())
        return Runtime::setError(s.ToString());

    return 0;
}

result_t LevelDB::put(const char *key, Buffer_base *value, exlib::AsyncEvent *ac)
{
    if (!db())
        return CALL_E_INVALID_CALL;

    if (switchToAsync(ac))
        return CALL_E_NOSYNC;

    std::string value1;
    value->toString(value1);

    leveldb::Slice sKey = key;
    leveldb::Slice sValue = value1;

    leveldb::Status s = Put(sKey, sValue);
    if (!s.ok())
        return Runtime::setError(s.ToString());

    return 0;
}

result_t LevelDB::remove(Buffer_base *key, exlib::AsyncEvent *ac)
{
    if (!db())
        return CALL_E_INVALID_CALL;

    if (switchToAsync(ac))
        return CALL_E_NOSYNC;

    std::string key1;
    key->toString(key1);

    leveldb::Slice sKey = key1;

    std::string value;
    leveldb::Status s = Delete(sKey);
    if (!s.ok())
        return Runtime::setError(s.ToString());

    return 0;
}

result_t LevelDB::remove(const char *key, exlib::AsyncEvent *ac)
{
    if (!db())
        return CALL_E_INVALID_CALL;

    if (switchToAsync(ac))
        return CALL_E_NOSYNC;

    leveldb::Slice sKey = key;

    std::string value;
    leveldb::Status s = Delete(sKey);
    if (!s.ok())
        return Runtime::setError(s.ToString());

    return 0;
}

result_t LevelDB::begin(obj_ptr<LevelDB_base> &retVal)
{
    obj_ptr<LevelDB> db = new LevelDB();

    db->m_base = this;
    db->m_batch = new leveldb::WriteBatch();

    retVal = db;

    return 0;
}

result_t LevelDB::commit()
{
    if (!db() || !m_batch)
        return CALL_E_INVALID_CALL;

    result_t hr = ac__commit(m_batch);
    close(NULL);

    return hr;
}

result_t LevelDB::close(exlib::AsyncEvent *ac)
{
    if (m_batch)
    {
        m_base.Release();

        m_batch->Clear();
        delete m_batch;
        m_batch = NULL;

        return 0;
    }

    if (!m_db)
        return 0;

    if (switchToAsync(ac))
        return CALL_E_NOSYNC;

    delete m_db;
    m_db = NULL;

    return 0;
}

}
