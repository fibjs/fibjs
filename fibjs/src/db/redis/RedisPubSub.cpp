/*
 * RedisPubSub.cpp
 *
 *  Created on: Aug 18, 2014
 *      Author: lion
 */

#include "Redis.h"

namespace fibjs
{

#define SUBSCRIBE       0
#define UNSUBSCRIBE     1
#define PSUBSCRIBE      2
#define PUNSUBSCRIBE    3

const char *s_cmd[][2] =
{
    {"SUBSCRIBE", "s_"},
    {"UNSUBSCRIBE", "s_"},
    {"PSUBSCRIBE", "p_"},
    {"PUNSUBSCRIBE", "p_"}
};

bool Redis::regsub(std::string &key, v8::Local<v8::Function> func)
{
    int32_t n = 0;
    on(key.c_str(), func, n);

    if (n)
    {
        std::map<std::string, int32_t>::iterator it = m_funcs.find(key);

        if (it != m_funcs.end())
            it->second ++;
        else
        {
            m_funcs.insert(std::pair<std::string, int32_t>(key, 1));
            return true;
        }
    }

    return false;
}

bool Redis::unregsub(std::string &key, v8::Local<v8::Function> func)
{
    int32_t n = 0;
    off(key.c_str(), func, n);

    if (n)
    {
        std::map<std::string, int32_t>::iterator it = m_funcs.find(key);

        if (it != m_funcs.end() && (--(it->second) == 0))
        {
            m_funcs.erase(it);
            return true;
        }
    }

    return false;
}

result_t Redis::_single(std::string key, v8::Local<v8::Function> func, int32_t cmd)
{
    std::string key1 = s_cmd[cmd][1] + key;

    m_subMode = 1;
    if (!((cmd & 1) ? unregsub(key1, func) : regsub(key1, func)))
        return 0;

    Variant v;
    return doCommand(s_cmd[cmd][0], key, v);
}

result_t Redis::sub(Buffer_base *channel, v8::Local<v8::Function> func)
{
    std::string key;
    channel->toString(key);
    return _single(key, func, SUBSCRIBE);
}

result_t Redis::unsub(Buffer_base *channel, v8::Local<v8::Function> func)
{
    std::string key;
    channel->toString(key);
    return _single(key, func, UNSUBSCRIBE);
}

result_t Redis::psub(const char *pattern, v8::Local<v8::Function> func)
{
    return _single(pattern, func, PSUBSCRIBE);
}

result_t Redis::unpsub(const char *pattern, v8::Local<v8::Function> func)
{
    return _single(pattern, func, PUNSUBSCRIBE);
}

result_t Redis::_map(v8::Local<v8::Object> &map, int32_t cmd)
{
    m_subMode = 1;

    Isolate* isolate = holder();
    v8::Local<v8::Array> channels = map->GetPropertyNames();
    int32_t sz = channels->Length();
    int32_t i;
    v8::Local<v8::Array> subs = v8::Array::New(isolate->m_isolate);
    int32_t count = 0;

    for (i = 0; i < sz; i ++)
    {
        v8::Local<v8::Value> channel = channels->Get(i);
        std::string s;

        GetArgumentValue(channel, s);
        s = s_cmd[cmd][1] + s;

        v8::Local<v8::Value> value = map->Get(channel);

        if (!value->IsFunction())
            return CHECK_ERROR(CALL_E_TYPEMISMATCH);

        v8::Local<v8::Function> func = v8::Local<v8::Function>::Cast(value);

        if ((cmd & 1) ? unregsub(s, func) : regsub(s, func))
            subs->Set(count ++, channel);
    }

    if (!count)
        return 0;

    Variant v;
    return doCommand(s_cmd[cmd][0], subs, v);
}

result_t Redis::sub(v8::Local<v8::Object> map)
{
    return _map(map, SUBSCRIBE);
}

result_t Redis::psub(v8::Local<v8::Object> map)
{
    return _map(map, PSUBSCRIBE);
}

result_t Redis::unsub(v8::Local<v8::Object> map)
{
    return _map(map, UNSUBSCRIBE);
}

result_t Redis::unpsub(v8::Local<v8::Object> map)
{
    return _map(map, PUNSUBSCRIBE);
}

result_t Redis::unsub(std::string key, int32_t cmd)
{
    int32_t n = 0;
    std::string key1 = s_cmd[cmd][1] + key;
    off(key1.c_str(), n);
    m_funcs.erase(key1);

    Variant v;
    return doCommand(s_cmd[cmd][0], key, v);
}

result_t Redis::unsub(Buffer_base *channel)
{
    std::string key;

    channel->toString(key);
    return unsub(key, UNSUBSCRIBE);
}

result_t Redis::unpsub(const char *pattern)
{
    return unsub(pattern, PUNSUBSCRIBE);
}

result_t Redis::unsub(v8::Local<v8::Array> &channels, int32_t cmd)
{
    int32_t sz = channels->Length();
    int32_t i;

    for (i = 0; i < sz; i ++)
    {
        v8::Local<v8::Value> key = channels->Get(i);
        std::string s;

        GetArgumentValue(key, s);
        s = s_cmd[cmd][1] + s;

        int32_t n = 0;
        off(s.c_str(), n);
        m_funcs.erase(s);
    }

    Variant v;
    return doCommand(s_cmd[cmd][0], channels, v);
}

result_t Redis::unsub(v8::Local<v8::Array> channels)
{
    return unsub(channels, UNSUBSCRIBE);
}

result_t Redis::unpsub(v8::Local<v8::Array> patterns)
{
    return unsub(patterns, PUNSUBSCRIBE);
}

result_t Redis::onsuberror(v8::Local<v8::Function> func)
{
    int32_t n = 0;
    return on("suberror", func, n);
}

result_t Redis::pub(Buffer_base *channel, Buffer_base *message, int32_t &retVal)
{
    return doCommand("PUBLISH", channel, message, retVal);
}

}
