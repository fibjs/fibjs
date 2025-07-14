/*
 * URLSearchParams.cpp
 *
 *  Created on: Jul 14, 2025
 *      Author: lion
 */

#include "object.h"
#include "URLSearchParams.h"
#include "ifs/encoding.h"
#include "StringBuffer.h"

namespace fibjs {

result_t URLSearchParams_base::_new(obj_ptr<URLSearchParams_base>& retVal, v8::Local<v8::Object> This)
{
    retVal = new URLSearchParams();
    return 0;
}

result_t URLSearchParams_base::_new(exlib::string init, obj_ptr<URLSearchParams_base>& retVal, v8::Local<v8::Object> This)
{
    obj_ptr<URLSearchParams> params = new URLSearchParams();
    retVal = params;
    const char* p = init.c_str();
    if (*p == '?')
        init = init.substr(1);
    return params->parse(init);
}

result_t URLSearchParams_base::_new(v8::Local<v8::Object> init, obj_ptr<URLSearchParams_base>& retVal, v8::Local<v8::Object> This)
{
    retVal = new URLSearchParams();
    return retVal->append(init);
}

result_t URLSearchParams_base::_new(v8::Local<v8::Array> init, obj_ptr<URLSearchParams_base>& retVal, v8::Local<v8::Object> This)
{
    retVal = new URLSearchParams();
    return retVal->append(init);
}

result_t URLSearchParams_base::_new(URLSearchParams_base* init, obj_ptr<URLSearchParams_base>& retVal, v8::Local<v8::Object> This)
{
    obj_ptr<URLSearchParams> headers = new URLSearchParams();
    retVal = headers;
    return headers->init(init);
}

result_t URLSearchParams::toString(exlib::string& retVal)
{
    StringBuffer bufs;
    
    for (size_t i = 0; i < m_map.size(); i++) {
        if (i > 0)
            bufs.append("&");
            
        pair& _pair = m_map[i];
        exlib::string key = _pair.first;
        exlib::string value = _pair.second.string();
        
        // URL encode key and value using formEncoded format
        exlib::string encodedKey, encodedValue;
        encoding_base::encodeURIComponent(key, true, encodedKey);
        encoding_base::encodeURIComponent(value, true, encodedValue);
        
        bufs.append(encodedKey);
        bufs.append("=");
        bufs.append(encodedValue);
    }

    retVal = bufs.str();
    return 0;
}

}