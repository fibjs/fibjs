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
    std::map<std::string, _linkedNode>::iterator find;

    cleanup();

    while (true)
    {
        find  = m_datas.find(name);

        if (find == m_datas.end())
        {
            if (!updater.IsEmpty())
            {
                static _linkedNode newNode;

                m_datas.insert(std::pair<std::string, _linkedNode>(name, newNode));
                find = m_datas.find(name);

                insert(find);

                if (m_timeout > 0)
                    find->second.insert.now();

                obj_ptr<Event_base> e = new Event();
                find->second.m_event = e;

                v8::Handle<v8::String> n = v8::String::NewFromUtf8(isolate, name);
                v8::Handle<v8::Value> a = n;
                v8::Local<v8::Value> v = updater->Call(wrap(), 1, &a);

                e->set();

                find = m_datas.find(name);
                if (!v.IsEmpty())
                {
                    if (find != m_datas.end())
                    {
                        wrap()->SetHiddenValue(n, v);
                        if (find->second.m_event == e)
                            find->second.m_event.Release();
                    }
                }
                else
                {
                    if (find != m_datas.end() && find->second.m_event == e)
                        remove(find);
                    return CHECK_ERROR(CALL_E_JAVASCRIPT);
                }

                retVal = v;
            }

            return 0;
        }

        if (find->second.m_event)
        {
            obj_ptr<Event_base> e = find->second.m_event;
            e->wait();
        }
        else
            break;
    }

    update(find);
    retVal = wrap()->GetHiddenValue(v8::String::NewFromUtf8(isolate, name));

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
        wrap()->SetHiddenValue(v8::String::NewFromUtf8(isolate, name), value);
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
        update(find);
        update_time(find);
    }

    if (m_timeout > 0)
        find->second.insert.now();
    wrap()->SetHiddenValue(v8::String::NewFromUtf8(isolate, name), value);

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

        if (!k->IsNumber() && !k->IsNumberObject())
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

    std::map<std::string, _linkedNode>::iterator it = m_begin_lru;
    v8::Local<v8::Object> obj = v8::Object::New(isolate);

    while (it != m_datas.end())
    {
        v8::Local<v8::String> name = v8::String::NewFromUtf8(isolate, it->first.c_str(),
                                     v8::String::kNormalString,
                                     (int) it->first.length());
        obj->Set(name, wrap()->GetHiddenValue(name));
        it = it->second.next();
    }

    retVal = obj;
    return 0;
}

} /* namespace fibjs */
