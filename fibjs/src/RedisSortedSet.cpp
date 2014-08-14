/*
 * RedisSortedSet.cpp
 *
 *  Created on: Aug 6, 2014
 *      Author: lion
 */

#include "RedisSortedSet.h"

namespace fibjs
{

result_t RedisSortedSet::add(v8::Local<v8::Array> sms, int32_t &retVal)
{
    v8::Local<v8::Array> mss = v8::Array::New(isolate);
    int32_t i;

    for (i = 0; i < (int32_t)sms->Length(); i += 2)
    {
        mss->Set(i, sms->Get(i + 1));
        mss->Set(i + 1, sms->Get(i));
    }

    return m_rdb->doCommand("ZADD", m_key, mss, retVal);
}

result_t RedisSortedSet::add(const v8::FunctionCallbackInfo<v8::Value> &args, int32_t &retVal)
{
    v8::Local<v8::Array> mss = v8::Array::New(isolate);
    int32_t i;

    for (i = 0; i < (int32_t)args.Length(); i += 2)
    {
        mss->Set(i, args[i + 1]);
        mss->Set(i + 1, args[i]);
    }

    return m_rdb->doCommand("ZADD", m_key, mss, retVal);
}

result_t RedisSortedSet::score(Buffer_base *member, obj_ptr<Buffer_base> &retVal)
{
    return m_rdb->doCommand("ZSCORE", m_key, member, retVal);
}

result_t RedisSortedSet::incr(Buffer_base *member, int64_t num, obj_ptr<Buffer_base> &retVal)
{
    return m_rdb->doCommand("ZINCRBY", m_key, num, member, retVal);
}

result_t RedisSortedSet::remove(v8::Local<v8::Array> members, int32_t &retVal)
{
    return m_rdb->doCommand("ZREM", m_key, members, retVal);
}

result_t RedisSortedSet::remove(const v8::FunctionCallbackInfo<v8::Value> &args, int32_t &retVal)
{
    Redis::_arg a(args);
    return m_rdb->doCommand("ZREM", m_key, a, retVal);
}

result_t RedisSortedSet::len(int32_t &retVal)
{
    return m_rdb->doCommand("ZCARD", m_key, retVal);
}

result_t RedisSortedSet::count(int32_t min, int32_t max, int32_t &retVal)
{
    return m_rdb->doCommand("ZCOUNT", m_key, min, max, retVal);
}

result_t RedisSortedSet::range(int32_t start, int32_t stop, bool withScores,
                               obj_ptr<List_base> &retVal)
{
    if (withScores)
        return m_rdb->doCommand("ZRANGE", m_key, start, stop, "WITHSCORES", retVal);
    else
        return m_rdb->doCommand("ZRANGE", m_key, start, stop, retVal);
}

result_t RedisSortedSet::rangeRev(int32_t start, int32_t stop, bool withScores,
                                  obj_ptr<List_base> &retVal)
{
    if (withScores)
        return m_rdb->doCommand("ZREVRANGE", m_key, start, stop, "WITHSCORES", retVal);
    else
        return m_rdb->doCommand("ZREVRANGE", m_key, start, stop, retVal);
}

result_t RedisSortedSet::rank(Buffer_base *member, int32_t &retVal)
{
    return m_rdb->doCommand("ZRANK", m_key, member, retVal);
}

result_t RedisSortedSet::rankRev(Buffer_base *member, int32_t &retVal)
{
    return m_rdb->doCommand("ZREVRANK", m_key, member, retVal);
}

}
