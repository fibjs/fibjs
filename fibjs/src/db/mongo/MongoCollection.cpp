/*
 * MongoCollection.cpp
 *
 *  Created on: Oct 25, 2012
 *      Author: lion
 */

#include "object.h"
#include "MongoCollection.h"
#include "MongoCursor.h"
#include "encoding_bson.h"
#include <vector>

namespace fibjs
{

result_t MongoCollection::_batchInsert(std::vector<const bson *> pdata, int num, int32_t &retVal, AsyncEvent *ac)
{
    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    obj_ptr<MongoDB> db(m_db);
    if (!db)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    retVal = mongo_insert_batch(&db->m_conn, m_ns.c_str(), pdata.data(), num, NULL, 0);
    return 0;
}

result_t MongoCollection::_insert(const bson *data, int32_t &retVal, AsyncEvent *ac)
{
    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    obj_ptr<MongoDB> db(m_db);
    if (!db)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    retVal = mongo_insert(&db->m_conn, m_ns.c_str(), data, NULL);
    return 0;
}

result_t MongoCollection::_update(const bson *cond, const bson *op, int flags, int32_t &retVal, AsyncEvent *ac)
{
    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    obj_ptr<MongoDB> db(m_db);
    if (!db)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    retVal = mongo_update(&db->m_conn, m_ns.c_str(), cond, op, flags, NULL);
    return 0;
}

result_t MongoCollection::_remove(const bson *data, int32_t &retVal, AsyncEvent *ac)
{
    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    obj_ptr<MongoDB> db(m_db);
    if (!db)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    retVal = mongo_remove(&db->m_conn, m_ns.c_str(), data, NULL);
    return 0;
}

result_t MongoCollection::find(v8::Local<v8::Object> query,
                               v8::Local<v8::Object> projection, obj_ptr<MongoCursor_base> &retVal)
{
    obj_ptr<MongoDB> db(m_db);
    if (!db)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    retVal = new MongoCursor(db, m_ns, m_name, query, projection);
    return 0;
}

result_t MongoCollection::findOne(v8::Local<v8::Object> query,
                                  v8::Local<v8::Object> projection, v8::Local<v8::Object> &retVal)
{
    obj_ptr<MongoDB> db(m_db);
    if (!db)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    obj_ptr<MongoCursor> cur = new MongoCursor(db, m_ns, m_name, query,
            projection);
    obj_ptr<MongoCursor_base> cur1;

    cur->ac_limit(1, cur1);
    return cur->next(retVal);
}

result_t MongoCollection::findAndModify(v8::Local<v8::Object> query,
                                        v8::Local<v8::Object> &retVal)
{
    return runCommand("findAndModify", query, retVal);
}

result_t MongoCollection::insert(v8::Local<v8::Array> documents)
{
    obj_ptr<MongoDB> db(m_db);
    if (!db)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    std::vector<bson> bbs;
    std::vector<const bson *> pbbs;
    int32_t n = documents->Length();
    int32_t i;
    result_t hr;

    if (n > 0)
    {
        Isolate* isolate = holder();

        bbs.resize(n);
        pbbs.resize(n);

        for (i = 0; i < n; i++)
        {
            hr = encodeObject(isolate, &bbs[i], documents->Get(i));
            if (hr < 0)
            {
                n = i;
                for (i = 0; i < n; i++)
                    bson_destroy (&bbs[i]);
                return hr;
            }

            pbbs[i] = &bbs[i];
        }

        int32_t result = MONGO_ERROR;
        ac__batchInsert(pbbs, n, result);

        for (i = 0; i < n; i++)
            bson_destroy (&bbs[i]);

        if (result != MONGO_OK)
            return CHECK_ERROR(db->error());
    }

    return 0;
}

result_t MongoCollection::insert(v8::Local<v8::Object> document)
{
    obj_ptr<MongoDB> db(m_db);
    if (!db)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    bson bb;
    result_t hr;

    hr = encodeObject(holder(), &bb, document);
    if (hr < 0)
        return hr;

    int32_t result = MONGO_ERROR;
    ac__insert(&bb, result);

    bson_destroy(&bb);

    if (result != MONGO_OK)
        return CHECK_ERROR(db->error());

    return 0;
}

result_t MongoCollection::save(v8::Local<v8::Object> document)
{
    Isolate* isolate = holder();

    v8::Local<v8::String> strId = isolate->NewFromUtf8("_id", 3);
    v8::Local<v8::Value> id = document->Get(strId);

    if (IsEmpty (id))
        return insert(document);
    else
    {
        v8::Local<v8::Object> query = v8::Object::New(isolate->m_isolate);

        query->Set(strId, id);
        return update(query, document, true, false);
    }
}

result_t MongoCollection::update(v8::Local<v8::Object> query,
                                 v8::Local<v8::Object> document, bool upsert, bool multi)
{
    obj_ptr<MongoDB> db(m_db);
    if (!db)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    bson bbq, bbd;
    int32_t flags = (upsert ? MONGO_UPDATE_UPSERT : 0)
                    + (multi ? MONGO_UPDATE_MULTI : 0);

    Isolate* isolate = holder();
    result_t hr;

    hr = encodeObject(isolate, &bbq, query);
    if (hr < 0)
        return hr;

    hr = encodeObject(isolate, &bbd, document);
    if (hr < 0)
    {
        bson_destroy(&bbq);
        return hr;
    }

    int32_t result = MONGO_ERROR;
    ac__update(&bbq, &bbd, flags, result);

    bson_destroy(&bbq);
    bson_destroy(&bbd);

    if (result != MONGO_OK)
        return CHECK_ERROR(db->error());

    return 0;
}

result_t MongoCollection::update(v8::Local<v8::Object> query,
                                 v8::Local<v8::Object> document, v8::Local<v8::Object> options)
{
    Isolate* isolate = holder();
    return update(query, document,
                  options->Get(isolate->NewFromUtf8("upsert", 6))->BooleanValue(),
                  options->Get(isolate->NewFromUtf8("multi", 5))->BooleanValue());
}

result_t MongoCollection::remove(v8::Local<v8::Object> query)
{
    obj_ptr<MongoDB> db(m_db);
    if (!db)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    bson bbq;
    result_t hr;

    hr = encodeObject(holder(), &bbq, query);
    if (hr < 0)
        return hr;

    int32_t result = MONGO_ERROR;
    ac__remove(&bbq, result);

    bson_destroy(&bbq);

    if (result != MONGO_OK)
        return CHECK_ERROR(db->error());

    return 0;
}

result_t MongoCollection::runCommand(v8::Local<v8::Object> cmd,
                                     v8::Local<v8::Object> &retVal)
{
    obj_ptr<MongoDB> db(m_db);
    if (!db)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    return db->runCommand(cmd, retVal);
}

result_t MongoCollection::runCommand(exlib::string cmd,
                                     v8::Local<v8::Object> arg, v8::Local<v8::Object> &retVal)
{

    obj_ptr<MongoDB> db(m_db);
    if (!db)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    bson bbq;

    bson_init(&bbq);
    bson_append_string(&bbq, cmd.c_str(), m_name.c_str());
    if (!appendObject(holder(), &bbq, arg))
    {
        bson_destroy(&bbq);
        return CHECK_ERROR(CALL_E_INVALIDARG);
    }
    bson_finish(&bbq);

    return db->bsonHandler(&bbq, retVal);
}

result_t MongoCollection::runCommand(exlib::string cmd, exlib::string cmd1,
                                     exlib::string arg, v8::Local<v8::Object> &retVal)
{
    obj_ptr<MongoDB> db(m_db);
    if (!db)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    bson bbq;

    bson_init(&bbq);
    bson_append_string(&bbq, cmd.c_str(), m_name.c_str());

    if (!cmd1.empty())
        bson_append_string(&bbq, cmd1.c_str(), arg.c_str());

    bson_finish(&bbq);

    return db->bsonHandler(&bbq, retVal);
}

result_t MongoCollection::drop()
{
    obj_ptr<MongoDB> db(m_db);
    if (!db)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    v8::Local<v8::Object> r;
    return db->runCommand("drop", holder()->NewFromUtf8(m_name), r);
}

result_t MongoCollection::ensureIndex(v8::Local<v8::Object> keys,
                                      v8::Local<v8::Object> options)
{
    obj_ptr<MongoDB> db(m_db);
    if (!db)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    exlib::string name;

    v8::Local<v8::Array> ks = keys->GetPropertyNames();
    int32_t len = (int32_t) ks->Length();
    int32_t i;

    for (i = 0; i < len; i++)
    {
        v8::Local<v8::Value> k = ks->Get(i);
        v8::Local<v8::Value> v = keys->Get(k);

        if (!v->IsNumber() && !v->IsNumberObject())
            return CHECK_ERROR(CALL_E_INVALIDARG);

        v8::String::Utf8Value sk(k);
        v8::String::Utf8Value sv(v);

        if (name.length())
            name += '_';

        name.append(*sk, sk.length());
        name += '_';
        name.append(*sv, sv.length());
    }

    Isolate* isolate = holder();
    v8::Local<v8::Object> idx = v8::Object::New(isolate->m_isolate);

    idx->Set(isolate->NewFromUtf8("ns"), isolate->NewFromUtf8(m_ns));
    idx->Set(isolate->NewFromUtf8("key"), keys);

    idx->Set(isolate->NewFromUtf8("name"), isolate->NewFromUtf8(name));

    extend(idx, options);

    result_t hr;
    obj_ptr<MongoCollection_base> coll;

    hr = db->getCollection("system.indexes", coll);
    if (hr < 0)
        return hr;

    return coll->insert(idx);
}

result_t MongoCollection::reIndex(v8::Local<v8::Object> &retVal)
{
    return runCommand("reIndex", "", "", retVal);
}

result_t MongoCollection::dropIndex(exlib::string name,
                                    v8::Local<v8::Object> &retVal)
{
    return runCommand("deleteIndexes", "index", name, retVal);
}

result_t MongoCollection::dropIndexes(v8::Local<v8::Object> &retVal)
{
    return runCommand("deleteIndexes", "index", "*", retVal);
}

result_t MongoCollection::getIndexes(obj_ptr<MongoCursor_base> &retVal)
{
    obj_ptr<MongoDB> db(m_db);
    if (!db)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    result_t hr;
    obj_ptr<MongoCollection_base> coll;

    hr = db->getCollection("system.indexes", coll);
    if (hr < 0)
        return hr;

    Isolate* isolate = holder();
    v8::Local<v8::Object> f = v8::Object::New(isolate->m_isolate);
    v8::Local<v8::Object> q = v8::Object::New(isolate->m_isolate);
    q->Set(isolate->NewFromUtf8("ns"), isolate->NewFromUtf8(m_ns));

    return coll->find(q, f, retVal);
}

result_t MongoCollection::getCollection(exlib::string name,
                                        obj_ptr<MongoCollection_base> &retVal)
{
    obj_ptr<MongoDB> db(m_db);
    if (!db)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    exlib::string nsStr(m_ns);
    exlib::string nameStr(m_name);

    nsStr += '.';
    nsStr.append(name);

    nameStr += '.';
    nameStr.append(name);

    retVal = new MongoCollection(db, nsStr, nameStr);

    return 0;
}

result_t MongoCollection::_named_getter(const char* property,
                                        obj_ptr<MongoCollection_base> &retVal)
{
    return getCollection(property, retVal);
}

result_t MongoCollection::_named_enumerator(v8::Local<v8::Array> &retVal)
{
    return 0;
}

} /* namespace fibjs */
