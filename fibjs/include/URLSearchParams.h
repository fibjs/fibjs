/*
 * URLSearchParams.h
 *
 *  Created on: Jul 14, 2025
 *      Author: lion
 */

#pragma once

#include "ifs/URLSearchParams.h"
#include "HttpCollectionTmpl.h"

namespace fibjs {

class URLSearchParams : public HttpCollectionTmpl<URLSearchParams_base, true> {
public:
    URLSearchParams()
        : HttpCollectionTmpl<URLSearchParams_base, true>(true)
    {
    }

public:
    result_t toString(exlib::string& retVal);
    result_t has(exlib::string name, v8::Local<v8::Value> value, bool& retVal);
    result_t _delete(exlib::string name, v8::Local<v8::Value> value);

public:
    result_t init(URLSearchParams_base* init)
    {
        URLSearchParams* _init = static_cast<URLSearchParams*>(init);
        m_map.insert(m_map.end(), _init->m_map.begin(), _init->m_map.end());

        return 0;
    }
};

}
