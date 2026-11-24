/*
 * Headers.h
 *
 *  Created on: Jul 11, 2025
 *      Author: lion
 */

#pragma once

#include "ifs/Headers.h"
#include "HeaderMapUtil.h"
#include "HttpCollectionTmpl.h"

namespace fibjs {

class Headers : public HttpCollectionTmpl<Headers_base> {
public:
    Headers()
        : HttpCollectionTmpl<Headers_base>(true)
    {
        // Fetch API spec: header names must always be stored lowercase
        m_lowercase_keys = true;
    }

    // append(HeaderMap&) below hides the base overloads; keep them visible for
    // append_string() and callers that pass a single name.
    using HttpCollectionTmpl<Headers_base>::append;
    using HttpCollectionTmpl<Headers_base>::set;

public:
    result_t get(exlib::string name, Variant& retVal)
    {
        if (name.empty())
            return result_t::invalid_arg();

        exlib::string result;
        bool found = false;

        for (size_t i = 0; i < m_map.size(); i++) {
            pair& _pair = m_map[i];

            if (!qstricmp(_pair.first.c_str(), name.c_str())) {
                if (found)
                    result += ", ";

                exlib::string s;
                _pair.second.toString(s);
                result += s;

                found = true;
            }
        }

        if (!found)
            return result_t::return_null();

        retVal = result;
        return result_t();
    }

    result_t init(Headers_base* init)
    {
        Headers* _init = static_cast<Headers*>(init);
        m_map.insert(m_map.end(), _init->m_map.begin(), _init->m_map.end());

        return result_t();
    }

    // Typed IDL HeaderMap input. Values are converted by the same rules as the
    // single-name append: an array appends one line per element and a scalar is
    // stored as its Variant (Headers itself stringifies on write).
    result_t append(HeaderMap& map)
    {
        result_t hr;

        for_each_header_value(map, [&](const exlib::string& name, const exlib::string& value) {
            if (hr.ok())
                hr = HttpCollectionTmpl<Headers_base>::append(name, Variant(value));
        });

        return hr;
    }

    result_t set(HeaderMap& map)
    {
        for (auto& item : map)
            remove(item.first);
        return append(map);
    }

public:
    void append_string(const char* name, int32_t szName, const char* value, int32_t szValue)
    {
        append(exlib::string(name, szName), exlib::string(value, szValue));
    }

    result_t first_string(exlib::string name, exlib::string& retVal)
    {
        Variant v;
        result_t ret = first(name, v);
        if (ret.has_value()) {
            v.toString(retVal);
        }
        return ret;
    }

public:
    size_t size()
    {
        size_t sz = 0;

        for (size_t i = 0; i < m_map.size(); i++) {
            pair& _pair = m_map[i];
            exlib::string& n = _pair.first;

            // skip headers managed by HttpMessage::getData
            if (!qstricmp(n.c_str(), "content-length")
                || !qstricmp(n.c_str(), "connection"))
                continue;

            sz += n.length() + _pair.second.string().length() + 4;
        }

        return sz;
    }

    void cp(char* buf, size_t sz, size_t& pos, const char* str, size_t szStr)
    {
        buf += pos;

        pos += szStr;
        if (pos > sz) {
            szStr -= pos - sz;
            pos = sz;
        }

        memcpy(buf, str, szStr);
    }

    size_t getData(char* buf, size_t sz)
    {
        size_t pos = 0;

        for (size_t i = 0; i < m_map.size(); i++) {
            pair& _pair = m_map[i];
            exlib::string& n = _pair.first;

            // skip headers managed by HttpMessage::getData
            if (!qstricmp(n.c_str(), "content-length")
                || !qstricmp(n.c_str(), "connection"))
                continue;

            exlib::string name = n;
            exlib::string v = _pair.second.string();

            // The public setters reject these characters; sanitize here as a
            // last line of defence for values assembled through internal paths.
            for (size_t j = 0; j < name.length(); j++)
                if (name[j] == '\r' || name[j] == '\n' || name[j] == '\0')
                    name[j] = ' ';
            for (size_t j = 0; j < v.length(); j++)
                if (v[j] == '\r' || v[j] == '\n' || v[j] == '\0')
                    v[j] = ' ';

            cp(buf, sz, pos, name.c_str(), name.length());
            cp(buf, sz, pos, ": ", 2);
            cp(buf, sz, pos, v.c_str(), v.length());
            cp(buf, sz, pos, "\r\n", 2);
        }

        return pos;
    }
};

}
