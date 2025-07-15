/*
 * Headers.cpp
 *
 *  Created on: Jul 11, 2025
 *      Author: lion
 */

#include "object.h"
#include "Headers.h"

namespace fibjs {

result_t Headers_base::_new(obj_ptr<Headers_base>& retVal, v8::Local<v8::Object> This)
{
    retVal = new Headers();
    return 0;
}

result_t Headers_base::_new(v8::Local<v8::Object> init, obj_ptr<Headers_base>& retVal, v8::Local<v8::Object> This)
{
    retVal = new Headers();
    return retVal->append(init);
}

result_t Headers_base::_new(v8::Local<v8::Array> init, obj_ptr<Headers_base>& retVal, v8::Local<v8::Object> This)
{
    retVal = new Headers();
    return retVal->append(init);
}

result_t Headers_base::_new(Headers_base* init, obj_ptr<Headers_base>& retVal, v8::Local<v8::Object> This)
{
    obj_ptr<Headers> headers = new Headers();
    retVal = headers;
    return headers->init(init);
}

}