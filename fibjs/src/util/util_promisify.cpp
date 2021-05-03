/*
 * util_promisify.cpp
 *
 *  Created on: Apr 1, 2021
 *      Author: lion
 */

#include "object.h"
#include "ifs/util.h"

namespace fibjs {

static void promisify_callback(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Object> _data = v8::Local<v8::Object>::Cast(args.Data());

    v8::Local<v8::Promise::Resolver> resolver = v8::Local<v8::Promise::Resolver>::Cast(_data);

    int32_t len = args.Length();

    if (len > 0) {
        if (!args[0].IsEmpty() && !args[0]->IsUndefined() && !args[0]->IsNull()) {
            resolver->Reject(isolate->GetCurrentContext(), args[0]);
            return;
        }
    }

    if (len > 1)
        resolver->Resolve(isolate->GetCurrentContext(), args[1]);
    else
        resolver->Resolve(isolate->GetCurrentContext(), v8::Undefined(isolate));
}

static void promisify_stub(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    Isolate* isolate = Isolate::current();
    std::vector<v8::Local<v8::Value>> argv;

    int32_t len = args.Length();
    int32_t i;

    argv.resize(len + 1);
    for (i = 0; i < len; i++)
        argv[i] = args[i];

    v8::Local<v8::Promise::Resolver> resolver;
    v8::Promise::Resolver::New(isolate->context()).ToLocal(&resolver);

    argv[i] = isolate->NewFunction("promisify_callback", promisify_callback, resolver);
    if (argv[i].IsEmpty()) {
        ThrowError("function alloc error.");
        return;
    }

    v8::Local<v8::Function> func = v8::Local<v8::Function>::Cast(args.Data());

    exlib::string str("util.promisify(");
    str += isolate->toString(func->GetName());
    str += ")";

    METHOD_NAME(str.c_str());

    v8::Local<v8::Value> result = func->Call(args.This(), (int32_t)argv.size(), argv.data());
    if (result.IsEmpty())
        return;

    args.GetReturnValue().Set(resolver->GetPromise());
}

result_t util_base::promisify(v8::Local<v8::Function> func, v8::Local<v8::Function>& retVal)
{
    Isolate* isolate = Isolate::current();
    v8::Local<v8::Function> func1;

    func1 = isolate->NewFunction("promisify", promisify_stub, func);
    if (func1.IsEmpty())
        return CHECK_ERROR(Runtime::setError("function alloc error."));

    retVal = func1;

    return 0;
}
}