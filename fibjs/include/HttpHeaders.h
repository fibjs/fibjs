/*
 * HttpHeaders.h
 *
 *  Created on: Jul 11, 2025
 *      Author: lion
 */

#pragma once

#include "ifs/HttpHeaders.h"
#include "HttpCollectionTmpl.h"

namespace fibjs {

class HttpHeaders : public HttpCollectionTmpl<HttpHeaders_base> {
public:
    HttpHeaders()
        : HttpCollectionTmpl<HttpHeaders_base>(true)
    {
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

    result_t init(HttpHeaders_base* init)
    {
        HttpHeaders* _init = static_cast<HttpHeaders*>(init);
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
};

}
