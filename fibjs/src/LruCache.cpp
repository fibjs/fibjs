/*
 * LruCache.cpp
 *
 *  Created on: Dec 7, 2013
 *      Author: lion
 */

#include <LruCache.h>

namespace fibjs
{

result_t LruCache_base::_new(int32_t size, int32_t timeout,
                             obj_ptr<LruCache_base> &retVal)
{
    retVal = new LruCache(size, timeout);
    return 0;
}

void LruCache::cleanup()
{
    std::map<std::string, _linkedNode>::iterator it;
    date_t t;

    t.now();

    if (m_timeout > 0)
    {
        while (((it = m_end) != m_datas.end())
                && (t.diff(it->second.update) > m_timeout))
        {
            remove(it);
            m_datas.erase(it);
        }
    }

    if (m_size > 0)
    {
        while ((int32_t) m_datas.size() > m_size)
        {
            it = m_end;
            remove(it);
            m_datas.erase(it);
        }
    }
}

result_t LruCache::get_size(int32_t &retVal)
{
    cleanup();

    retVal = (int32_t) m_datas.size();
    return 0;
}

result_t LruCache::clear()
{
    m_datas.clear();
    m_begin = m_datas.end();
    m_end = m_datas.end();

    return 0;
}

result_t LruCache::has(const char *name, bool &retVal)
{
    cleanup();

    retVal = m_datas.find(name) != m_datas.end();
    return 0;
}

result_t LruCache::get(const char *name, v8::Local<v8::Value> &retVal)
{
    cleanup();

    std::map<std::string, _linkedNode>::iterator find = m_datas.find(name);

    if (find == m_datas.end())
        return 0;

    if (m_begin != find)
    {
        remove(find);
        insert(find);
    }

    retVal = find->second.value;
    find->second.update.now();

    return 0;
}

result_t LruCache::set(const char *name, v8::Local<v8::Value> value)
{
    std::map<std::string, _linkedNode>::iterator find = m_datas.find(name);

    if (find != m_datas.end())
    {
        if (m_begin != find)
        {
            remove(find);
            insert(find);
        }

        find->second.value = value;
        find->second.update.now();
    }

    cleanup();

    return 0;
}

result_t LruCache::put(const char *name, v8::Local<v8::Value> value)
{
    std::map<std::string, _linkedNode>::iterator find = m_datas.find(name);

    if (find == m_datas.end())
    {
        static _linkedNode newNode;

        m_datas.insert(std::pair<std::string, _linkedNode>(name, newNode));
        find = m_datas.find(name);

        insert(find);
    }
    else
    {
        if (m_begin != find)
        {
            remove(find);
            insert(find);
        }
    }

    find->second.value = value;
    find->second.update.now();

    cleanup();

    return 0;
}

inline result_t _map(LruCache *o, v8::Local<v8::Object> m,
                     result_t (LruCache::*fn)(const char *name, v8::Local<v8::Value> value))
{
    v8::Local<v8::Array> ks = m->GetPropertyNames();
    int len = ks->Length();
    int i;

    for (i = 0; i < len; i++)
    {
        v8::Local<v8::Value> k = ks->Get(i);

        if (!k->IsNumber())
            (o->*fn)(*v8::String::Utf8Value(k), m->Get(k));
    }

    return 0;
}

result_t LruCache::put(v8::Local<v8::Object> map)
{
    return _map(this, map, &LruCache::put);
}

result_t LruCache::remove(const char *name)
{
    std::map<std::string, _linkedNode>::iterator find = m_datas.find(name);

    if (find == m_datas.end())
        return 0;

    remove(find);
    m_datas.erase(find);

    return 0;
}

result_t LruCache::isEmpty(bool &retVal)
{
    cleanup();

    retVal = m_datas.empty();
    return 0;
}

result_t LruCache::toJSON(const char *key, v8::Local<v8::Value> &retVal)
{
    cleanup();

    std::map<std::string, _linkedNode>::iterator it = m_begin;
    v8::Local<v8::Object> obj = v8::Object::New(isolate);

    while (it != m_datas.end())
    {
        obj->Set(v8::String::NewFromUtf8(isolate, it->first.c_str(),
                                         v8::String::kNormalString,
                                         (int) it->first.length()),
                 it->second.value);
        it = it->second.next();
    }

    retVal = obj;
    return 0;
}

} /* namespace fibjs */
