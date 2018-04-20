/*
 * RedisSet.cpp
 *
 *  Created on: Aug 5, 2014
 *      Author: lion
 */

#include "object.h"
#include "RedisSet.h"

namespace fibjs {

result_t RedisSet::add(v8::Local<v8::Array> members, int32_t& retVal)
{
    return m_rdb->doCommand("SADD", m_key, members, retVal);
}

result_t RedisSet::add(OptArgs members, int32_t& retVal)
{
    return m_rdb->doCommand("SADD", m_key, members, retVal);
}

result_t RedisSet::remove(v8::Local<v8::Array> members, int32_t& retVal)
{
    return m_rdb->doCommand("SREM", m_key, members, retVal);
}

result_t RedisSet::remove(OptArgs members, int32_t& retVal)
{
    return m_rdb->doCommand("SREM", m_key, members, retVal);
}

result_t RedisSet::len(int32_t& retVal)
{
    return m_rdb->doCommand("SCARD", m_key, retVal);
}

result_t RedisSet::exists(Buffer_base* member, bool& retVal)
{
    return m_rdb->doCommand("SISMEMBER", m_key, member, retVal);
}

result_t RedisSet::members(obj_ptr<NArray>& retVal)
{
    return m_rdb->doCommand("SMEMBERS", m_key, retVal);
}

result_t RedisSet::pop(obj_ptr<Buffer_base>& retVal)
{
    return m_rdb->doCommand("SPOP", m_key, retVal);
}

result_t RedisSet::randMember(v8::Local<v8::Value>& retVal)
{
    obj_ptr<Buffer_base> v;
    result_t hr;

    hr = m_rdb->doCommand("SRANDMEMBER", m_key, v);
    if (hr < 0)
        return hr;

    retVal = GetReturnValue(holder()->m_isolate, v);

    return 0;
}

result_t RedisSet::randMember(int32_t count, v8::Local<v8::Value>& retVal)
{
    obj_ptr<NArray> v;
    result_t hr;

    hr = m_rdb->doCommand("SRANDMEMBER", m_key, count, v);
    if (hr < 0)
        return hr;
    
    retVal = GetReturnValue(holder()->m_isolate, v);

    return 0;
}
}
