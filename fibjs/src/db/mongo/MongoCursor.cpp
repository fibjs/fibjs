/*
 * MongoCursor.cpp
 *
 *  Created on: Oct 31, 2012
 *      Author: lion
 */

#include "MongoCursor.h"
#include "encoding_bson.h"
#include "ifs/util.h"
#include "Fiber.h"

namespace fibjs
{

result_t MongoCursor::_initCursor(MongoDB *db, AsyncEvent *ac)
{
    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    m_cursor = new cursor;
    m_cursor->m_db = db;

    mongo_cursor_init(m_cursor, &(m_cursor->m_db)->m_conn, m_ns.c_str());
    mongo_cursor_set_query(m_cursor, &m_bbq);
    mongo_cursor_set_fields(m_cursor, &m_bbp);
    return 0;
}

result_t MongoCursor::_nextCursor(int32_t &retVal, AsyncEvent *ac)
{
    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    retVal = mongo_cursor_next(m_cursor);
    return 0;
}

result_t MongoCursor::_bsonCursor(bson &out, AsyncEvent *ac)
{
    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    out = *(mongo_cursor_bson(m_cursor));
    return 0;
}

result_t MongoCursor::_limitCursor(int32_t size, AsyncEvent *ac)
{
    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    mongo_cursor_set_limit(m_cursor, size);
    return 0;
}

result_t MongoCursor::_skipCursor(int32_t num, AsyncEvent *ac)
{
    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    mongo_cursor_set_skip(m_cursor, num);
    return 0;
}

MongoCursor::MongoCursor(MongoDB *db, const std::string &ns,
                         const std::string &name, v8::Local<v8::Object> query,
                         v8::Local<v8::Object> projection)
{
    m_state = CUR_NONE;

    m_ns = ns;
    m_name = name;

    v8::Local<v8::Value> _query;
    util_base::clone(query, _query);
    m_query.Reset(Isolate::now()->m_isolate, v8::Local<v8::Object>::Cast(_query)->Clone());

    encodeObject(&m_bbp, projection);

    cc__initCursor(db);

    m_bInit = false;
    m_bSpecial = false;
}

static void _close(MongoCursor::cursor *cur)
{
    mongo_cursor_destroy(cur);
    delete cur;
}

MongoCursor::~MongoCursor()
{
    m_query.Reset();

    asyncCall(_close, m_cursor);

    if (m_bInit)
        bson_destroy(&m_bbq);
    bson_destroy(&m_bbp);
}

void MongoCursor::ensureSpecial()
{
    if (!m_bSpecial)
    {
        Isolate* isolate = Isolate::now();
        v8::Local<v8::Object> o = v8::Object::New(isolate->m_isolate);

        o->Set(v8::String::NewFromUtf8(isolate->m_isolate, "query"),
               v8::Local<v8::Object>::New(isolate->m_isolate, m_query));
        m_query.Reset();

        m_query.Reset(isolate->m_isolate, o);
        m_bSpecial = true;
    }
}

result_t MongoCursor::hint(v8::Local<v8::Object> opts,
                           obj_ptr<MongoCursor_base> &retVal)
{
    return _addSpecial("$hint", opts, retVal);
}

result_t MongoCursor::limit(int32_t size, obj_ptr<MongoCursor_base> &retVal)
{
    if (m_bInit)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    cc__limitCursor(size);
    retVal = this;
    return 0;
}

result_t MongoCursor::count(bool applySkipLimit, int32_t &retVal)
{
    bson bbq;

    bson_init(&bbq);
    bson_append_string(&bbq, "count", m_name.c_str());

    Isolate* isolate = Isolate::now();
    if (m_bSpecial)
        encodeValue(&bbq, "query",
                    v8::Local<v8::Object>::New(isolate->m_isolate, m_query)->Get(v8::String::NewFromUtf8(isolate->m_isolate, "query")));
    else
        encodeValue(&bbq, "query", v8::Local<v8::Object>::New(isolate->m_isolate, m_query));

    if (applySkipLimit)
    {
        if (m_cursor->limit)
            bson_append_int(&bbq, "limit", m_cursor->limit);
        if (m_cursor->skip)
            bson_append_int(&bbq, "skip", m_cursor->skip);
    }

    bson_finish(&bbq);

    v8::Local<v8::Object> res;

    result_t hr = m_cursor->m_db->bsonHandler(&bbq, res);
    if (hr < 0)
        return hr;

    retVal = res->Get(v8::String::NewFromUtf8(isolate->m_isolate, "n"))->Int32Value();

    return 0;
}

result_t MongoCursor::forEach(v8::Local<v8::Function> func)
{
    result_t hr;
    v8::Local<v8::Object> o;
    Isolate* isolate = Isolate::now();

    while ((hr = next(o)) != CALL_RETURN_NULL)
    {
        v8::Local<v8::Value> a = o;
        v8::Local<v8::Value> v = func->Call(v8::Undefined(isolate->m_isolate), 1, &a);

        if (v.IsEmpty())
            return CALL_E_JAVASCRIPT;
    }

    return hr < 0 ? hr : 0;
}

result_t MongoCursor::map(v8::Local<v8::Function> func,
                          v8::Local<v8::Array> &retVal)
{
    result_t hr;
    Isolate* isolate = Isolate::now();
    v8::Local<v8::Object> o;
    v8::Local<v8::Array> as = v8::Array::New(isolate->m_isolate);
    int32_t n = 0;

    while ((hr = next(o)) != CALL_RETURN_NULL)
    {
        v8::Local<v8::Value> a = o;
        v8::Local<v8::Value> v = func->Call(v8::Undefined(isolate->m_isolate), 1, &a);

        if (v.IsEmpty())
            return CALL_E_JAVASCRIPT;

        as->Set(n, v);
        n++;
    }

    retVal = as;
    return hr < 0 ? hr : 0;
}

result_t MongoCursor::hasNext(bool &retVal)
{
    if (!m_bInit)
    {
        result_t hr;

        hr = encodeObject(&m_bbq, v8::Local<v8::Object>::New(Isolate::now()->m_isolate, m_query));
        if (hr < 0)
            return hr;

        m_bInit = true;
    }

    if (m_state == CUR_NONE)
    {
        int32_t res;
        cc__nextCursor(res);
        m_state = (res == MONGO_OK) ? CUR_DATA : CUR_NODATA;
    }

    retVal = m_state == CUR_DATA;

    return CHECK_ERROR(m_cursor->m_db->error());
}

result_t MongoCursor::next(v8::Local<v8::Object> &retVal)
{
    bool has;
    result_t hr = hasNext(has);

    if (hr < 0)
        return hr;

    if (!has)
        return CALL_RETURN_NULL;

    retVal = decodeObject(mongo_cursor_bson(m_cursor));
    m_state = CUR_NONE;

    return 0;
}

result_t MongoCursor::size(int32_t &retVal)
{
    return count(true, retVal);
}

result_t MongoCursor::skip(int32_t num, obj_ptr<MongoCursor_base> &retVal)
{
    if (m_bInit)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    cc__skipCursor(num);
    retVal = this;

    return 0;
}

result_t MongoCursor::sort(v8::Local<v8::Object> opts,
                           obj_ptr<MongoCursor_base> &retVal)
{
    return _addSpecial("orderby", opts, retVal);
}

result_t MongoCursor::_addSpecial(const char *name, v8::Local<v8::Value> opts,
                                  obj_ptr<MongoCursor_base> &retVal)
{
    if (m_bInit)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    ensureSpecial();
    Isolate* isolate = Isolate::now();
    v8::Local<v8::Object>::New(isolate->m_isolate, m_query)->Set(v8::String::NewFromUtf8(isolate->m_isolate, name), opts);

    retVal = this;
    return 0;
}

result_t MongoCursor::toArray(v8::Local<v8::Array> &retVal)
{
    result_t hr;
    v8::Local<v8::Object> o;
    v8::Local<v8::Array> as = v8::Array::New(Isolate::now()->m_isolate);
    int32_t n = 0;

    while ((hr = next(o)) != CALL_RETURN_NULL)
    {
        as->Set(n, o);
        n++;
    }

    if (hr < 0)
        return hr;

    retVal = as;
    return 0;
}

result_t MongoCursor::toJSON(const char *key, v8::Local<v8::Value> &retVal)
{
    result_t hr;
    v8::Local<v8::Array> as;

    hr = toArray(as);
    if (hr < 0)
        return hr;

    retVal = as;
    return 0;
}

} /* namespace fibjs */
