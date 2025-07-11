/*
 * HttpHeaders.cpp
 *
 *  Created on: Jul 11, 2025
 *      Author: lion
 */

#include "object.h"
#include "HttpHeaders.h"

namespace fibjs {

result_t HttpHeaders_base::_new(obj_ptr<HttpHeaders_base>& retVal, v8::Local<v8::Object> This)
{
    retVal = new HttpHeaders();
    return 0;
}

result_t HttpHeaders_base::_new(v8::Local<v8::Object> init, obj_ptr<HttpHeaders_base>& retVal, v8::Local<v8::Object> This)
{
    retVal = new HttpHeaders();
    return retVal->append(init);
}

result_t HttpHeaders_base::_new(v8::Local<v8::Array> init, obj_ptr<HttpHeaders_base>& retVal, v8::Local<v8::Object> This)
{
    retVal = new HttpHeaders();
    return retVal->append(init);
}

result_t HttpHeaders_base::_new(HttpHeaders_base* init, obj_ptr<HttpHeaders_base>& retVal, v8::Local<v8::Object> This)
{
    obj_ptr<HttpHeaders> headers = new HttpHeaders();
    retVal = headers;
    return headers->init(init);
}

}