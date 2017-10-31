/*
 * RedisList.cpp
 *
 *  Created on: Aug 5, 2014
 *      Author: lion
 */

#include "object.h"
#include "RedisList.h"

namespace fibjs {

result_t RedisList::push(v8::Local<v8::Array> values, int32_t& retVal)
{
    return m_rdb->doCommand("LPUSH", m_key, values, retVal);
}

result_t RedisList::push(OptArgs values, int32_t& retVal)
{
    return m_rdb->doCommand("LPUSH", m_key, values, retVal);
}

result_t RedisList::pop(obj_ptr<Buffer_base>& retVal)
{
    return m_rdb->doCommand("LPOP", m_key, retVal);
}

result_t RedisList::rpush(v8::Local<v8::Array> values, int32_t& retVal)
{
    return m_rdb->doCommand("RPUSH", m_key, values, retVal);
}

result_t RedisList::rpush(OptArgs values, int32_t& retVal)
{
    return m_rdb->doCommand("RPUSH", m_key, values, retVal);
}

result_t RedisList::rpop(obj_ptr<Buffer_base>& retVal)
{
    return m_rdb->doCommand("RPOP", m_key, retVal);
}

result_t RedisList::set(int32_t index, Buffer_base* value)
{
    Variant v;
    return m_rdb->doCommand("LSET", m_key, index, value, v);
}

result_t RedisList::get(int32_t index, obj_ptr<Buffer_base>& retVal)
{
    return m_rdb->doCommand("LINDEX", m_key, index, retVal);
}

result_t RedisList::insertBefore(Buffer_base* pivot, Buffer_base* value, int32_t& retVal)
{
    return m_rdb->doCommand("LINSERT", m_key, "BEFORE", pivot, value, retVal);
}

result_t RedisList::insertAfter(Buffer_base* pivot, Buffer_base* value, int32_t& retVal)
{
    return m_rdb->doCommand("LINSERT", m_key, "AFTER", pivot, value, retVal);
}

result_t RedisList::remove(int32_t count, Buffer_base* value, int32_t& retVal)
{
    return m_rdb->doCommand("LREM", m_key, count, value, retVal);
}

result_t RedisList::trim(int32_t start, int32_t stop)
{
    Variant v;
    return m_rdb->doCommand("LTRIM", m_key, start, stop, v);
}

result_t RedisList::len(int32_t& retVal)
{
    return m_rdb->doCommand("LLEN", m_key, retVal);
}

result_t RedisList::range(int32_t start, int32_t stop, obj_ptr<NArray>& retVal)
{
    return m_rdb->doCommand("LRANGE", m_key, start, stop, retVal);
}
}
