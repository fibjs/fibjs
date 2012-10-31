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
	MongoCollection(MongoDB* db, const char* ns) :
			m_db(db), m_ns(ns)
	{
	}

public:
	// MongoCollection_base
	virtual result_t find(obj_ptr<MongoCursor_base>& retVal);
	virtual result_t find(v8::Handle<v8::Object> query, obj_ptr<MongoCursor_base>& retVal);
	virtual result_t find(v8::Handle<v8::Object> query, v8::Handle<v8::Object> projection, obj_ptr<MongoCursor_base>& retVal);
	virtual result_t findOne(v8::Handle<v8::Object> query, v8::Handle<v8::Object>& retVal);
	virtual result_t findAndModify(v8::Handle<v8::Object> query);
	virtual result_t insert(v8::Handle<v8::Array> documents);
	virtual result_t insert(v8::Handle<v8::Object> document);
	virtual result_t save(v8::Handle<v8::Object> document);
	virtual result_t update(v8::Handle<v8::Object> query, v8::Handle<v8::Object> document, bool upsert, bool multi);
	virtual result_t update(v8::Handle<v8::Object> query, v8::Handle<v8::Object> document, v8::Handle<v8::Object> options);

private:
	obj_ptr<MongoDB> m_db;
	std::string m_ns;
};

} /* namespace fibjs */
#endif /* MONGOCOLLECTION_H_ */
