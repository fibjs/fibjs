/*
 * LevelDB.cpp
 *
 *  Created on: Jul 10, 2014
 *      Author: lion
 */

#include "object.h"
#include "LevelDB.h"
#include "ifs/db.h"
#include "Buffer.h"
#include "List.h"

namespace fibjs {

result_t db_base::openLevelDB(exlib::string connString,
    obj_ptr<LevelDB_base>& retVal, AsyncEvent* ac)
{
    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    result_t hr;
    const char* c_str = connString.c_str();

    if (!qstrcmp(c_str, "leveldb:", 8))
        c_str += 8;

    obj_ptr<LevelDB> db = new LevelDB();
    hr = db->open(c_str);
    if (hr < 0)
        return hr;

    retVal = db;

    return 0;
}

result_t LevelDB::open(const char* connString)
{
    leveldb::Options options;
    options.create_if_missing = true;
    leveldb::Status s = leveldb::DB::Open(options, connString, &m_db);
    if (!s.ok())
        return CHECK_ERROR(Runtime::setError(s.ToString()));

    return 0;
}

result_t close_ldb(leveldb::DB* db)
{
    delete db;
    return 0;
}

LevelDB::~LevelDB()
{
    if (m_batch) {
        m_batch->Clear();
        delete m_batch;
    } else if (m_db)
        asyncCall(close_ldb, m_db);
}

result_t LevelDB::has(Buffer_base* key, bool& retVal, AsyncEvent* ac)
{
    if (!db())
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    exlib::string key1;
    key->toString(key1);

    std::string value;
    leveldb::Status s = db()->Get(leveldb::ReadOptions(),
        leveldb::Slice(key1.c_str(), key1.length()),
        &value);
    if (s.IsNotFound()) {
        retVal = false;
        return 0;
    } else if (!s.ok())
        return CHECK_ERROR(Runtime::setError(s.ToString()));

    retVal = true;

    return 0;
}

result_t LevelDB::get(Buffer_base* key, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
{
    if (!db())
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    exlib::string key1;
    key->toString(key1);

    std::string value;
    leveldb::Status s = db()->Get(leveldb::ReadOptions(),
        leveldb::Slice(key1.c_str(), key1.length()),
        &value);
    if (s.IsNotFound())
        return CALL_RETURN_NULL;
    else if (!s.ok())
        return CHECK_ERROR(Runtime::setError(s.ToString()));

    retVal = new Buffer(value);

    return 0;
}

result_t LevelDB::_mget(std::vector<exlib::string>* keys,
    obj_ptr<List_base>& retVal, AsyncEvent* ac)
{
    std::vector<exlib::string>& ks = *keys;
    obj_ptr<List> list = new List();
    int32_t i;

    Variant nil;
    nil.setNull();

    for (i = 0; i < (int32_t)ks.size(); i++) {
        std::string value;

        leveldb::Status s = db()->Get(leveldb::ReadOptions(),
            leveldb::Slice(ks[i].c_str(), ks[i].length()),
            &value);
        if (s.IsNotFound())
            list->append(nil);
        else if (!s.ok())
            return CHECK_ERROR(Runtime::setError(s.ToString()));
        else
            list->append(new Buffer(value));
    }

    retVal = list;

    return 0;
}

result_t LevelDB::mget(v8::Local<v8::Array> keys, obj_ptr<List_base>& retVal)
{
    std::vector<exlib::string> ks;
    int32_t len = keys->Length();
    int32_t i;
    result_t hr;

    if (!len)
        return CALL_RETURN_NULL;

    ks.resize(len);

    Isolate* isolate = holder();

    for (i = 0; i < len; i++) {
        v8::Local<v8::Value> v = keys->Get(i);
        obj_ptr<Buffer_base> buf;

        hr = GetArgumentValue(isolate->m_isolate, v, buf);
        if (hr < 0)
            return CHECK_ERROR(hr);

        exlib::string s;
        buf->toString(s);

        ks[i] = s;
    }

    return ac__mget(&ks, retVal);
}

result_t LevelDB::_commit(leveldb::WriteBatch* batch, AsyncEvent* ac)
{
    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    leveldb::Status s = db()->Write(leveldb::WriteOptions(), batch);
    if (!s.ok())
        return CHECK_ERROR(Runtime::setError(s.ToString()));

    return 0;
}

result_t LevelDB::set(Buffer_base* key, Buffer_base* value, AsyncEvent* ac)
{
    if (!db())
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    exlib::string key1;
    key->toString(key1);

    exlib::string value1;
    value->toString(value1);

    leveldb::Status s = Set(leveldb::Slice(key1.c_str(), key1.length()),
        leveldb::Slice(value1.c_str(), value1.length()));
    if (!s.ok())
        return CHECK_ERROR(Runtime::setError(s.ToString()));

    return 0;
}

result_t LevelDB::mset(v8::Local<v8::Object> map)
{
    if (!db())
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    leveldb::WriteBatch batch;
    leveldb::WriteBatch* batch_ = m_batch ? m_batch : &batch;

    v8::Local<v8::Array> ks = map->GetPropertyNames();
    int32_t len = ks->Length();
    int32_t i;
    result_t hr;

    for (i = 0; i < len; i++) {
        v8::Local<v8::Value> k = ks->Get(i);
        v8::String::Utf8Value uk(k);
        exlib::string key(*uk, uk.length());

        exlib::string value1;
        hr = getValue(map->Get(k), value1);
        if (hr < 0)
            return hr;

        batch_->Put(leveldb::Slice(key.c_str(), key.length()),
            leveldb::Slice(value1.c_str(), value1.length()));
    }

    if (m_batch)
        return 0;

    return ac__commit(&batch);
}

result_t LevelDB::mremove(v8::Local<v8::Array> keys)
{
    if (!db())
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    leveldb::WriteBatch batch;
    leveldb::WriteBatch* batch_ = m_batch ? m_batch : &batch;

    int32_t len = keys->Length();
    int32_t i;
    result_t hr;

    for (i = 0; i < len; i++) {
        exlib::string key;
        hr = getValue(keys->Get(i), key);
        if (hr < 0)
            return hr;

        batch_->Delete(leveldb::Slice(key.c_str(), key.length()));
    }

    if (m_batch)
        return 0;

    return ac__commit(&batch);
}

result_t LevelDB::remove(Buffer_base* key, AsyncEvent* ac)
{
    if (!db())
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    exlib::string key1;
    key->toString(key1);

    exlib::string value;
    leveldb::Status s = Delete(leveldb::Slice(key1.c_str(), key1.length()));
    if (!s.ok())
        return CHECK_ERROR(Runtime::setError(s.ToString()));

    return 0;
}

result_t LevelDB::Iter::_iter(AsyncEvent* ac)
{
    m_count = 0;

    if (m_first) {
        if (m_from.empty())
            m_it->SeekToFirst();
        else
            m_it->Seek(leveldb::Slice(m_from.c_str(), m_from.length()));

        m_first = false;

        if (!m_it->Valid()) {
            m_end = true;
            return 0;
        }

        if (!m_from.empty()) {
            leveldb::Slice key = m_it->key();
            if (key.compare(leveldb::Slice(m_from.c_str(), m_from.length())) == 0) {
                m_it->Next();
                if (!m_it->Valid()) {
                    m_end = true;
                    return 0;
                }
            }
        }
    }

    while (m_count < ITER_BLOCK_SIZE) {
        leveldb::Slice key = m_it->key();
        if (!m_to.empty() && key.compare(leveldb::Slice(m_to.c_str(), m_to.length())) >= 0) {
            m_end = true;
            break;
        }

        m_kvs[m_count * 2] = new Buffer(key.ToString());
        m_kvs[m_count * 2 + 1] = new Buffer(m_it->value().ToString());

        m_count++;

        m_it->Next();
        if (!m_it->Valid()) {
            m_end = true;
            break;
        }
    }

    return 0;
}

result_t LevelDB::Iter::iter(Isolate* isolate, v8::Local<v8::Function> func)
{
    result_t hr;
    int32_t i;

    do {
        hr = ac__iter();
        if (hr < 0)
            return hr;

        for (i = 0; i < m_count; i++) {
            v8::Local<v8::Value> args[2] = {
                m_kvs[i * 2 + 1]->wrap(), m_kvs[i * 2]->wrap()
            };

            m_kvs[i * 2].Release();
            m_kvs[i * 2 + 1].Release();

            v8::Local<v8::Value> v = func->Call(v8::Undefined(isolate->m_isolate), 2, args);
            if (v.IsEmpty())
                return CALL_E_JAVASCRIPT;

            if (v->BooleanValue()) {
                m_end = true;
                break;
            }
        }
    } while (!m_end);

    return 0;
}

result_t LevelDB::forEach(v8::Local<v8::Function> func)
{
    if (!db())
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    return Iter(db()).iter(holder(), func);
}

result_t LevelDB::between(Buffer_base* from, Buffer_base* to, v8::Local<v8::Function> func)
{
    if (!db())
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    obj_ptr<Iter> it = new Iter(db());

    result_t hr = it->getValue(from, to);
    if (hr < 0)
        return hr;

    return it->iter(holder(), func);
}

result_t LevelDB::begin(obj_ptr<LevelDB_base>& retVal)
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
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    result_t hr = ac__commit(m_batch);
    close(NULL);

    return hr;
}

result_t LevelDB::close(AsyncEvent* ac)
{
    if (m_batch) {
        m_base.Release();

        m_batch->Clear();
        delete m_batch;
        m_batch = NULL;

        return 0;
    }

    if (!m_db)
        return 0;

    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    delete m_db;
    m_db = NULL;

    return 0;
}
}
