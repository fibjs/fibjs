/*
 * RedisSortedSet.cpp
 *
 *  Created on: Aug 6, 2014
 *      Author: lion
 */

#include "object.h"
#include "RedisSortedSet.h"

namespace fibjs {

result_t RedisSortedSet::add(v8::Local<v8::Object> sms, int32_t& retVal)
{
    if (sms->IsArray())
        return CHECK_ERROR(CALL_E_INVALIDARG);

    JSArray keys = sms->GetPropertyNames();
    v8::Local<v8::Array> mss = v8::Array::New(holder()->m_isolate);

    int32_t i, n = 0;

    for (i = 0; i < (int32_t)keys->Length(); i++) {
        JSValue v = keys->Get(i);

        mss->Set(n++, JSValue(sms->Get(v)));
        mss->Set(n++, v);
    }

    return m_rdb->doCommand("ZADD", m_key, mss, retVal);
}

result_t RedisSortedSet::add(OptArgs sms, int32_t& retVal)
{
    if (sms.Length() & 1)
        return CHECK_ERROR(CALL_E_INVALIDARG);

    std::vector<v8::Local<v8::Value>> mss;
    sms.GetData(mss);

    int32_t i;
    for (i = 0; i < (int32_t)mss.size(); i += 2) {
        v8::Local<v8::Value> v = mss[i];
        mss[i] = mss[i + 1];
        mss[i + 1] = v;
    }

    return m_rdb->doCommand("ZADD", m_key, mss, retVal);
}

result_t RedisSortedSet::score(Buffer_base* member, obj_ptr<Buffer_base>& retVal)
{
    return m_rdb->doCommand("ZSCORE", m_key, member, retVal);
}

result_t RedisSortedSet::incr(Buffer_base* member, int64_t num, obj_ptr<Buffer_base>& retVal)
{
    return m_rdb->doCommand("ZINCRBY", m_key, num, member, retVal);
}

result_t RedisSortedSet::remove(v8::Local<v8::Array> members, int32_t& retVal)
{
    return m_rdb->doCommand("ZREM", m_key, members, retVal);
}

result_t RedisSortedSet::remove(OptArgs members, int32_t& retVal)
{
    return m_rdb->doCommand("ZREM", m_key, members, retVal);
}

result_t RedisSortedSet::len(int32_t& retVal)
{
    return m_rdb->doCommand("ZCARD", m_key, retVal);
}

result_t RedisSortedSet::count(int32_t min, int32_t max, int32_t& retVal)
{
    return m_rdb->doCommand("ZCOUNT", m_key, min, max, retVal);
}

result_t RedisSortedSet::range(int32_t start, int32_t stop, bool withScores,
    obj_ptr<NArray>& retVal)
{
    if (withScores)
        return m_rdb->doCommand("ZRANGE", m_key, start, stop, "WITHSCORES", retVal);
    else
        return m_rdb->doCommand("ZRANGE", m_key, start, stop, retVal);
}

result_t RedisSortedSet::rangeRev(int32_t start, int32_t stop, bool withScores,
    obj_ptr<NArray>& retVal)
{
    if (withScores)
        return m_rdb->doCommand("ZREVRANGE", m_key, start, stop, "WITHSCORES", retVal);
    else
        return m_rdb->doCommand("ZREVRANGE", m_key, start, stop, retVal);
}

result_t RedisSortedSet::rank(Buffer_base* member, int32_t& retVal)
{
    return m_rdb->doCommand("ZRANK", m_key, member, retVal);
}

result_t RedisSortedSet::rankRev(Buffer_base* member, int32_t& retVal)
{
    return m_rdb->doCommand("ZREVRANK", m_key, member, retVal);
}
}
