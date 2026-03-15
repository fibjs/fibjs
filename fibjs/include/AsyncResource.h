/*
 * AsyncResource.h
 *
 *  Created on: Mar 16, 2026
 *      Author: lion
 */

#pragma once

#include "ifs/AsyncResource.h"

namespace fibjs {

class AsyncResource : public AsyncResource_base {
public:
    AsyncResource(double asyncId, double triggerAsyncId)
        : m_asyncId(asyncId)
        , m_triggerAsyncId(triggerAsyncId)
    {
    }

public:
    // AsyncResource_base
    virtual result_t asyncId(double& retVal);
    virtual result_t triggerAsyncId(double& retVal);
    virtual result_t runInAsyncScope(v8::Local<v8::Function> fn, v8::Local<v8::Value> thisArg, OptArgs args, v8::Local<v8::Value>& retVal);
    virtual result_t emitDestroy(obj_ptr<AsyncResource_base>& retVal);
    virtual result_t bind(v8::Local<v8::Function> fn, v8::Local<v8::Value> thisArg, v8::Local<v8::Function>& retVal);

public:
    double m_asyncId;
    double m_triggerAsyncId;
    v8::Global<v8::Map> m_asyncContext;
};

}
