/*
 * MongoCollection.cpp
 *
 *  Created on: Oct 25, 2012
 *      Author: lion
 */

#include "MongoCollection.h"
#include "MongoID.h"
#include "encoding_bson.h"
#include <vector>

namespace fibjs
{

result_t MongoCollection::find(v8::Handle<v8::Object> query,
		v8::Handle<v8::Object> projection, obj_ptr<MongoCursor_base>& retVal)
{
	return 0;
}

result_t MongoCollection::findOne(v8::Handle<v8::Object> query,
		v8::Handle<v8::Object> projection, v8::Handle<v8::Object>& retVal)
{
	bson bbq, bbp, bbo;

	bson_init(&bbq);
	encodeObject(&bbq, query);
	bson_finish(&bbq);

	bson_init(&bbp);
	encodeObject(&bbp, projection);
	bson_finish(&bbp);

	bson_init(&bbo);

	int result = mongo_find_one(&m_db->m_conn, m_ns.c_str(), &bbq, &bbp, &bbo);
	bson_destroy(&bbq);
	bson_destroy(&bbp);

	if (result != MONGO_OK)
		return m_db->error();

	retVal = decodeObject(&bbo);

	bson_destroy(&bbo);

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

	bson_init(&bbq);
	encodeObject(&bbq, query);
	bson_finish(&bbq);

	bson_init(&bbd);
	encodeObject(&bbd, document);
	bson_finish(&bbd);

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

result_t MongoCollection::oid(const char* hexStr, obj_ptr<MongoID_base>& retVal)
{
	retVal = new MongoID(hexStr);
	return 0;
}

} /* namespace fibjs */
