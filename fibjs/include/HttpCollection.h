/*
 * HttpCollection.h
 *
 *  Created on: Aug 9, 2012
 *      Author: lion
 */

#pragma once

#include "ifs/HttpCollection.h"
#include "QuickArray.h"

#include "HttpCollectionTmpl.h"

namespace fibjs {

class HttpCollection : public HttpCollectionTmpl<HttpCollection_base> {
public:
    HttpCollection(bool string_only = true)
        : HttpCollectionTmpl<HttpCollection_base>(string_only)
    {
    }

public:
    void add_string(const char* name, int32_t szName, const char* value, int32_t szValue)
    {
        add(exlib::string(name, szName), exlib::string(value, szValue));
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
    result_t parse(exlib::string& str, const char* sep = "&", const char* eq = "=");
    result_t parseCookie(exlib::string& str);
    result_t parseMultipart(exlib::string& str, const char* boundary);

public:
    size_t size()
    {
        size_t sz = 0;
        size_t i;

        for (i = 0; i < m_count; i++) {
            pair& _pair = m_map[i];
            sz += _pair.first.length() + _pair.second.string().length() + 4;
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
        size_t i;

        for (i = 0; i < m_count; i++) {
            pair& _pair = m_map[i];
            exlib::string& n = _pair.first;
            exlib::string v = _pair.second.string();

            cp(buf, sz, pos, n.c_str(), n.length());
            cp(buf, sz, pos, ": ", 2);
            cp(buf, sz, pos, v.c_str(), v.length());
            cp(buf, sz, pos, "\r\n", 2);
        }

        return pos;
    }
};

} /* namespace fibjs */
