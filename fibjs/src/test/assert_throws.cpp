/*
 * assert_throws.cpp
 *
 *  Created on: Dec 8, 2024
 *      Author: lion
 */

#include "utils.h"
#include "object.h"
#include "ifs/assert.h"
#include "ifs/encoding.h"
#include "ifs/util.h"
#include "QuickArray.h"
#include "../util/util.h"
#include "assert.h"

namespace fibjs {

static bool check_error(v8::Local<v8::Value> exp, v8::Local<v8::Value> error)
{
    if (exp.IsEmpty())
        return false;

    if (error.IsEmpty())
        return true;

    Isolate* isolate = Isolate::current();
    v8::Local<v8::Context> _context = isolate->context();

    if (error->IsRegExp()) {
        v8::Local<v8::String> s = exp->ToString(_context).FromMaybe(v8::Local<v8::String>());
        v8::Local<v8::RegExp> p = error.As<v8::RegExp>();
        v8::Local<v8::Object> r = p->Exec(_context, s).FromMaybe(v8::Local<v8::Object>());
        return !r->IsNull();
    } else if (error->IsFunction()) {
        v8::Local<v8::Function> f = error.As<v8::Function>();
        if (exp->InstanceOf(_context, f).FromMaybe(false))
            return true;

        v8::Local<v8::Value> r = f->Call(_context, exp, 1, &exp).FromMaybe(v8::Local<v8::Value>());
        return !r.IsEmpty() && r->IsTrue();
    } else if (error->IsObject()) {
        v8::Local<v8::Object> o = error.As<v8::Object>();
        v8::Local<v8::Array> a = o->GetOwnPropertyNames(_context).FromMaybe(v8::Local<v8::Array>());

        v8::Local<v8::Object> o1 = exp->ToObject(_context).FromMaybe(v8::Local<v8::Object>());
        if (o1.IsEmpty())
            return false;

        for (int i = 0; i < a->Length(); i++) {
            v8::Local<v8::Value> p = a->Get(_context, i).FromMaybe(v8::Local<v8::Value>());
            v8::Local<v8::Value> v = o->Get(_context, p).FromMaybe(v8::Local<v8::Value>());
            v8::Local<v8::Value> v1 = o1->Get(_context, p).FromMaybe(v8::Local<v8::Value>());

            if (v->IsRegExp()) {
                v8::Local<v8::String> s = v1->ToString(_context).FromMaybe(v8::Local<v8::String>());
                v8::Local<v8::RegExp> p = v.As<v8::RegExp>();
                v8::Local<v8::Object> r = p->Exec(_context, s).FromMaybe(v8::Local<v8::Object>());
                if (r->IsNull())
                    return false;
            } else {
                bool r;
                util_base::isDeepStrictEqual(v, v1, r);
                if (!r)
                    return false;
            }
        }
        return true;
    }

    return true;
}

result_t assert_base::throws(v8::Local<v8::Function> block, v8::Local<v8::Value> error, exlib::string msg)
{
    Isolate* isolate = Isolate::current(block);
    v8::Local<v8::Context> _context = isolate->context();
    bool err;
    {
        TryCatch try_catch;
        block->Call(isolate->m_isolate, isolate->context(), v8::Undefined(isolate->m_isolate), 0, NULL);

        v8::Local<v8::Value> exp;
        if (try_catch.HasCaught()) {
            exp = try_catch.Exception()->ToObject(_context).FromMaybe(v8::Local<v8::Value>());
            if (exp.IsEmpty())
                exp = v8::Undefined(isolate->m_isolate);
        }

        err = check_error(exp, error);
    }
    return _test(err, _msg(msg, "Missing expected exception."));
}

result_t assert_base::throws(v8::Local<v8::Function> block, exlib::string msg)
{
    return throws(block, v8::Local<v8::Value>(), msg);
}

result_t assert_base::doesNotThrow(v8::Local<v8::Function> block, exlib::string msg)
{
    Isolate* isolate = Isolate::current(block);
    bool err;
    {
        TryCatch try_catch;
        block->Call(isolate->m_isolate, isolate->context(), v8::Undefined(isolate->m_isolate), 0, NULL);
        err = try_catch.HasCaught();
    }
    return _test(!err, _msg(msg, "Got unwanted exception."));
}

struct CallbackData {
    v8::Persistent<v8::Promise::Resolver> resolver;
    v8::Persistent<v8::Value> errorCheck;
    v8::Persistent<v8::Value> error;
};

static void OnResolve(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    Isolate* isolate = Isolate::current(args);
    CallbackData* data = static_cast<CallbackData*>(v8::External::Cast(*args.Data())->Value());
    v8::Local<v8::Context> context = isolate->context();

    data->resolver.Get(isolate->m_isolate)->Reject(context, data->error.Get(isolate->m_isolate));

    delete data;
}

static void OnReject(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    Isolate* isolate = Isolate::current(args);
    CallbackData* data = static_cast<CallbackData*>(v8::External::Cast(*args.Data())->Value());
    v8::Local<v8::Context> context = isolate->context();

    bool err_match = check_error(args[0], data->errorCheck.Get(isolate->m_isolate));

    if (err_match) {
        data->resolver.Get(isolate->m_isolate)->Resolve(context, v8::Undefined(isolate->m_isolate));
    } else {
        data->resolver.Get(isolate->m_isolate)->Reject(context, data->error.Get(isolate->m_isolate));
    }

    delete data;
}

result_t assert_base::rejects(v8::Local<v8::Promise> result, v8::Local<v8::Value> error, exlib::string msg, v8::Local<v8::Promise>& retVal)
{
    Isolate* isolate = Isolate::current(result);
    v8::Local<v8::Context> context = isolate->context();

    v8::Local<v8::Promise::Resolver> resolver = v8::Promise::Resolver::New(context).FromMaybe(v8::Local<v8::Promise::Resolver>());
    retVal = resolver->GetPromise();

    if (msg.empty())
        msg = "Missing expected rejection.";

    auto callbackData = new CallbackData {
        v8::Persistent<v8::Promise::Resolver>(isolate->m_isolate, resolver),
        v8::Persistent<v8::Value>(isolate->m_isolate, error),
        v8::Persistent<v8::Value>(isolate->m_isolate, v8::Exception::Error(isolate->NewString(msg)))
    };

    v8::Local<v8::External> external = v8::External::New(isolate->m_isolate, callbackData);

    result->Then(context,
        v8::Function::New(context, OnResolve, external).FromMaybe(v8::Local<v8::Function>()),
        v8::Function::New(context, OnReject, external).FromMaybe(v8::Local<v8::Function>()));

    return 0;
}

result_t assert_base::rejects(v8::Local<v8::Promise> result, exlib::string msg, v8::Local<v8::Promise>& retVal)
{
    return rejects(result, v8::Local<v8::Value>(), msg, retVal);
}

result_t assert_base::rejects(v8::Local<v8::Function> block, v8::Local<v8::Value> error, exlib::string msg, v8::Local<v8::Promise>& retVal)
{
    if (!block->IsAsyncFunction())
        return Runtime::setError("rejects() can only be used with async functions.");

    Isolate* isolate = Isolate::current(block);
    v8::Local<v8::Context> context = isolate->context();

    v8::Local<v8::Value> v = block->Call(isolate->m_isolate, isolate->context(), v8::Undefined(isolate->m_isolate), 0, NULL).FromMaybe(v8::Local<v8::Value>());
    if (!v->IsPromise()) {
        v8::Local<v8::Promise::Resolver> resolver = v8::Promise::Resolver::New(context).FromMaybe(v8::Local<v8::Promise::Resolver>());
        retVal = resolver->GetPromise();

        v8::Local<v8::Value> error = isolate->NewString("Missing expected rejection.");
        resolver->Reject(context, error).IsJust();
        return 0;
    }

    v8::Local<v8::Promise> result = v.As<v8::Promise>();
    return rejects(result, error, msg, retVal);
}

result_t assert_base::rejects(v8::Local<v8::Function> block, exlib::string msg, v8::Local<v8::Promise>& retVal)
{
    return rejects(block, v8::Local<v8::Value>(), msg, retVal);
}

}
