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
	MongoCollection(MongoDB* db, const char* ns, const char* name) :
			m_db(db), m_ns(ns), m_name(name)
	{
	}

public:
	// MongoCollection_base
	virtual result_t find(v8::Handle<v8::Object> query, v8::Handle<v8::Object> projection, obj_ptr<MongoCursor_base>& retVal);
	virtual result_t findOne(v8::Handle<v8::Object> query, v8::Handle<v8::Object> projection, v8::Handle<v8::Object>& retVal);
	virtual result_t findAndModify(v8::Handle<v8::Object> query, v8::Handle<v8::Object>& retVal);
	virtual result_t insert(v8::Handle<v8::Array> documents);
	virtual result_t insert(v8::Handle<v8::Object> document);
	virtual result_t save(v8::Handle<v8::Object> document);
	virtual result_t update(v8::Handle<v8::Object> query, v8::Handle<v8::Object> document, bool upsert, bool multi);
	virtual result_t update(v8::Handle<v8::Object> query, v8::Handle<v8::Object> document, v8::Handle<v8::Object> options);
	virtual result_t remove(v8::Handle<v8::Object> query);
	virtual result_t runCommand(v8::Handle<v8::Object> cmd, v8::Handle<v8::Object>& retVal);
	virtual result_t runCommand(const char* cmd, v8::Handle<v8::Object> arg, v8::Handle<v8::Object>& retVal);
	virtual result_t drop();
	virtual result_t ensureIndex(v8::Handle<v8::Object> keys, v8::Handle<v8::Object> options);
	virtual result_t reIndex(v8::Handle<v8::Object>& retVal);
	virtual result_t dropIndex(const char* name, v8::Handle<v8::Object>& retVal);
	virtual result_t dropIndexes(v8::Handle<v8::Object>& retVal);
	virtual result_t getIndexes(v8::Handle<v8::Array>& retVal);
	virtual result_t oid(const char* hexStr, obj_ptr<MongoID_base>& retVal);

private:
	result_t runCommand(const char* cmd, const char* cmd1, const char* arg,
			v8::Handle<v8::Object>& retVal);

private:
	obj_ptr<MongoDB> m_db;
	std::string m_ns;
	std::string m_name;
};

} /* namespace fibjs */
#endif /* MONGOCOLLECTION_H_ */
