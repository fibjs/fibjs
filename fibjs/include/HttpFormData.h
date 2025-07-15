/*
 * HttpFormData.h
 *
 *  Created on: Jul 14, 2025
 *      Author: lion
 */

#pragma once

#include "ifs/HttpFormData.h"
#include "HttpCollectionTmpl.h"

namespace fibjs {

class HttpFormData : public HttpCollectionTmpl<HttpFormData_base, true> {
public:
    HttpFormData()
        : HttpCollectionTmpl<HttpFormData_base, true>(true)
    {
    }

public:
    // HttpFormData_base
    virtual result_t append(exlib::string name, Blob_base* value);
    virtual result_t append(exlib::string name, Blob_base* value, exlib::string filename);
    virtual result_t set(exlib::string name, Blob_base* value);
    virtual result_t set(exlib::string name, Blob_base* value, exlib::string filename);

public:
    result_t init(HttpFormData_base* init)
    {
        HttpFormData* _init = static_cast<HttpFormData*>(init);
        m_map.insert(m_map.end(), _init->m_map.begin(), _init->m_map.end());

        return 0;
    }
};

}
