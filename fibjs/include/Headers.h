/*
 * Headers.h
 *
 *  Created on: Jul 11, 2025
 *      Author: lion
 */

#pragma once

#include "ifs/Headers.h"
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

public:
    result_t get(exlib::string name, Variant& retVal)
    {
        if (name.empty())
            return CALL_E_INVALIDARG;

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
            return CALL_RETURN_NULL;

        retVal = result;
        return 0;
    }

    result_t init(Headers_base* init)
    {
        Headers* _init = static_cast<Headers*>(init);
        m_map.insert(m_map.end(), _init->m_map.begin(), _init->m_map.end());

        return 0;
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
        if (ret == 0) {
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

            exlib::string v = _pair.second.string();

            cp(buf, sz, pos, n.c_str(), n.length());
            cp(buf, sz, pos, ": ", 2);
            cp(buf, sz, pos, v.c_str(), v.length());
            cp(buf, sz, pos, "\r\n", 2);
        }

        return pos;
    }
};

}
