/*
 * function.cpp
 *
 *  Created on: Aug 9, 2021
 *      Author: lion
 */

#include "object.h"

namespace fibjs {

v8::Local<v8::Value> JSFunction::Call(v8::Local<v8::Context> context, v8::Local<v8::Value> recv,
    int argc, v8::Local<v8::Value> argv[], bool async)
{
    v8::Local<v8::Value> result = (*this)->Call(context, recv, argc, argv).FromMaybe(v8::Local<v8::Value>());
    if (result.IsEmpty())
        return result;

    if (async || (*this)->IsAsyncFunction()) {
        Isolate* isolate = Isolate::current(context);
        return isolate->await(result);
    }

    return result;
}
}