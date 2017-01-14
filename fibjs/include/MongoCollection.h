/*
 * MongoCollection.h
 *
 *  Created on: Oct 25, 2012
 *      Author: lion
 */

#include "ifs/MongoCollection.h"
#include "MongoDB.h"

#ifndef MONGOCOLLECTION_H_
#define MONGOCOLLECTION_H_

namespace fibjs
{

class MongoCollection: public MongoCollection_base
{
public:
    MongoCollection(MongoDB *db, exlib::string ns, exlib::string name) :
        m_db(db), m_ns(ns), m_name(name)
    {
    }

public:
    virtual bool enterTask(exlib::Task_base *current)
    {
        return m_db->enterTask(current);
    }

    virtual void enter()
    {
        m_db->enter();
    }

    virtual void leave(exlib::Task_base *current = NULL)
    {
        m_db->leave(current);
    }

public:
    // MongoCollection_base
    virtual result_t find(v8::Local<v8::Object> query, v8::Local<v8::Object> projection, obj_ptr<MongoCursor_base> &retVal);
    virtual result_t findOne(v8::Local<v8::Object> query, v8::Local<v8::Object> projection, v8::Local<v8::Object> &retVal);
    virtual result_t findAndModify(v8::Local<v8::Object> query, v8::Local<v8::Object> &retVal);
    virtual result_t insert(v8::Local<v8::Array> documents);
    virtual result_t insert(v8::Local<v8::Object> document);
    virtual result_t save(v8::Local<v8::Object> document);
    virtual result_t update(v8::Local<v8::Object> query, v8::Local<v8::Object> document, bool upsert, bool multi);
    virtual result_t update(v8::Local<v8::Object> query, v8::Local<v8::Object> document, v8::Local<v8::Object> options);
    virtual result_t remove(v8::Local<v8::Object> query);
    virtual result_t runCommand(v8::Local<v8::Object> cmd, v8::Local<v8::Object> &retVal);
    virtual result_t runCommand(exlib::string cmd, v8::Local<v8::Object> arg, v8::Local<v8::Object> &retVal);
    virtual result_t drop();
    virtual result_t ensureIndex(v8::Local<v8::Object> keys, v8::Local<v8::Object> options);
    virtual result_t reIndex(v8::Local<v8::Object> &retVal);
    virtual result_t dropIndex(exlib::string name, v8::Local<v8::Object> &retVal);
    virtual result_t dropIndexes(v8::Local<v8::Object> &retVal);
    virtual result_t getIndexes(obj_ptr<MongoCursor_base> &retVal);
    virtual result_t getCollection(exlib::string name, obj_ptr<MongoCollection_base> &retVal);
    virtual result_t _named_getter(const char* property, obj_ptr<MongoCollection_base> &retVal);
    virtual result_t _named_enumerator(v8::Local<v8::Array> &retVal);

private:
    result_t runCommand(exlib::string cmd, exlib::string cmd1, exlib::string arg,
                        v8::Local<v8::Object> &retVal);

    result_t _insert(const bson *data, int32_t &retVal, AsyncEvent *ac);
    ASYNC_MEMBERVALUE2_AC(MongoCollection, _insert, bson *, int32_t);

    result_t _batchInsert(const std::vector<const bson *> pdata, int num, int32_t &retVal, AsyncEvent *ac);
    ASYNC_MEMBERVALUE3_AC(MongoCollection, _batchInsert, std::vector<const bson *>, int, int32_t);

    result_t _update(const bson *cond, const bson *op, int flags, int32_t &retVal, AsyncEvent *ac);
    ASYNC_MEMBERVALUE4_AC(MongoCollection, _update, bson *, bson *, int, int32_t);

    result_t _remove(const bson *data, int32_t &retVal, AsyncEvent *ac);
    ASYNC_MEMBERVALUE2_AC(MongoCollection, _remove, bson *, int32_t);

private:
    weak_ptr<MongoDB> m_db;
    exlib::string m_ns;
    exlib::string m_name;
};

} /* namespace fibjs */
#endif /* MONGOCOLLECTION_H_ */
