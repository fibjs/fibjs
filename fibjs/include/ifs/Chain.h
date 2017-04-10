/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _Chain_base_H_
#define _Chain_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "Handler.h"

namespace fibjs {

class Handler_base;

class Chain_base : public Handler_base {
    DECLARE_CLASS(Chain_base);

public:
    // Chain_base
    static result_t _new(v8::Local<v8::Array> hdlrs, obj_ptr<Chain_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    virtual result_t append(v8::Local<v8::Array> hdlrs) = 0;
    virtual result_t append(v8::Local<v8::Value> hdlr) = 0;

public:
    template <typename T>
    static void __new(const T& args);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_append(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

namespace fibjs {
inline ClassInfo& Chain_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "append", s_append, false }
    };

    static ClassData s_cd = {
        "Chain", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, 0, NULL, NULL, NULL,
        &Handler_base::class_info()
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void Chain_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    CONSTRUCT_INIT();
    __new(args);
}

template <typename T>
void Chain_base::__new(const T& args)
{
    obj_ptr<Chain_base> vr;

    CONSTRUCT_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Array>, 0);

    hr = _new(v0, vr, args.This());

    CONSTRUCT_RETURN();
}

inline void Chain_base::s_append(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(Chain_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Array>, 0);

    hr = pInst->append(v0);

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = pInst->append(v0);

    METHOD_VOID();
}
}

#endif
