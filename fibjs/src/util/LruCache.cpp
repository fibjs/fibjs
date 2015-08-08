/*
 * LruCache.cpp
 *
 *  Created on: Dec 7, 2013
 *      Author: lion
 */

#include <LruCache.h>
#include <Event.h>

namespace fibjs
{

result_t LruCache_base::_new(int32_t size, int32_t timeout,
                             obj_ptr<LruCache_base> &retVal, v8::Local<v8::Object> This)
{
    retVal = new LruCache(size, timeout);
    return 0;
}

void LruCache::cleanup()
{
    std::map<std::string, _linkedNode>::iterator it;

    if (m_timeout > 0)
    {
        date_t t;
        t.now();

        while ((it = m_end) != m_datas.end() &&
                t.diff(it->second.insert) > m_timeout)
            remove(it);
    }

    if (m_size > 0)
    {
        while ((int32_t) m_datas.size() > m_size)
            remove(m_end_lru);
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

    m_begin_lru = m_datas.end();
    m_end_lru = m_datas.end();

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
    return get(name, v8::Local<v8::Function>(), retVal);
}

result_t LruCache::get(const char *name, v8::Local<v8::Function> updater,
                       v8::Local<v8::Value> &retVal)
{
    static _linkedNode newNode;
    v8::Handle<v8::Object> o = wrap();
    v8::Handle<v8::String> n = v8::String::NewFromUtf8(Isolate::now()->isolate, name);
    std::string sname(name);
    v8::Handle<v8::Value> a = n;

    std::map<std::string, _linkedNode>::iterator find;

    cleanup();

    while (true)
    {
        obj_ptr<Event_base> e;

        find = m_datas.find(sname);
        if (find != m_datas.end())
            break;

        if (updater.IsEmpty())
            return 0;

        std::map<std::string, obj_ptr<Event_base> >::iterator padding;
        padding = m_paddings.find(sname);
        if (padding == m_paddings.end())
        {
            e = new Event();
            padding = m_paddings.insert(std::pair<std::string, obj_ptr<Event_base> >(sname, e)).first;
            v8::Local<v8::Value> v = updater->Call(o, 1, &a);
            m_paddings.erase(padding);
            e->set();

            if (v.IsEmpty())
                return CALL_E_JAVASCRIPT;

            find = m_datas.insert(std::pair<std::string, _linkedNode>(sname, newNode)).first;
            insert(find);

            if (m_timeout > 0)
                find->second.insert.now();
            o->SetHiddenValue(n, v);

            retVal = v;
            return 0;
        }

        e = padding->second;
        e->wait();
    }

    update(find);
    retVal = o->GetHiddenValue(n);

    return 0;
}

result_t LruCache::set(const char *name, v8::Local<v8::Value> value)
{
    std::map<std::string, _linkedNode>::iterator find = m_datas.find(name);

    if (find != m_datas.end())
    {
        update(find);
        update_time(find);

        if (m_timeout > 0)
            find->second.insert.now();
        wrap()->SetHiddenValue(v8::String::NewFromUtf8(Isolate::now()->isolate, name), value);
    }

    cleanup();

    return 0;
}

result_t LruCache::put(const char *name, v8::Local<v8::Value> value)
{
    static _linkedNode newNode;
    std::map<std::string, _linkedNode>::iterator find = m_datas.find(name);

    if (find == m_datas.end())
    {
        find = m_datas.insert(std::pair<std::string, _linkedNode>(name, newNode)).first;
        insert(find);
    }
    else
    {
        update(find);
        update_time(find);
    }

    if (m_timeout > 0)
        find->second.insert.now();
    wrap()->SetHiddenValue(v8::String::NewFromUtf8(Isolate::now()->isolate, name), value);

    cleanup();

    return 0;
}

inline result_t _map(LruCache *o, v8::Local<v8::Object> m,
                     result_t (LruCache::*fn)(const char *name, v8::Local<v8::Value> value))
{
    v8::Local<v8::Array> ks = m->GetPropertyNames();
    int32_t len = ks->Length();
    int32_t i;

    for (i = 0; i < len; i++)
    {
        v8::Local<v8::Value> k = ks->Get(i);
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

    Isolate* isolate = Isolate::now();
    std::map<std::string, _linkedNode>::iterator it = m_begin_lru;
    v8::Local<v8::Object> obj = v8::Object::New(isolate->isolate);

    while (it != m_datas.end())
    {
        v8::Local<v8::String> name = v8::String::NewFromUtf8(isolate->isolate, it->first.c_str(),
                                     v8::String::kNormalString,
                                     (int32_t) it->first.length());
        obj->Set(name, wrap()->GetHiddenValue(name));
        it = it->second.m_next;
    }

    retVal = obj;
    return 0;
}

} /* namespace fibjs */
