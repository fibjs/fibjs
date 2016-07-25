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
    virtual result_t toJSON(exlib::string key, v8::Local<v8::Value> &retVal);

public:
    // LruCache_base
    virtual result_t get_size(int32_t &retVal);
    virtual result_t clear();
    virtual result_t has(exlib::string name, bool &retVal);
    virtual result_t get(exlib::string name, v8::Local<v8::Value> &retVal);
    virtual result_t get(exlib::string name, v8::Local<v8::Function> updater, v8::Local<v8::Value> &retVal);
    virtual result_t set(exlib::string name, v8::Local<v8::Value> value);
    virtual result_t put(exlib::string name, v8::Local<v8::Value> value);
    virtual result_t put(v8::Local<v8::Object> map);
    virtual result_t remove(exlib::string name);
    virtual result_t isEmpty(bool &retVal);

private:
    class _linkedNode
    {
    public:
        date_t insert;
        std::map<exlib::string, void *>::iterator m_prev, m_next, m_prev1, m_next1;
    };

    inline std::map<exlib::string, void *>::iterator _generalize(std::map<exlib::string, _linkedNode>::iterator it)
    {
        return *((std::map<exlib::string, void *>::iterator *) &it);
    }

    inline std::map<exlib::string, _linkedNode>::iterator _instantiate(std::map<exlib::string, void *>::iterator it)
    {
        return *((std::map<exlib::string, _linkedNode>::iterator *) &it);
    }

    void remove(std::map<exlib::string, _linkedNode>::iterator it)
    {
        std::map<exlib::string, _linkedNode>::iterator prev = _instantiate(it->second.m_prev);
        std::map<exlib::string, _linkedNode>::iterator next = _instantiate(it->second.m_next);

        if (prev != m_datas.end())
            prev->second.m_next = _generalize(next);
        else
            m_begin_lru = next;

        if (next != m_datas.end())
            next->second.m_prev = _generalize(prev);
        else
            m_end_lru = prev;

        if (m_timeout > 0)
        {
            std::map<exlib::string, _linkedNode>::iterator prev1 = _instantiate(it->second.m_prev1);
            std::map<exlib::string, _linkedNode>::iterator next1 = _instantiate(it->second.m_next1);

            if (prev1 != m_datas.end())
                prev1->second.m_next1 = _generalize(next1);
            else
                m_begin = next1;

            if (next1 != m_datas.end())
                next1->second.m_prev1 = _generalize(prev1);
            else
                m_end = prev1;
        }

        DeletePrivate(it->first);

        m_datas.erase(it);
    }

    void insert(std::map<exlib::string, _linkedNode>::iterator it)
    {
        it->second.m_next = _generalize(m_begin_lru);
        it->second.m_prev = _generalize(m_datas.end());

        if (m_begin_lru != m_datas.end())
            m_begin_lru->second.m_prev = _generalize(it);
        else
            m_end_lru = it;

        m_begin_lru = it;

        if (m_timeout > 0)
        {
            it->second.m_next1 = _generalize(m_begin);
            it->second.m_prev1 = _generalize(m_datas.end());

            if (m_begin != m_datas.end())
                m_begin->second.m_prev1 = _generalize(it);
            else
                m_end = it;

            m_begin = it;
        }
    }

    void update(std::map<exlib::string, _linkedNode>::iterator it)
    {
        if (m_begin_lru != it)
        {
            std::map<exlib::string, _linkedNode>::iterator prev = _instantiate(it->second.m_prev);
            std::map<exlib::string, _linkedNode>::iterator next = _instantiate(it->second.m_next);

            if (prev != m_datas.end())
                prev->second.m_next = _generalize(next);
            else
                m_begin_lru = next;

            if (next != m_datas.end())
                next->second.m_prev = _generalize(prev);
            else
                m_end_lru = prev;

            it->second.m_next = _generalize(m_begin_lru);
            it->second.m_prev = _generalize(m_datas.end());

            if (m_begin_lru != m_datas.end())
                m_begin_lru->second.m_prev = _generalize(it);
            else
                m_end_lru = it;

            m_begin_lru = it;
        }
    }

    void update_time(std::map<exlib::string, _linkedNode>::iterator it)
    {
        if (m_timeout > 0 && m_begin != it)
        {
            std::map<exlib::string, _linkedNode>::iterator prev1 = _instantiate(it->second.m_prev1);
            std::map<exlib::string, _linkedNode>::iterator next1 = _instantiate(it->second.m_next1);

            if (prev1 != m_datas.end())
                prev1->second.m_next1 = _generalize(next1);
            else
                m_begin = next1;

            if (next1 != m_datas.end())
                next1->second.m_prev1 = _generalize(prev1);
            else
                m_end = prev1;

            it->second.m_next1 = _generalize(m_begin);
            it->second.m_prev1 = _generalize(m_datas.end());

            if (m_begin != m_datas.end())
                m_begin->second.m_prev1 = _generalize(it);
            else
                m_end = it;

            m_begin = it;
        }
    }

    void cleanup();

    std::map<exlib::string, _linkedNode> m_datas;
    std::map<exlib::string, _linkedNode>::iterator m_begin_lru;
    std::map<exlib::string, _linkedNode>::iterator m_end_lru;

    std::map<exlib::string, _linkedNode>::iterator m_begin;
    std::map<exlib::string, _linkedNode>::iterator m_end;

    std::map<exlib::string, obj_ptr<Event_base> > m_paddings;

    int32_t m_size;
    int32_t m_timeout;
    date_t m_checkTime;
};

} /* namespace fibjs */

#endif /* LRUCACHE_H_ */
