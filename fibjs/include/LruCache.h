/*
 * LruCache.h
 *
 *  Created on: Dec 7, 2013
 *      Author: lion
 */

#include "ifs/LruCache.h"
#include "ifs/Event.h"
#include "map"

#ifndef LRUCACHE_H_
#define LRUCACHE_H_

namespace fibjs
{

class LruCache: public LruCache_base
{
public:
    LruCache(int32_t size, int32_t timeout) :
        m_size(size), m_timeout(timeout)
    {
        m_begin_lru = m_datas.end();
        m_end_lru = m_datas.end();

        m_begin = m_datas.end();
        m_end = m_datas.end();

        m_checkTime.now();
    }

public:
    // object_base
    virtual result_t toJSON(const char *key, v8::Local<v8::Value> &retVal);

public:
    // LruCache_base
    virtual result_t get_size(int32_t &retVal);
    virtual result_t clear();
    virtual result_t has(const char *name, bool &retVal);
    virtual result_t get(const char *name, v8::Local<v8::Value> &retVal);
    virtual result_t get(const char *name, v8::Local<v8::Function> updater, v8::Local<v8::Value> &retVal);
    virtual result_t set(const char *name, v8::Local<v8::Value> value);
    virtual result_t put(const char *name, v8::Local<v8::Value> value);
    virtual result_t put(v8::Local<v8::Object> map);
    virtual result_t remove(const char *name);
    virtual result_t isEmpty(bool &retVal);

private:
    class _linkedNode
    {
    public:
        std::map<std::string, _linkedNode>::iterator &last()
        {
            return *(std::map<std::string, _linkedNode>::iterator *) &m_last;
        }

        std::map<std::string, _linkedNode>::iterator &next()
        {
            return *(std::map<std::string, _linkedNode>::iterator *) &m_next;
        }

        std::map<std::string, _linkedNode>::iterator &last1()
        {
            return *(std::map<std::string, _linkedNode>::iterator *) &m_last1;
        }

        std::map<std::string, _linkedNode>::iterator &next1()
        {
            return *(std::map<std::string, _linkedNode>::iterator *) &m_next1;
        }

    public:
        VariantEx value;
        date_t insert;
        obj_ptr<Event_base> m_event;
        std::map<std::string, int>::iterator m_last, m_next, m_last1, m_next1;
    };

    void remove(std::map<std::string, _linkedNode>::iterator it)
    {
        std::map<std::string, _linkedNode>::iterator &last = it->second.last();
        std::map<std::string, _linkedNode>::iterator &next = it->second.next();

        if (last != m_datas.end())
            last->second.next() = next;
        else
            m_begin_lru = next;

        if (next != m_datas.end())
            next->second.last() = last;
        else
            m_end_lru = last;

        if (m_timeout > 0)
        {
            std::map<std::string, _linkedNode>::iterator &last1 = it->second.last1();
            std::map<std::string, _linkedNode>::iterator &next1 = it->second.next1();

            if (last1 != m_datas.end())
                last1->second.next1() = next1;
            else
                m_begin = next1;

            if (next1 != m_datas.end())
                next1->second.last1() = last1;
            else
                m_end = last1;
        }

        m_datas.erase(it);
    }

    void insert(std::map<std::string, _linkedNode>::iterator it)
    {
        it->second.next() = m_begin_lru;
        it->second.last() = m_datas.end();

        if (m_begin_lru != m_datas.end())
            m_begin_lru->second.last() = it;
        else
            m_end_lru = it;

        m_begin_lru = it;

        if (m_timeout > 0)
        {
            it->second.next1() = m_begin;
            it->second.last1() = m_datas.end();

            if (m_begin != m_datas.end())
                m_begin->second.last1() = it;
            else
                m_end = it;

            m_begin = it;
        }
    }

    void update(std::map<std::string, _linkedNode>::iterator it)
    {
        if (m_begin_lru != it)
        {
            std::map<std::string, _linkedNode>::iterator &last = it->second.last();
            std::map<std::string, _linkedNode>::iterator &next = it->second.next();

            if (last != m_datas.end())
                last->second.next() = next;
            else
                m_begin_lru = next;

            if (next != m_datas.end())
                next->second.last() = last;
            else
                m_end_lru = last;

            it->second.next() = m_begin_lru;
            it->second.last() = m_datas.end();

            if (m_begin_lru != m_datas.end())
                m_begin_lru->second.last() = it;
            else
                m_end_lru = it;

            m_begin_lru = it;
        }
    }

    void update_time(std::map<std::string, _linkedNode>::iterator it)
    {
        if (m_timeout > 0 && m_begin != it)
        {
            std::map<std::string, _linkedNode>::iterator &last1 = it->second.last1();
            std::map<std::string, _linkedNode>::iterator &next1 = it->second.next1();

            if (last1 != m_datas.end())
                last1->second.next1() = next1;
            else
                m_begin = next1;

            if (next1 != m_datas.end())
                next1->second.last1() = last1;
            else
                m_end = last1;

            it->second.next1() = m_begin;
            it->second.last1() = m_datas.end();

            if (m_begin != m_datas.end())
                m_begin->second.last1() = it;
            else
                m_end = it;

            m_begin = it;
        }
    }

    void cleanup();

    std::map<std::string, _linkedNode> m_datas;
    std::map<std::string, _linkedNode>::iterator m_begin_lru;
    std::map<std::string, _linkedNode>::iterator m_end_lru;

    std::map<std::string, _linkedNode>::iterator m_begin;
    std::map<std::string, _linkedNode>::iterator m_end;

    int32_t m_size;
    int32_t m_timeout;
    date_t m_checkTime;
};

} /* namespace fibjs */

#endif /* LRUCACHE_H_ */
