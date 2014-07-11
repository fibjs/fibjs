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
    if (!ac)
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
    if (!m_db)
        return CALL_E_INVALID_CALL;

    if (!ac)
        return CALL_E_NOSYNC;

    std::string key1;
    key->toString(key1);

    leveldb::Slice sKey = key1;

    std::string value;
    leveldb::Status s = m_db->Get(leveldb::ReadOptions(), sKey, &value);
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
    if (!m_db)
        return CALL_E_INVALID_CALL;

    if (!ac)
        return CALL_E_NOSYNC;

    leveldb::Slice sKey = key;

    std::string value;
    leveldb::Status s = m_db->Get(leveldb::ReadOptions(), sKey, &value);
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
    if (!m_db)
        return CALL_E_INVALID_CALL;

    if (!ac)
        return CALL_E_NOSYNC;

    std::string key1;
    key->toString(key1);

    leveldb::Slice sKey = key1;

    std::string value;
    leveldb::Status s = m_db->Get(leveldb::ReadOptions(), sKey, &value);
    if (s.IsNotFound())
        return CALL_RETURN_NULL;
    else if (!s.ok())
        return Runtime::setError(s.ToString());

    retVal = new Buffer(value);

    return 0;
}

result_t LevelDB::get(const char *key, obj_ptr<Buffer_base> &retVal, exlib::AsyncEvent *ac)
{
    if (!m_db)
        return CALL_E_INVALID_CALL;

    if (!ac)
        return CALL_E_NOSYNC;

    leveldb::Slice sKey = key;

    std::string value;
    leveldb::Status s = m_db->Get(leveldb::ReadOptions(), sKey, &value);
    if (s.IsNotFound())
        return CALL_RETURN_NULL;
    else if (!s.ok())
        return Runtime::setError(s.ToString());

    retVal = new Buffer(value);

    return 0;
}

result_t LevelDB::_commit(leveldb::WriteBatch *batch, exlib::AsyncEvent *ac)
{
    if (!ac)
        return CALL_E_NOSYNC;

    leveldb::Status s = m_db->Write(leveldb::WriteOptions(), batch);
    if (!s.ok())
        return Runtime::setError(s.ToString());

    return 0;
}

result_t LevelDB::put(v8::Local<v8::Object> map)
{
    if (!m_db)
        return CALL_E_INVALID_CALL;

    leveldb::WriteBatch batch;

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

        batch.Put(sKey, sValue);
    }

    return ac__commit(&batch);
}

result_t LevelDB::put(Buffer_base *key, Buffer_base *value, exlib::AsyncEvent *ac)
{
    if (!m_db)
        return CALL_E_INVALID_CALL;

    if (!ac)
        return CALL_E_NOSYNC;

    std::string key1;
    key->toString(key1);

    std::string value1;
    value->toString(value1);

    leveldb::Slice sKey = key1;
    leveldb::Slice sValue = value1;

    leveldb::Status s = m_db->Put(leveldb::WriteOptions(), sKey, sValue);
    if (!s.ok())
        return Runtime::setError(s.ToString());

    return 0;
}

result_t LevelDB::put(const char *key, Buffer_base *value, exlib::AsyncEvent *ac)
{
    if (!m_db)
        return CALL_E_INVALID_CALL;

    if (!ac)
        return CALL_E_NOSYNC;

    std::string value1;
    value->toString(value1);

    leveldb::Slice sKey = key;
    leveldb::Slice sValue = value1;

    leveldb::Status s = m_db->Put(leveldb::WriteOptions(), sKey, sValue);
    if (!s.ok())
        return Runtime::setError(s.ToString());

    return 0;
}

result_t LevelDB::remove(Buffer_base *key, exlib::AsyncEvent *ac)
{
    if (!m_db)
        return CALL_E_INVALID_CALL;

    if (!ac)
        return CALL_E_NOSYNC;

    std::string key1;
    key->toString(key1);

    leveldb::Slice sKey = key1;

    std::string value;
    leveldb::Status s = m_db->Delete(leveldb::WriteOptions(), sKey);
    if (!s.ok())
        return Runtime::setError(s.ToString());

    return 0;
}

result_t LevelDB::remove(const char *key, exlib::AsyncEvent *ac)
{
    if (!m_db)
        return CALL_E_INVALID_CALL;

    if (!ac)
        return CALL_E_NOSYNC;

    leveldb::Slice sKey = key;

    std::string value;
    leveldb::Status s = m_db->Delete(leveldb::WriteOptions(), sKey);
    if (!s.ok())
        return Runtime::setError(s.ToString());

    return 0;
}

result_t LevelDB::close(exlib::AsyncEvent *ac)
{
    if (!m_db)
        return 0;

    if (!ac)
        return CALL_E_NOSYNC;

    if (m_db)
    {
        delete m_db;
        m_db = NULL;
    }
    return 0;
}

}
