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
    MongoCollection(MongoDB *db, const char *ns, const char *name) :
        m_db(db), m_ns(ns), m_name(name)
    {
    }

public:
    virtual void enter()
    {
        m_db->enter();
    }

    virtual void leave()
    {
        m_db->leave();
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
    virtual result_t runCommand(const char *cmd, v8::Local<v8::Object> arg, v8::Local<v8::Object> &retVal);
    virtual result_t drop();
    virtual result_t ensureIndex(v8::Local<v8::Object> keys, v8::Local<v8::Object> options);
    virtual result_t reIndex(v8::Local<v8::Object> &retVal);
    virtual result_t dropIndex(const char *name, v8::Local<v8::Object> &retVal);
    virtual result_t dropIndexes(v8::Local<v8::Object> &retVal);
    virtual result_t getIndexes(obj_ptr<MongoCursor_base> &retVal);
    virtual result_t getCollection(const char *name, obj_ptr<MongoCollection_base> &retVal);
    virtual result_t _named_getter(const char *property, obj_ptr<MongoCollection_base> &retVal);
    virtual result_t _named_enumerator(v8::Local<v8::Array> &retVal);

private:
    result_t runCommand(const char *cmd, const char *cmd1, const char *arg,
                        v8::Local<v8::Object> &retVal);

private:
    obj_ptr<MongoDB> m_db;
    std::string m_ns;
    std::string m_name;
};

} /* namespace fibjs */
#endif /* MONGOCOLLECTION_H_ */
