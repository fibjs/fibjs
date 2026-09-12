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
        : HttpCollectionTmpl<HttpCollection_base>(string_only, true)
    {
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
    result_t parseCookie(exlib::string& str);
};

} /* namespace fibjs */
