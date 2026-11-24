/*
 * LruCache.h
 *
 *  Created on: May 27, 2024
 *      Author: lion
 */

#pragma once

#include "obj_ptr.h"
#include <atomic>
#include <list>
#include <chrono>
#include <memory>
#include <unordered_map>
#include <mutex>
#include <optional>

namespace fibjs {

template <typename T>
class LruCache {
private:
    typedef std::chrono::steady_clock cache_clock;

    class CacheItem : public obj_base {
    public:
        std::optional<T> m_value;
        // Only an entry that a resolver left pending is visible to waiters:
        // set() fills the value under the lock before the entry can be found,
        // so it needs no event at all.
        std::unique_ptr<exlib::Event> m_ready;

        typename std::unordered_map<exlib::string, obj_ptr<CacheItem>>::iterator m_map_iter;
        typename std::list<CacheItem*>::iterator m_used_iter;

        // Both time points are valid from insertion; only m_visited is
        // refreshed on a hit, and only while the idle timeout is enabled.
        cache_clock::time_point m_visited;
        cache_clock::time_point m_updated;
    };

public:
    typedef std::function<bool(exlib::string&, T&)> Resolver;

public:
    LruCache(int32_t size = 1024, int64_t idleTimeout = 300, int64_t updateTimeout = 300)
        : m_size(normalize_size(size))
        , m_idleTimeout(idleTimeout > 0 ? idleTimeout : updateTimeout)
        , m_updateTimeout(updateTimeout)
    {
    }

    LruCache(Resolver resolver, int32_t size = 1024, int64_t idleTimeout = 300, int64_t updateTimeout = 300)
        : m_resolver(resolver)
        , m_has_resolver((bool)resolver)
        , m_size(normalize_size(size))
        , m_idleTimeout(idleTimeout > 0 ? idleTimeout : updateTimeout)
        , m_updateTimeout(updateTimeout)
    {
    }

public:
    void set(const exlib::string& key, T value)
    {
        std::lock_guard<exlib::spinlock> lock(m_lock);

        obj_ptr<CacheItem> item;
        find_item(key, item, true);

        try {
            item->m_value = value;
        } catch (...) {
            // A pending item has to leave the map when it cannot be filled, or
            // later lookups would wait on an event that is never set.
            erase_item(item);
            throw;
        }

        // An entry waiting for a resolver can be published early; entries that
        // were just created have no event (no one can find them until unlock).
        if (item->m_ready)
            item->m_ready->set();
    }

    bool lookup(const exlib::string& key, T& value, bool auto_resolve = true)
    {
        // Serving a complete entry needs neither the resolver nor the release
        // of the lock, so the hot path avoids copying the std::function (and
        // the pending-entry wait machinery) once per lookup.
        {
            std::lock_guard<exlib::spinlock> lock(m_lock);
            obj_ptr<CacheItem> item;

            clean_expired();

            if (find_item(key, item, false) && item->m_value.has_value()) {
                value = item->m_value.value();
                return true;
            }
        }

        if (!auto_resolve || !m_has_resolver.load(std::memory_order_relaxed))
            return lookup(key, value, Resolver());

        Resolver resolver;
        {
            std::lock_guard<exlib::spinlock> lock(m_lock);
            resolver = m_resolver;
        }

        return lookup(key, value, resolver);
    }

    // 契约：resolver 返回 false 时仅放弃缓存（条目会被移除），且**不保证回填 `value`**
    // ——同一 key 的并发等待者在该分支下取不到值。
    // 现有调用点均采用「调用方过滤」（只读查 → 计算 → 成功才 set），不依赖 false 分支；
    // 若要启用该分支，必须先修等待者语义并补测试。
    //
    // The resolver runs outside the lock, while the entry it was invoked for
    // stays in the map as pending: concurrent lookups wait on its event instead
    // of resolving the same key again. Every read and write of m_value happens
    // under m_lock, so a concurrent set() cannot race with a reader's copy.
    bool lookup(const exlib::string& key, T& value, Resolver resolver)
    {
        obj_ptr<CacheItem> item;
        bool alloced = false;

        {
            std::lock_guard<exlib::spinlock> lock(m_lock);

            clean_expired();

            alloced = find_item(key, item, (bool)resolver);
            if (item == nullptr)
                return false;

            if (alloced) {
                // The entry is about to be visible to waiters while the
                // resolver runs outside the lock, so this is the only case
                // that needs the event.
                item->m_ready.reset(new exlib::Event());
            } else if (item->m_value.has_value()) {
                // Complete entry: copy under the lock so a concurrent set()
                // cannot race with the copy.
                value = item->m_value.value();
                return true;
            }
        }

        if (alloced) {
            bool ok = false;
            // The resolver signature takes the key by non-const reference
            // (it may normalize it), so the shared lookup parameter is copied
            // for the call.
            exlib::string resolve_key(key);

            try {
                ok = resolver(resolve_key, value);
            } catch (...) {
                ok = false;
            }

            std::lock_guard<exlib::spinlock> lock(m_lock);

            if (ok) {
                bool stored = false;

                try {
                    item->m_value = value;
                    stored = true;
                } catch (...) {
                }

                if (item->m_ready)
                    item->m_ready->set();

                // A stored value stays in the map; an allocation failure
                // removes the pending item so a later lookup can retry.
                if (stored)
                    return true;
            } else if (item->m_ready)
                item->m_ready->set();

            erase_item(item);
            return false;
        }

        // Another resolver owns this pending entry: wait for it to publish
        // (or discard) the value.
        if (item->m_ready)
            item->m_ready->wait();

        std::lock_guard<exlib::spinlock> lock(m_lock);
        if (item->m_value.has_value()) {
            value = item->m_value.value();
            return true;
        }

        return false;
    }

