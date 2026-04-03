/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#pragma once

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs {

class Buffer_base;

class StreamReader_base : public object_base {
    DECLARE_CLASS(StreamReader_base);

public:
    class ReadType : public NType {
    public:
        virtual void to_value(Isolate* isolate, v8::Local<v8::Object>& retVal)
        {
            v8::Local<v8::Context> context = retVal->GetCreationContextChecked();
            retVal->Set(context, isolate->NewString("done"), GetReturnValue(isolate, done)).Check();
            retVal->Set(context, isolate->NewString("value"), GetReturnValue(isolate, value)).Check();
        }

        virtual void to_args(Isolate* isolate, std::vector<v8::Local<v8::Value>>& args)
        {
            args.push_back(GetReturnValue(isolate, done));
            args.push_back(GetReturnValue(isolate, value));
        }

    public:
        bool done;
        obj_ptr<Buffer_base> value;
    };

public:
    // StreamReader_base
    virtual result_t read(obj_ptr<ReadType>& retVal, AsyncEvent* ac) = 0;
    virtual result_t releaseLock() = 0;
    virtual result_t cancel(exlib::string reason, AsyncEvent* ac) = 0;
    virtual result_t get_closed(v8::Local<v8::Promise>& retVal) = 0;

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        ThrowTypeError("not a constructor");
    }

    static result_t load(v8::Local<v8::Value> v, obj_ptr<StreamReader_base>& retVal)
    { return CALL_E_TYPEMISMATCH; }

public:
    static void s_read(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_releaseLock(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_cancel(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_closed(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
    ASYNC_MEMBERVALUE1(StreamReader_base, read, obj_ptr<ReadType>);
    ASYNC_MEMBER1(StreamReader_base, cancel, exlib::string);
};
}

#include "ifs/Buffer.h"

namespace fibjs {
inline ClassInfo& StreamReader_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "read", s_read, false, ClassData::ASYNC_PROMISE },
        { "releaseLock", s_releaseLock, false, ClassData::ASYNC_SYNC },
        { "cancel", s_cancel, false, ClassData::ASYNC_PROMISE }
    };

    static ClassData::ClassProperty s_property[] = {
        { "closed", s_get_closed, block_set, false }
    };

    static ClassData s_cd = {
        "StreamReader", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &object_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void StreamReader_base::s_read(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<ReadType> vr;

    ASYNC_METHOD_INSTANCE(StreamReader_base);
    ASYNC_METHOD_ENTER("StreamReader.read");

    METHOD_OVER(0, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_read(cb, args);
    else
        hr = pInst->ac_read(vr);

    ASYNC_METHOD_RETURN();
}

inline void StreamReader_base::s_releaseLock(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(StreamReader_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->releaseLock();

    METHOD_VOID();
}

inline void StreamReader_base::s_cancel(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ASYNC_METHOD_INSTANCE(StreamReader_base);
    ASYNC_METHOD_ENTER("StreamReader.cancel");

    METHOD_OVER(1, 0);

    OPT_ARG(exlib::string, 0, "");

    if (!cb.IsEmpty())
        hr = pInst->acb_cancel(v0, cb, args);
    else
        hr = pInst->ac_cancel(v0);

    ASYNC_METHOD_VOID();
}

inline void StreamReader_base::s_get_closed(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Promise> vr;

    METHOD_INSTANCE(StreamReader_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_closed(vr);

    METHOD_RETURN();
}
}
