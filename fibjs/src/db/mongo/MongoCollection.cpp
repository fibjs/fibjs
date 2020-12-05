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

namespace fibjs {

result_t MongoCollection::_batchInsert(std::vector<const bson*> pdata, int num, int32_t& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    retVal = mongo_insert_batch(m_db->m_conn, m_ns.c_str(), pdata.data(), num, NULL, 0);
    return 0;
}

result_t MongoCollection::_insert(const bson* data, int32_t& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    retVal = mongo_insert(m_db->m_conn, m_ns.c_str(), data, NULL);
    return 0;
}

result_t MongoCollection::_update(const bson* cond, const bson* op, int flags, int32_t& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    retVal = mongo_update(m_db->m_conn, m_ns.c_str(), cond, op, flags, NULL);
    return 0;
}

result_t MongoCollection::_remove(const bson* data, int32_t& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    retVal = mongo_remove(m_db->m_conn, m_ns.c_str(), data, NULL);
    return 0;
}

result_t MongoCollection::find(v8::Local<v8::Object> query,
    v8::Local<v8::Object> projection, obj_ptr<MongoCursor_base>& retVal)
{
    retVal = new MongoCursor(m_db, m_ns, m_name, query, projection);
    return 0;
}

result_t MongoCollection::findOne(v8::Local<v8::Object> query,
    v8::Local<v8::Object> projection, v8::Local<v8::Object>& retVal)
{
    obj_ptr<MongoCursor> cur = new MongoCursor(m_db, m_ns, m_name, query,
        projection);
    obj_ptr<MongoCursor_base> cur1;

    cur->ac_limit(1, cur1);
    return cur->next(retVal);
}

result_t MongoCollection::findAndModify(v8::Local<v8::Object> query,
    v8::Local<v8::Object>& retVal)
{
    return runCommand("findAndModify", query, retVal);
}

result_t MongoCollection::insert(v8::Local<v8::Array> documents)
{
    std::vector<bson> bbs;
    std::vector<const bson*> pbbs;
    int32_t n = documents->Length();
    int32_t i;
    result_t hr;

    if (n > 0) {
        Isolate* isolate = holder();

        bbs.resize(n);
        pbbs.resize(n);

        for (i = 0; i < n; i++) {
            hr = encodeObject(isolate, &bbs[i], JSValue(documents->Get(i)));
            if (hr < 0) {
                n = i;
                for (i = 0; i < n; i++)
                    bson_destroy(&bbs[i]);
                return hr;
            }

            pbbs[i] = &bbs[i];
        }

        int32_t result = MONGO_ERROR;
        ac__batchInsert(pbbs, n, result);

        for (i = 0; i < n; i++)
            bson_destroy(&bbs[i]);

        if (result != MONGO_OK)
            return CHECK_ERROR(m_db->error());
    }

    return 0;
}

result_t MongoCollection::insert(v8::Local<v8::Object> document)
{
    bson bb;
    result_t hr;

    hr = encodeObject(holder(), &bb, document);
    if (hr < 0)
        return hr;

    int32_t result = MONGO_ERROR;
    ac__insert(&bb, result);

    bson_destroy(&bb);

    if (result != MONGO_OK)
        return CHECK_ERROR(m_db->error());

    return 0;
}

result_t MongoCollection::save(v8::Local<v8::Object> document)
{
    Isolate* isolate = holder();

    v8::Local<v8::String> strId = isolate->NewString("_id", 3);
    JSValue id = document->Get(strId);

    if (IsEmpty(id))
        return insert(document);
    else {
        v8::Local<v8::Object> query = v8::Object::New(isolate->m_isolate);

        query->Set(strId, id);
        return update(query, document, true, false);
    }
}

result_t MongoCollection::update(v8::Local<v8::Object> query,
    v8::Local<v8::Object> document, bool upsert, bool multi)
{
    bson bbq, bbd;
    int32_t flags = (upsert ? MONGO_UPDATE_UPSERT : 0)
        + (multi ? MONGO_UPDATE_MULTI : 0);

    Isolate* isolate = holder();
    result_t hr;

    hr = encodeObject(isolate, &bbq, query);
    if (hr < 0)
        return hr;

    hr = encodeObject(isolate, &bbd, document);
    if (hr < 0) {
        bson_destroy(&bbq);
        return hr;
    }

    int32_t result = MONGO_ERROR;
    ac__update(&bbq, &bbd, flags, result);

    bson_destroy(&bbq);
    bson_destroy(&bbd);

    if (result != MONGO_OK)
        return CHECK_ERROR(m_db->error());

    return 0;
}

result_t MongoCollection::update(v8::Local<v8::Object> query,
    v8::Local<v8::Object> document, v8::Local<v8::Object> options)
{
    Isolate* isolate = holder();
    return update(query, document,
        JSValue(options->Get(isolate->NewString("upsert", 6)))->BooleanValue(),
        JSValue(options->Get(isolate->NewString("multi", 5)))->BooleanValue());
}

result_t MongoCollection::remove(v8::Local<v8::Object> query)
{
    bson bbq;
    result_t hr;

    hr = encodeObject(holder(), &bbq, query);
    if (hr < 0)
        return hr;

    int32_t result = MONGO_ERROR;
    ac__remove(&bbq, result);

    bson_destroy(&bbq);

    if (result != MONGO_OK)
        return CHECK_ERROR(m_db->error());

    return 0;
}

result_t MongoCollection::runCommand(v8::Local<v8::Object> cmd,
    v8::Local<v8::Object>& retVal)
{
    return m_db->runCommand(cmd, retVal);
}

result_t MongoCollection::runCommand(exlib::string cmd,
    v8::Local<v8::Object> arg, v8::Local<v8::Object>& retVal)
{
    bson bbq;

    bson_init(&bbq);
    bson_append_string(&bbq, cmd.c_str(), m_name.c_str());
    if (!appendObject(holder(), &bbq, arg)) {
        bson_destroy(&bbq);
        return CHECK_ERROR(CALL_E_INVALIDARG);
    }
    bson_finish(&bbq);

    return m_db->bsonHandler(&bbq, retVal);
}

result_t MongoCollection::runCommand(exlib::string cmd, exlib::string cmd1,
    exlib::string arg, v8::Local<v8::Object>& retVal)
{
    bson bbq;

    bson_init(&bbq);
    bson_append_string(&bbq, cmd.c_str(), m_name.c_str());

    if (!cmd1.empty())
        bson_append_string(&bbq, cmd1.c_str(), arg.c_str());

    bson_finish(&bbq);

    return m_db->bsonHandler(&bbq, retVal);
}

result_t MongoCollection::drop()
{
    v8::Local<v8::Object> r;
    return m_db->runCommand("drop", holder()->NewString(m_name), r);
}

result_t MongoCollection::ensureIndex(v8::Local<v8::Object> keys,
    v8::Local<v8::Object> options)
{
    exlib::string name;

    JSArray ks = keys->GetPropertyNames();
    int32_t len = (int32_t)ks->Length();
    int32_t i;
    Isolate* isolate = holder();

    for (i = 0; i < len; i++) {
        JSValue k = ks->Get(i);
        JSValue v = keys->Get(k);

        if (!v->IsNumber() && !v->IsNumberObject())
            return CHECK_ERROR(CALL_E_INVALIDARG);

        v8::String::Utf8Value sk(isolate->m_isolate, k);
        v8::String::Utf8Value sv(isolate->m_isolate, v);

        if (name.length())
            name += '_';

        name.append(*sk, sk.length());
        name += '_';
        name.append(*sv, sv.length());
    }

    v8::Local<v8::Object> idx = v8::Object::New(isolate->m_isolate);

    idx->Set(isolate->NewString("name"), isolate->NewString(name));
    idx->Set(isolate->NewString("key"), keys);
    extend(options, idx);

    v8::Local<v8::Array> idxs = v8::Array::New(isolate->m_isolate);
    idxs->Set(0, idx);

    v8::Local<v8::Object> cmd = v8::Object::New(isolate->m_isolate);
    cmd->Set(isolate->NewString("indexes"), idxs);

    v8::Local<v8::Object> retVal;
    return runCommand("createIndexes", cmd, retVal);
}

result_t MongoCollection::reIndex(v8::Local<v8::Object>& retVal)
{
    return runCommand("reIndex", "", "", retVal);
}

result_t MongoCollection::dropIndex(exlib::string name,
    v8::Local<v8::Object>& retVal)
{
    return runCommand("dropIndexes", "index", name, retVal);
}

result_t MongoCollection::dropIndexes(v8::Local<v8::Object>& retVal)
{
    return runCommand("dropIndexes", "index", "*", retVal);
}

result_t MongoCollection::getIndexes(v8::Local<v8::Array>& retVal)
{
    Isolate* isolate = holder();
    v8::Local<v8::Object> r, cursor;

    result_t hr = m_db->runCommand("listIndexes", isolate->NewString(m_name), r);
    if (hr < 0)
        return hr;

    hr = GetConfigValue(isolate->m_isolate, r, "cursor", cursor, true);
    if (hr < 0)
        return hr;

    return GetConfigValue(isolate->m_isolate, cursor, "firstBatch", retVal, true);
}

result_t MongoCollection::getCollection(exlib::string name,
    obj_ptr<MongoCollection_base>& retVal)
{
    exlib::string nsStr(m_ns);
    exlib::string nameStr(m_name);

    nsStr += '.';
    nsStr.append(name);

    nameStr += '.';
    nameStr.append(name);

    retVal = new MongoCollection(m_db, nsStr, nameStr);

    return 0;
}

result_t MongoCollection::_named_getter(exlib::string property,
    obj_ptr<MongoCollection_base>& retVal)
{
    return getCollection(property, retVal);
}

result_t MongoCollection::_named_enumerator(v8::Local<v8::Array>& retVal)
{
    return 0;
}

} /* namespace fibjs */
