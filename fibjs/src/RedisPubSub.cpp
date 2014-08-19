/*
 * RedisPubSub.cpp
 *
 *  Created on: Aug 18, 2014
 *      Author: lion
 */

#include "Redis.h"

namespace fibjs
{

result_t Redis::sub(Buffer_base *channel, v8::Local<v8::Function> func)
{
    if (m_subMode == 0)
        m_subMode = 1;

    std::string key;

    channel->toString(key);
    key = "s_" + key;

    on(key.c_str(), func);

    Variant v;
    return doCommand("SUBSCRIBE", channel, v);
}

result_t Redis::psub(const char *pattern, v8::Local<v8::Function> func)
{
    if (m_subMode == 0)
        m_subMode = 1;

    std::string key("p_");
    key += pattern;

    on(key.c_str(), func);

    Variant v;
    return doCommand("PSUBSCRIBE", pattern, v);
}

result_t Redis::sub(const char *prefix, const char *cmd, v8::Local<v8::Object> &map)
{
    if (m_subMode == 0)
        m_subMode = 1;

    v8::Local<v8::Array> channels = map->GetPropertyNames();
    int32_t sz = channels->Length();
    int32_t i;

    for (i = 0; i < sz; i ++)
    {
        v8::Local<v8::Value> channel = channels->Get(i);
        std::string s;

        GetArgumentValue(channel, s);
        s = prefix + s;

        v8::Local<v8::Value> value = map->Get(channel);

        if (!value->IsFunction())
            return CHECK_ERROR(CALL_E_TYPEMISMATCH);

        v8::Local<v8::Function> func = v8::Local<v8::Function>::Cast(value);

        on(s.c_str(), func);
    }

    Variant v;
    return doCommand(cmd, channels, v);
}

result_t Redis::sub(v8::Local<v8::Object> map)
{
    return sub("s_", "SUBSCRIBE", map);
}

result_t Redis::psub(v8::Local<v8::Object> map)
{
    return sub("p_", "PSUBSCRIBE", map);
}

result_t Redis::unsub(Buffer_base *channel)
{
    std::string key;

    channel->toString(key);
    key = "s_" + key;

    off(key.c_str());

    Variant v;
    return doCommand("UNSUBSCRIBE", channel, v);
}

result_t Redis::unpsub(const char *pattern)
{
    std::string key("p_");
    key += pattern;

    off(key.c_str());

    Variant v;
    return doCommand("PUNSUBSCRIBE", pattern, v);
}

result_t Redis::unsub(const char *prefix, const char *cmd, v8::Local<v8::Array> &channels)
{
    int32_t sz = channels->Length();
    int32_t i;

    for (i = 0; i < sz; i ++)
    {
        v8::Local<v8::Value> key = channels->Get(i);
        std::string s;

        GetArgumentValue(key, s);
        s = prefix + s;

        off(s.c_str());
    }

    Variant v;
    return doCommand(cmd, channels, v);
}

result_t Redis::unsub(v8::Local<v8::Array> channels)
{
    return unsub("s_", "UNSUBSCRIBE", channels);
}

result_t Redis::unpsub(v8::Local<v8::Array> patterns)
{
    return unsub("p_", "PUNSUBSCRIBE", patterns);
}

result_t Redis::pub(Buffer_base *channel, Buffer_base *message, int32_t &retVal)
{
    return doCommand("PUBLISH", channel, message, retVal);
}

}