    bool erase(const exlib::string& key)
    {
        std::lock_guard<exlib::spinlock> lock(m_lock);

        auto it = find_map(key);
        if (it == m_map.end())
            return false;

        erase_item(it->second);
        return true;
    }

    void set_resolver(Resolver resolver)
    {
        std::lock_guard<exlib::spinlock> lock(m_lock);
        m_resolver = resolver;
        m_has_resolver.store((bool)resolver, std::memory_order_relaxed);
    }

    void resize(int32_t new_size)
    {
        std::lock_guard<exlib::spinlock> lock(m_lock);
        m_size = normalize_size(new_size);
        clean_used();
    }

    void set_timeout(int64_t idleTimeout, int64_t updateTimeout)
    {
        std::lock_guard<exlib::spinlock> lock(m_lock);

        int64_t new_idle = idleTimeout > 0 ? idleTimeout : updateTimeout;

        // While idle expiry is off, hits do not refresh m_visited, so the list
        // order (last access) and m_visited stop agreeing. Re-enabling starts
        // every entry from the same instant, which keeps clean_expired()'s
        // "least recently visited is at the back" walk valid.
        if (m_idleTimeout <= 0 && new_idle > 0) {
            cache_clock::time_point now = cache_clock::now();

            for (CacheItem* item : m_used)
                item->m_visited = now;
        }

        m_idleTimeout = new_idle;
        m_updateTimeout = updateTimeout;
        clean_expired();
    }

    void clear()
    {
        std::lock_guard<exlib::spinlock> lock(m_lock);

        while (!m_used.empty())
            erase_item(m_used.back());
    }

    // fn runs with the cache locked and must not call back into the cache.
    template <typename Fn>
    void forEach(Fn fn)
    {
        std::lock_guard<exlib::spinlock> lock(m_lock);

        for (auto& kv : m_map) {
            if (kv.second->m_value.has_value())
                fn(kv.first, kv.second->m_value.value());
        }
    }

private:
    static int32_t normalize_size(int32_t size)
    {
        // A negative capacity used to compare as a huge size_t and silently
        // disabled eviction; zero is the smallest meaningful cache.
        return size > 0 ? size : 0;
    }

    typename std::unordered_map<exlib::string, obj_ptr<CacheItem>>::iterator find_map(const exlib::string& key)
    {
        auto it = m_map.find(key);
        if (it == m_map.end() || m_updateTimeout <= 0)
            return it;

        CacheItem* item = it->second;
        if (std::chrono::duration_cast<std::chrono::milliseconds>(cache_clock::now() - item->m_updated).count() >= m_updateTimeout * 1000) {
            erase_item(item);
            return m_map.end();
        }

        return it;
    }

    void erase_item(CacheItem* item)
    {
        if (item->m_map_iter != m_map.end()) {
            m_used.erase(item->m_used_iter);

            // Write the sentinel BEFORE erasing from the map: the map holds
            // the only reference to the item, so m_map.erase() may free it.
            // Touching item->m_map_iter after that would be a use-after-free.
            auto mit = item->m_map_iter;
            item->m_map_iter = m_map.end();
            m_map.erase(mit);
        }
    }

    bool find_item(const exlib::string& key, obj_ptr<CacheItem>& item, bool alloc)
    {
        auto itr = find_map(key);
        if (itr != m_map.end()) {
            item = itr->second;

            // splice keeps m_used_iter valid, so the iterator does not have to
            // be refreshed after the move to the front.
            if (item->m_used_iter != m_used.begin())
                m_used.splice(m_used.begin(), m_used, item->m_used_iter);

            if (m_idleTimeout > 0)
                item->m_visited = cache_clock::now();

            return false;
        }

        if (!alloc)
            return false;

        item = new CacheItem();

        item->m_map_iter = m_map.emplace(key, item).first;
        m_used.push_front(item);
        item->m_used_iter = m_used.begin();

        // Both timestamps are set on insertion even when the corresponding
        // timeout is disabled: set_timeout() may enable one later, and
        // clean_expired()/find_map() must not read an uninitialized point.
        item->m_visited = item->m_updated = cache_clock::now();

        clean_used();

        return true;
    }

    void clean_used()
    {
        while (!m_used.empty() && m_used.size() > (size_t)m_size)
            erase_item(m_used.back());
    }

    void clean_expired()
    {
        if (m_idleTimeout <= 0)
            return;

        cache_clock::time_point now = cache_clock::now();

        // m_used is ordered by access; the back is the least recently used and
        // therefore also the least recently visited.
        while (!m_used.empty()) {
            CacheItem* item = m_used.back();
            if (std::chrono::duration_cast<std::chrono::milliseconds>(now - item->m_visited).count() < m_idleTimeout * 1000)
                break;

            erase_item(item);
        }
    }

private:
    Resolver m_resolver;
    std::atomic<bool> m_has_resolver { false };
    std::unordered_map<exlib::string, obj_ptr<CacheItem>> m_map;
    std::list<CacheItem*> m_used;

    int32_t m_size;
    int64_t m_idleTimeout;
    int64_t m_updateTimeout;

    exlib::spinlock m_lock;
};

} // namespace fibjs
