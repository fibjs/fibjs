/*
 * MongoCollection.cpp
 *
 *  Created on: Oct 25, 2012
 *      Author: lion
 */

#include "MongoCollection.h"
#include "MongoCursor.h"
#include "encoding_bson.h"
#include <vector>

namespace fibjs
{

result_t MongoCollection::find(v8::Handle<v8::Object> query,
                               v8::Handle<v8::Object> projection, obj_ptr<MongoCursor_base> &retVal)
{
    retVal = new MongoCursor(m_db, m_ns, m_name, query, projection);
    return 0;
}

result_t MongoCollection::findOne(v8::Handle<v8::Object> query,
                                  v8::Handle<v8::Object> projection, v8::Handle<v8::Object> &retVal)
{
    obj_ptr < MongoCursor > cur = new MongoCursor(m_db, m_ns, m_name, query,
            projection);
    obj_ptr < MongoCursor_base > cur1;

    cur->limit(1, cur1);
    return cur->next(retVal);
}

result_t MongoCollection::findAndModify(v8::Handle<v8::Object> query,
                                        v8::Handle<v8::Object> &retVal)
{
    return runCommand("findAndModify", query, retVal);
}

result_t MongoCollection::insert(v8::Handle<v8::Array> documents)
{
    std::vector < bson > bbs;
    std::vector<const bson *> pbbs;
    int n = documents->Length();
    int i;
    result_t hr;

    if (n > 0)
    {
        bbs.resize(n);
        pbbs.resize(n);

        for (i = 0; i < n; i++)
        {
            hr = encodeObject(&bbs[i], documents->Get(i));
            if (hr < 0)
            {
                n = i;
                for (i = 0; i < n; i++)
                    bson_destroy (&bbs[i]);
                return hr;
            }

            pbbs[i] = &bbs[i];
        }

        int result = mongo_insert_batch(&m_db->m_conn, m_ns.c_str(),
                                        pbbs.data(), n, NULL, 0);

        for (i = 0; i < n; i++)
            bson_destroy (&bbs[i]);

        if (result != MONGO_OK)
            return m_db->error();
    }

    return 0;
}

result_t MongoCollection::insert(v8::Handle<v8::Object> document)
{
    bson bb;
    result_t hr;

    hr = encodeObject(&bb, document);
    if (hr < 0)
        return hr;

    int result = mongo_insert(&m_db->m_conn, m_ns.c_str(), &bb, NULL);
    bson_destroy(&bb);

    if (result != MONGO_OK)
        return m_db->error();

    return 0;
}

result_t MongoCollection::save(v8::Handle<v8::Object> document)
{
    v8::Handle < v8::String > strId = v8::String::NewSymbol("_id", 3);
    v8::Handle < v8::Value > id = document->Get(strId);

    if (IsEmpty (id))
        return insert(document);
    else
    {
        v8::Handle < v8::Object > query = v8::Object::New();

        query->Set(strId, id);
        return update(query, document, true, false);
    }
}

result_t MongoCollection::update(v8::Handle<v8::Object> query,
                                 v8::Handle<v8::Object> document, bool upsert, bool multi)
{
    bson bbq, bbd;
    int flags = (upsert ? MONGO_UPDATE_UPSERT : 0)
                + (multi ? MONGO_UPDATE_MULTI : 0);

    result_t hr;

    hr = encodeObject(&bbq, query);
    if (hr < 0)
        return hr;

    hr = encodeObject(&bbd, document);
    if (hr < 0)
    {
        bson_destroy(&bbq);
        return hr;
    }

    int result = mongo_update(&m_db->m_conn, m_ns.c_str(), &bbq, &bbd, flags,
                              NULL);

    bson_destroy(&bbq);
    bson_destroy(&bbd);

    if (result != MONGO_OK)
        return m_db->error();

    return 0;
}

result_t MongoCollection::update(v8::Handle<v8::Object> query,
                                 v8::Handle<v8::Object> document, v8::Handle<v8::Object> options)
{
    return update(query, document,
                  options->Get(v8::String::NewSymbol("upsert", 6))->BooleanValue(),
                  options->Get(v8::String::NewSymbol("multi", 5))->BooleanValue());
}

result_t MongoCollection::remove(v8::Handle<v8::Object> query)
{
    bson bbq;
    result_t hr;

    hr = encodeObject(&bbq, query);
    if (hr < 0)
        return hr;

    int result = mongo_remove(&m_db->m_conn, m_ns.c_str(), &bbq, NULL);

    bson_destroy(&bbq);

    if (result != MONGO_OK)
        return m_db->error();

    return 0;
}

result_t MongoCollection::runCommand(v8::Handle<v8::Object> cmd,
                                     v8::Handle<v8::Object> &retVal)
{
    return m_db->runCommand(cmd, retVal);
}

result_t MongoCollection::runCommand(const char *cmd,
                                     v8::Handle<v8::Object> arg, v8::Handle<v8::Object> &retVal)
{
    bson bbq;

    bson_init(&bbq);
    bson_append_string(&bbq, cmd, m_name.c_str());
    if (!appendObject(&bbq, arg))
    {
        bson_destroy(&bbq);
        return CALL_E_INVALIDARG;
    }
    bson_finish(&bbq);

    return m_db->run_command(&bbq, retVal);
}

result_t MongoCollection::runCommand(const char *cmd, const char *cmd1,
                                     const char *arg, v8::Handle<v8::Object> &retVal)
{
    bson bbq;

    bson_init(&bbq);
    bson_append_string(&bbq, cmd, m_name.c_str());

    if (cmd1)
        bson_append_string(&bbq, cmd1, arg);

    bson_finish(&bbq);

    return m_db->run_command(&bbq, retVal);
}

result_t MongoCollection::drop()
{
    v8::Handle < v8::Object > r;
    return m_db->runCommand("drop",
                            v8::String::New(m_name.c_str(), (int) m_name.length()), r);
}

result_t MongoCollection::ensureIndex(v8::Handle<v8::Object> keys,
                                      v8::Handle<v8::Object> options)
{
    std::string name;

    v8::Handle < v8::Array > ks = keys->GetPropertyNames();
    int len = (int) ks->Length();
    int i;

    for (i = 0; i < len; i++)
    {
        v8::Handle < v8::Value > k = ks->Get(i);
        v8::Handle < v8::Value > v = keys->Get(k);

        if (!v->IsNumber())
            return CALL_E_INVALIDARG;

        v8::String::Utf8Value sk(k);
        v8::String::Utf8Value sv(v);

        if (name.length())
            name += '_';

        name.append(*sk, sk.length());
        name += '_';
        name.append(*sv, sv.length());
    }

    v8::Handle < v8::Object > idx = v8::Object::New();

    idx->Set(v8::String::New("ns"),
             v8::String::New(m_ns.c_str(), (int) m_ns.length()));
    idx->Set(v8::String::New("key"), keys);

    idx->Set(v8::String::New("name"),
             v8::String::New(name.c_str(), (int) name.length()));

    extend(idx, options);

    result_t hr;
    obj_ptr<MongoCollection_base> coll;

    hr = m_db->getCollection("system.indexes", coll);
    if (hr < 0)
        return hr;

    return coll->insert(idx);
}

result_t MongoCollection::reIndex(v8::Handle<v8::Object> &retVal)
{
    return runCommand("reIndex", NULL, NULL, retVal);
}

result_t MongoCollection::dropIndex(const char *name,
                                    v8::Handle<v8::Object> &retVal)
{
    return runCommand("deleteIndexes", "index", name, retVal);
}

result_t MongoCollection::dropIndexes(v8::Handle<v8::Object> &retVal)
{
    return runCommand("deleteIndexes", "index", "*", retVal);
}

result_t MongoCollection::getIndexes(obj_ptr<MongoCursor_base> &retVal)
{
    result_t hr;
    obj_ptr<MongoCollection_base> coll;

    hr = m_db->getCollection("system.indexes", coll);
    if (hr < 0)
        return hr;

    v8::Handle < v8::Object > f = v8::Object::New();
    v8::Handle < v8::Object > q = v8::Object::New();
    q->Set(v8::String::New("ns"),
           v8::String::New(m_ns.c_str(), (int) m_ns.length()));

    return coll->find(q, f, retVal);
}

result_t MongoCollection::getCollection(const char *name,
                                        obj_ptr<MongoCollection_base> &retVal)
{
    std::string nsStr(m_ns);
    std::string nameStr(m_name);

    nsStr += '.';
    nsStr.append(name);

    nameStr += '.';
    nameStr.append(name);

    retVal = new MongoCollection(m_db, nsStr.c_str(), nameStr.c_str());

    return 0;
}

result_t MongoCollection::_named_getter(const char *property,
                                        obj_ptr<MongoCollection_base> &retVal)
{
    return getCollection(property, retVal);
}

result_t MongoCollection::_named_enumerator(v8::Handle<v8::Array> &retVal)
{
    return 0;
}

} /* namespace fibjs */
