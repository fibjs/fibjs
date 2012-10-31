/*
 * MongoCollection.cpp
 *
 *  Created on: Oct 25, 2012
 *      Author: lion
 */

#include "MongoCollection.h"
#include "encoding_bson.h"
#include <vector>

namespace fibjs
{

result_t MongoCollection::find(obj_ptr<MongoCursor_base>& retVal)
{
	return 0;
}

result_t MongoCollection::find(v8::Handle<v8::Object> query,
		obj_ptr<MongoCursor_base>& retVal)
{
	return 0;
}

result_t MongoCollection::find(v8::Handle<v8::Object> query,
		v8::Handle<v8::Object> projection, obj_ptr<MongoCursor_base>& retVal)
{
	return 0;
}

result_t MongoCollection::findOne(v8::Handle<v8::Object> query,
		v8::Handle<v8::Object>& retVal)
{
	return 0;
}

result_t MongoCollection::findAndModify(v8::Handle<v8::Object> query)
{
	return 0;
}

result_t MongoCollection::insert(v8::Handle<v8::Array> documents)
{
	std::vector < bson > bbs;
	std::vector<const bson*> pbbs;
	int n = documents->Length();
	int i;

	if (n > 0)
	{
		bbs.resize(n);
		pbbs.resize(n);

		for (i = 0; i < n; i++)
		{
			pbbs[i] = &bbs[i];
			bson_init (&bbs[i]);
			v8::Handle < v8::Value > v = documents->Get(i);
			encodeObject(&bbs[i], v);
			bson_finish(&bbs[i]);
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

	bson_init(&bb);
	encodeObject(&bb, document);
	bson_finish(&bb);

	int result = mongo_insert(&m_db->m_conn, m_ns.c_str(), &bb, NULL);
	bson_destroy(&bb);

	if (result != MONGO_OK)
		return m_db->error();

	return 0;
}

result_t MongoCollection::save(v8::Handle<v8::Object> document)
{
	return 0;
}

result_t MongoCollection::update(v8::Handle<v8::Object> query,
		v8::Handle<v8::Object> document, bool upsert, bool multi)
{
	return 0;
}

result_t MongoCollection::update(v8::Handle<v8::Object> query,
		v8::Handle<v8::Object> document, v8::Handle<v8::Object> options)
{
	return 0;
}

} /* namespace fibjs */
