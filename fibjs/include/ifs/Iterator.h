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

class Iterator_base : public object_base {
    DECLARE_CLASS(Iterator_base);

public:
    class NextType : public NType {
    public:
        virtual void fillMembers(Isolate* isolate, v8::Local<v8::Object>& retVal)
        {
            v8::Local<v8::Context> context = retVal->GetCreationContextChecked();
            retVal->Set(context, isolate->NewString("value"), GetReturnValue(isolate, value)).Check();
            retVal->Set(context, isolate->NewString("done"), GetReturnValue(isolate, done)).Check();
        }

        virtual void fillArguments(Isolate* isolate, std::vector<v8::Local<v8::Value>>& args)
        {
            args.push_back(GetReturnValue(isolate, value));
            args.push_back(GetReturnValue(isolate, done));
        }

    public:
        v8::Local<v8::Value> value;
        bool done;
    };

public:
    // Iterator_base
    virtual result_t symbol_iterator(obj_ptr<Iterator_base>& retVal) = 0;
    virtual result_t next(obj_ptr<NextType>& retVal) = 0;

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_symbol_iterator(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_next(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

namespace fibjs {
inline ClassInfo& Iterator_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "@iterator", s_symbol_iterator, false, false },
        { "next", s_next, false, false }
    };

    static ClassData s_cd = {
        "Iterator", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, 0, NULL, 0, NULL, NULL, NULL,
        &object_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void Iterator_base::s_symbol_iterator(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Iterator_base> vr;

    METHOD_INSTANCE(Iterator_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->symbol_iterator(vr);

    METHOD_RETURN();
}

inline void Iterator_base::s_next(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<NextType> vr;

    METHOD_INSTANCE(Iterator_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->next(vr);

    METHOD_RETURN();
}
}
