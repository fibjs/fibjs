/*
 * OrderedMap.h
 *
 * A small insertion-ordered map used by the IDL `Map<T>` / `map Name: T`
 * types. The JavaScript side is a plain object, whose property enumeration is
 * part of the API contract (header order, listener order, routing order), so
 * std::map (sorted) and std::unordered_map (unordered) cannot be used.
 *
 * This is deliberately not a JavaScript object: the converted map lives only
 * on the JS thread and is built from a plain object by GetArgumentValue before
 * the native implementation runs.
 */

#pragma once

#include <utility>
#include <vector>

#include "qstring.h"

namespace fibjs {

template <typename T>
class OrderedMap {
public:
    using value_type = std::pair<exlib::string, T>;
    using container_type = std::vector<value_type>;
    using iterator = typename container_type::iterator;
    using const_iterator = typename container_type::const_iterator;

public:
    void set(const exlib::string& key, T value)
    {
        for (auto& item : m_items) {
            if (item.first == key) {
                item.second = std::move(value);
                return;
            }
        }
        m_items.emplace_back(key, std::move(value));
    }

    bool has(const exlib::string& key) const
    {
        return find(key) != nullptr;
    }

    T* find(const exlib::string& key)
    {
        for (auto& item : m_items)
            if (item.first == key)
                return &item.second;
        return nullptr;
    }

    const T* find(const exlib::string& key) const
    {
        for (const auto& item : m_items)
            if (item.first == key)
                return &item.second;
        return nullptr;
    }

    T& operator[](const exlib::string& key)
    {
        T* value = find(key);
        if (value)
            return *value;

        m_items.emplace_back(key, T());
        return m_items.back().second;
    }

    void erase(const exlib::string& key)
    {
        for (auto it = m_items.begin(); it != m_items.end(); ++it) {
            if (it->first == key) {
                m_items.erase(it);
                return;
            }
        }
    }

    size_t size() const
    {
        return m_items.size();
    }

    bool empty() const
    {
        return m_items.empty();
    }

    void clear()
    {
        m_items.clear();
    }

    // Some IDL maps (util.parseEnv) are specified as null-prototype objects.
    // The flag only affects the JS object GetReturnValue builds; conversion
    // from JS has no prototype requirement.
    void set_null_prototype(bool value = true)
    {
        m_null_prototype = value;
    }

    bool null_prototype() const
    {
        return m_null_prototype;
    }

    iterator begin()
    {
        return m_items.begin();
    }

    iterator end()
    {
        return m_items.end();
    }

    const_iterator begin() const
    {
        return m_items.begin();
    }

    const_iterator end() const
    {
        return m_items.end();
    }

private:
    container_type m_items;
    bool m_null_prototype = false;
};

} // namespace fibjs
