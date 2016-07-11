/*
 * MongoCursor.h
 *
 *  Created on: Oct 31, 2012
 *      Author: lion
 */

#include "ifs/MongoCursor.h"
#include "MongoDB.h"

#ifndef MONGOCURSOR_H_
#define MONGOCURSOR_H_

namespace fibjs
{

class MongoCursor: public MongoCursor_base
{
public:
    MongoCursor(MongoDB *db, const qstring &ns, const qstring &name,
                v8::Local<v8::Object> query, v8::Local<v8::Object> projection);
    ~MongoCursor();

public:
    virtual void enter()
    {
        m_cursor->m_db->enter();
    }

    virtual void leave()
    {
        m_cursor->m_db->leave();
    }

public:
    // object_base
    virtual result_t toJSON(const char *key, v8::Local<v8::Value> &retVal);

public:
    // MongoCursor_base
    virtual result_t skip(int32_t num, obj_ptr<MongoCursor_base> &retVal);
    virtual result_t limit(int32_t size, obj_ptr<MongoCursor_base> &retVal);
    virtual result_t sort(v8::Local<v8::Object> opts, obj_ptr<MongoCursor_base> &retVal);
    virtual result_t hasNext(bool &retVal);
    virtual result_t next(v8::Local<v8::Object> &retVal);
    virtual result_t count(bool applySkipLimit, int32_t &retVal);
    virtual result_t size(int32_t &retVal);
    virtual result_t forEach(v8::Local<v8::Function> func);
    virtual result_t map(v8::Local<v8::Function> func, v8::Local<v8::Array> &retVal);
    virtual result_t toArray(v8::Local<v8::Array> &retVal);
    virtual result_t hint(v8::Local<v8::Object> opts, obj_ptr<MongoCursor_base> &retVal);

private:
    void ensureSpecial();
    result_t _addSpecial(const char *name, v8::Local<v8::Value> opts,
                         obj_ptr<MongoCursor_base> &retVal);

    result_t _initCursor(MongoDB *db, AsyncEvent *ac);
    ASYNC_MEMBER1(MongoCursor, _initCursor, MongoDB *);

    result_t _nextCursor(int32_t &retVal, AsyncEvent *ac);
    ASYNC_MEMBERVALUE1(MongoCursor, _nextCursor, int32_t);

    result_t _bsonCursor(bson &out, AsyncEvent *ac);
    ASYNC_MEMBERVALUE1(MongoCursor, _bsonCursor, bson);

    result_t _limitCursor(int32_t size, AsyncEvent *ac);
    ASYNC_MEMBER1(MongoCursor, _limitCursor, int32_t);

    result_t _skipCursor(int32_t num, AsyncEvent *ac);
    ASYNC_MEMBER1(MongoCursor, _skipCursor, int32_t);

public:
    class cursor : public mongo_cursor
    {
    public:
        obj_ptr<MongoDB> m_db;
    };

private:
    qstring m_ns;
    qstring m_name;
    cursor* m_cursor;
    bson m_bbq, m_bbp;
    bool m_bInit;
    bool m_bSpecial;
    v8::Persistent<v8::Object> m_query;
    enum
    {
        CUR_NONE, CUR_DATA, CUR_NODATA
    } m_state;
};

} /* namespace fibjs */
#endif /* MONGOCURSOR_H_ */
