/*
 * MongoCursor.cpp
 *
 *  Created on: Oct 31, 2012
 *      Author: lion
 */

#include "MongoCursor.h"
#include "encoding_bson.h"

namespace fibjs
{

MongoCursor::MongoCursor(MongoDB* db, const char* ns,
		v8::Handle<v8::Object> query, v8::Handle<v8::Object> projection)
{
	m_state = CUR_NONE;
	m_db = db;

	mongo_cursor_init(&m_cursor, &db->m_conn, ns);

	bson_init(&m_bbq);
	encodeObject(&m_bbq, query);
	bson_finish(&m_bbq);

	mongo_cursor_set_query(&m_cursor, &m_bbq);

	bson_init(&m_bbp);
	encodeObject(&m_bbp, projection);
	bson_finish(&m_bbp);

	mongo_cursor_set_fields(&m_cursor, &m_bbp);
}

MongoCursor::~MongoCursor()
{
	mongo_cursor_destroy(&m_cursor);
	bson_destroy(&m_bbq);
	bson_destroy(&m_bbp);
}

result_t MongoCursor::batchSize(int32_t size, obj_ptr<MongoCursor_base>& retVal)
{
	return 0;
}

result_t MongoCursor::hint(v8::Handle<v8::Object> opts,
		obj_ptr<MongoCursor_base>& retVal)
{
	return 0;
}

result_t MongoCursor::limit(int32_t size, obj_ptr<MongoCursor_base>& retVal)
{
	mongo_cursor_set_limit(&m_cursor, size);
	retVal = this;
	return 0;
}

result_t MongoCursor::readPref(const char* mode, v8::Handle<v8::Array> tagSet,
		obj_ptr<MongoCursor_base>& retVal)
{
	return 0;
}

result_t MongoCursor::showDiskLoc(obj_ptr<MongoCursor_base>& retVal)
{
	return 0;
}

result_t MongoCursor::count(bool override, int32_t& retVal)
{
	return 0;
}

result_t MongoCursor::explain(v8::Handle<v8::Object>& retVal)
{
	return 0;
}

result_t MongoCursor::forEach(v8::Handle<v8::Function> func)
{
	return 0;
}

result_t MongoCursor::map(v8::Handle<v8::Function> func,
		v8::Handle<v8::Array>& retVal)
{
	return 0;
}

result_t MongoCursor::hasNext(bool& retVal)
{
	if (m_state == CUR_NONE)
		m_state =
				mongo_cursor_next(&m_cursor) == MONGO_OK ?
						CUR_DATA : CUR_NODATA;

	retVal = m_state == CUR_DATA;

	return m_db->error();
}

result_t MongoCursor::next(v8::Handle<v8::Object>& retVal)
{
	bool has;
	hasNext(has);

	if (!has)
		return CALL_RETURN_NULL;

	retVal = decodeObject(mongo_cursor_bson(&m_cursor));
	m_state = CUR_NONE;

	return 0;
}

result_t MongoCursor::size(int32_t& retVal)
{
	return 0;
}

result_t MongoCursor::skip(int32_t num, obj_ptr<MongoCursor_base>& retVal)
{
	mongo_cursor_set_skip(&m_cursor, num);
	retVal = this;

	return 0;
}

result_t MongoCursor::snapshot(obj_ptr<MongoCursor_base>& retVal)
{
	return 0;
}

result_t MongoCursor::sort(v8::Handle<v8::Object> opts,
		obj_ptr<MongoCursor_base>& retVal)
{
	return 0;
}

} /* namespace fibjs */
