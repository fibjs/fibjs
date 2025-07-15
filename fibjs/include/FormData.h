/*
 * FormData.h
 *
 *  Created on: Jul 14, 2025
 *      Author: lion
 */

#pragma once

#include "ifs/FormData.h"
#include "HttpCollectionTmpl.h"

namespace fibjs {

class FormData : public HttpCollectionTmpl<FormData_base, true> {
public:
    FormData()
        : HttpCollectionTmpl<FormData_base, true>(true)
    {
    }

public:
    // FormData_base
    virtual result_t append(exlib::string name, Blob_base* value);
    virtual result_t append(exlib::string name, Blob_base* value, exlib::string filename);
    virtual result_t set(exlib::string name, Blob_base* value);
    virtual result_t set(exlib::string name, Blob_base* value, exlib::string filename);

public:
    result_t init(FormData_base* init)
    {
        FormData* _init = static_cast<FormData*>(init);
        m_map.insert(m_map.end(), _init->m_map.begin(), _init->m_map.end());

        return 0;
    }
};

}
