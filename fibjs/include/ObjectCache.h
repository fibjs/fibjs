/*
 * ObjectCache.h
 *
 *  Created on: May 27, 2024
 *      Author: lion
 */

#pragma once

#include "object.h"
#include <list>
#include <chrono>
#include <unordered_map>
#include <map>

namespace fibjs {

template <typename T>
class ObjectCache {
private:
    class CacheItem : public T {
    public:
        obj_ptr<T> m_obj;
        exlib::Event m_ready;
        std::chrono::time_point<std::chrono::system_clock> m_updated;

        std::unordered_map<exlib::string, CacheItem*>::iterator m_map_iter;
        std::list<CacheItem*>::iterator m_used_iter;
        std::list<obj_ptr<CacheItem>>::iterator m_expired_iter;
    };

public:
    typedef std::function<T*(exlib::string&)> Resolver;

public:
    ObjectCache(int32_t size = 1024, int64_t timeout = 300)
        : m_size(size)
        , m_timeout(timeout)
    {
    }

    ObjectCache(Resolver resolver, int32_t size = 1024, int64_t timeout = 300)
        : m_resolver(resolver)
        , m_size(size)
        , m_timeout(timeout)
    {
    }

public:
    void set(exlib::string key, T* obj)
    {
        m_lock.lock();

        obj_ptr<CacheItem> item;
        alloc(key, item);
        item->m_obj = obj;
        item->m_ready.set();

        m_lock.unlock();
    }

    bool lookup(exlib::string key, obj_ptr<T>& obj, bool auto_resolve = true)
    {
        obj_ptr<CacheItem> item;

        m_lock.lock();

        clean_expired();

        if (auto_resolve && m_resolver) {
            bool alloced = alloc(key, item);
            if (alloced) {
                m_lock.unlock();

                obj = m_resolver(key);

                item->m_obj = obj;
                item->m_ready.set();

                if (obj == nullptr) {
                    m_lock.lock();
                    erase_item(item);
                    m_lock.unlock();
                    return false;
                }

                return true;
            }
        } else {
            auto it = m_map.find(key);
            if (it == m_map.end()) {
                m_lock.unlock();
                return false;
            }

            item = it->second;
        }

        if (item->m_used_iter != m_used.begin()) {
            m_used.erase(item->m_used_iter);
            item->m_used_iter = m_used.insert(m_used.begin(), item);
        }

        m_lock.unlock();

        item->m_ready.wait();
        obj = item->m_obj;

        return obj != nullptr;
    }

    bool erase(exlib::string key)
    {
        m_lock.lock();

        auto it = m_map.find(key);
        if (it == m_map.end()) {
            m_lock.unlock();
            return false;
        }

        erase_item(it->second);

        m_lock.unlock();

        return true;
    }

    void set_resolver(Resolver resolver)
    {
        m_lock.lock();
        m_resolver = resolver;
        m_lock.unlock();
    }

    void resize(int32_t new_size)
    {
        m_lock.lock();
        m_size = new_size;
        clean_used();
        m_lock.unlock();
    }

    void set_timeout(int64_t timeout)
    {
        m_lock.lock();
        m_timeout = timeout;
        clean_expired();
        m_lock.unlock();
    }

    void clear()
    {
        m_lock.lock();
        while (!m_expired.empty())
            erase_item(m_expired.back());
        m_lock.unlock();
    }

private:
    void erase_item(CacheItem* item)
    {
        if (item->m_map_iter != m_map.end()) {
            m_map.erase(item->m_map_iter);
            m_used.erase(item->m_used_iter);
            m_expired.erase(item->m_expired_iter);
            item->m_map_iter = m_map.end();
        }
    }

    bool alloc(exlib::string key, obj_ptr<CacheItem>& item)
    {
        auto itr = m_map.find(key);
        if (itr != m_map.end()) {
            item = itr->second;

            if (item->m_used_iter != m_used.begin()) {
                m_used.erase(item->m_used_iter);
                item->m_used_iter = m_used.insert(m_used.begin(), item);
            }

            if (item->m_expired_iter != m_expired.begin()) {
                m_expired.erase(item->m_expired_iter);
                item->m_expired_iter = m_expired.insert(m_expired.begin(), item);
            }

            item->m_updated = std::chrono::system_clock::now();

            return false;
        }

        item = new CacheItem();
        item->m_map_iter = m_map.emplace(key, item).first;
        item->m_used_iter = m_used.insert(m_used.begin(), item);
        item->m_expired_iter = m_expired.insert(m_expired.begin(), item);

        item->m_updated = std::chrono::system_clock::now();

        clean_used();

        return true;
    }

    void clean_used()
    {
        while (m_used.size() > m_size)
            erase_item(m_used.back());
    }

    void clean_expired()
    {
        std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
        while (!m_expired.empty()) {
            CacheItem* item = m_expired.back();
            if (std::chrono::duration_cast<std::chrono::milliseconds>(now - item->m_updated).count() < m_timeout * 1000)
                break;

            erase_item(item);
        }
    }

private:
    Resolver m_resolver;
    std::unordered_map<exlib::string, CacheItem*> m_map;
    std::list<CacheItem*> m_used;
    std::list<obj_ptr<CacheItem>> m_expired;

    int32_t m_size;
    int64_t m_timeout;

    exlib::spinlock m_lock;
};

} // namespace fibjs
