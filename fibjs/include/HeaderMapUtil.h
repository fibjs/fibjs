/*
 * HeaderMapUtil.h
 *
 * Shared iteration for the typed IDL HeaderMap: one callback call per header
 * line, in insertion order. An array value yields one call per element; every
 * other alternative is rendered through its Variant string form, which is what
 * the HTTP layer writes on the wire.
 */

#pragma once

#include <type_traits>
#include <vector>

#include "ifs/MapTypes.h"

namespace fibjs {

template <typename Fn>
inline void for_each_header_value(const HeaderMap& headers, Fn&& fn)
{
    for (auto& item : headers) {
        if (auto values = std::get_if<std::vector<exlib::string>>(&item.second)) {
            for (auto& value : *values)
                fn(item.first, value);
            continue;
        }

        Variant v;
        std::visit([&](auto& value) {
            using T = std::decay_t<decltype(value)>;
            if constexpr (!std::is_same_v<T, std::vector<exlib::string>>)
                v = Variant(value);
        }, item.second);

        exlib::string value;
        v.toString(value);
        fn(item.first, value);
    }
}

} // namespace fibjs
