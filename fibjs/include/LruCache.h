/*
 * LruCache.h
 *
 *  Created on: May 27, 2024
 *      Author: lion
 */

#pragma once

#include "obj_ptr.h"
#include <list>
#include <chrono>
#include <unordered_map>
#include <map>
#include <optional>

namespace fibjs {

template <typename T>
class LruCache {
private:
    class CacheItem : public obj_base {
    public:
        std::optional<T> m_value;
        exlib::Event m_ready;

        typename std::unordered_map<exlib::string, obj_ptr<CacheItem>>::iterator m_map_iter;
        typename std::list<CacheItem*>::iterator m_used_iter;

        std::chrono::time_point<std::chrono::system_clock> m_visited;
        std::chrono::time_point<std::chrono::system_clock> m_updated;
        typename std::list<CacheItem*>::iterator m_expired_iter;
    };

public:
    typedef std::function<bool(exlib::string&, T&)> Resolver;

public:
    LruCache(int32_t size = 1024, int64_t idleTimeout = 300, int64_t updateTimeout = 300)
        : m_size(size)
        , m_idleTimeout(idleTimeout > 0 ? idleTimeout : updateTimeout)
        , m_updateTimeout(updateTimeout)
    {
    }

    LruCache(Resolver resolver, int32_t size = 1024, int64_t idleTimeout = 300, int64_t updateTimeout = 300)
        : m_resolver(resolver)
        , m_size(size)
        , m_idleTimeout(idleTimeout > 0 ? idleTimeout : updateTimeout)
        , m_updateTimeout(updateTimeout)
    {
    }

public:
    void set(exlib::string key, T value)
    {
        m_lock.lock();

        obj_ptr<CacheItem> item;
        find_item(key, item, true);
        item->m_value = value;
        item->m_ready.set();

        m_lock.unlock();
    }

    bool lookup(exlib::string key, T& value, bool auto_resolve = true)
    {
        return lookup(key, value, auto_resolve ? m_resolver : nullptr);
    }

    bool lookup(exlib::string key, T& value, Resolver resolver)
    {
        obj_ptr<CacheItem> item;

        m_lock.lock();

        clean_expired();

        if (resolver) {
            bool alloced = find_item(key, item, true);
            if (alloced) {
                m_lock.unlock();

                if (resolver(key, value)) {
                    item->m_value = value;
                    item->m_ready.set();

                    return true;
                }

                item->m_ready.set();

                m_lock.lock();
                erase_item(item);
                m_lock.unlock();

                return false;
            }
        } else {
            find_item(key, item, false);
            if (item == nullptr) {
                m_lock.unlock();
                return false;
            }
        }

        m_lock.unlock();

        item->m_ready.wait();
        if (item->m_value.has_value()) {
            value = item->m_value.value();
            return true;
        }

        return false;
    }

    bool erase(exlib::string key)
    {
        m_lock.lock();

        auto it = find_map(key);
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

    void set_timeout(int64_t idleTimeout, int64_t updateTimeout)
    {
        m_lock.lock();
        m_idleTimeout = idleTimeout > 0 ? idleTimeout : updateTimeout;
        m_updateTimeout = updateTimeout;
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
    std::unordered_map<exlib::string, obj_ptr<CacheItem>>::iterator find_map(exlib::string key)
    {
        auto it = m_map.find(key);
        if (it == m_map.end() || m_updateTimeout <= 0)
            return it;

        CacheItem* item = it->second;
        std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
        if (std::chrono::duration_cast<std::chrono::milliseconds>(now - item->m_updated).count() >= m_updateTimeout * 1000) {
            erase_item(item);
            return m_map.end();
        }

        return it;
    }

    void erase_item(CacheItem* item)
    {
        if (item->m_map_iter != m_map.end()) {
            m_used.erase(item->m_used_iter);
            m_expired.erase(item->m_expired_iter);

            m_map.erase(item->m_map_iter);
            item->m_map_iter = m_map.end();
        }
    }

    bool find_item(exlib::string key, obj_ptr<CacheItem>& item, bool alloc)
    {
        auto itr = find_map(key);
        if (itr != m_map.end()) {
            item = itr->second;

            if (item->m_used_iter != m_used.begin()) {
                m_used.erase(item->m_used_iter);
                item->m_used_iter = m_used.insert(m_used.begin(), item);
            }

            item->m_visited = std::chrono::system_clock::now();
            if (item->m_expired_iter != m_expired.begin()) {
                m_expired.erase(item->m_expired_iter);
                item->m_expired_iter = m_expired.insert(m_expired.begin(), item);
            }

            return false;
        }

        if (!alloc)
            return false;

        item = new CacheItem();

        item->m_map_iter = m_map.emplace(key, item).first;
        item->m_used_iter = m_used.insert(m_used.begin(), item);

        item->m_updated = item->m_visited = std::chrono::system_clock::now();
        item->m_expired_iter = m_expired.insert(m_expired.begin(), item);

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
        if (m_idleTimeout <= 0)
            return;

        std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
        while (!m_expired.empty()) {
            CacheItem* item = m_expired.back();
            if (std::chrono::duration_cast<std::chrono::milliseconds>(now - item->m_visited).count() < m_idleTimeout * 1000)
                break;

            erase_item(item);
        }
    }

private:
    Resolver m_resolver;
    std::unordered_map<exlib::string, obj_ptr<CacheItem>> m_map;
    std::list<CacheItem*> m_used;
    std::list<CacheItem*> m_expired;

    int32_t m_size;
    int64_t m_idleTimeout;
    int64_t m_updateTimeout;

    exlib::spinlock m_lock;
};

} // namespace fibjs
