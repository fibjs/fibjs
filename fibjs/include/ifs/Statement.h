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

class Iterator_base;

class Statement_base : public object_base {
    DECLARE_CLASS(Statement_base);

public:
    // Statement_base
    virtual result_t get(OptArgs args, Variant& retVal, AsyncEvent* ac) = 0;
    virtual result_t all(OptArgs args, obj_ptr<NArray>& retVal, AsyncEvent* ac) = 0;
    virtual result_t run(OptArgs args, Variant& retVal, AsyncEvent* ac) = 0;
    virtual result_t iterate(OptArgs args, obj_ptr<Iterator_base>& retVal, AsyncEvent* ac) = 0;
    virtual result_t columns(obj_ptr<NArray>& retVal, AsyncEvent* ac) = 0;
    virtual result_t get_sourceSQL(exlib::string& retVal) = 0;
    virtual result_t close(AsyncEvent* ac) = 0;

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        ThrowTypeError("not a constructor");
    }

    static result_t load(v8::Local<v8::Value> v, obj_ptr<Statement_base>& retVal)
    { return CALL_E_TYPEMISMATCH; }

public:
    static void s_get(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_all(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_run(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_iterate(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_columns(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_sourceSQL(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_close(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
    ASYNC_MEMBERVALUE2(Statement_base, get, OptArgs, Variant);
    ASYNC_MEMBERVALUE2(Statement_base, all, OptArgs, obj_ptr<NArray>);
    ASYNC_MEMBERVALUE2(Statement_base, run, OptArgs, Variant);
    ASYNC_MEMBERVALUE2(Statement_base, iterate, OptArgs, obj_ptr<Iterator_base>);
    ASYNC_MEMBERVALUE1(Statement_base, columns, obj_ptr<NArray>);
    ASYNC_MEMBER0(Statement_base, close);
};
}

#include "ifs/Iterator.h"

namespace fibjs {
inline ClassInfo& Statement_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "get", s_get, false, ClassData::ASYNC_ASYNC },
        { "all", s_all, false, ClassData::ASYNC_ASYNC },
        { "run", s_run, false, ClassData::ASYNC_ASYNC },
        { "iterate", s_iterate, false, ClassData::ASYNC_ASYNC },
        { "columns", s_columns, false, ClassData::ASYNC_ASYNC },
        { "close", s_close, false, ClassData::ASYNC_ASYNC }
    };

    static ClassData::ClassProperty s_property[] = {
        { "sourceSQL", s_get_sourceSQL, block_set, false }
    };

    static ClassData s_cd = {
        "Statement", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &object_base::class_info(),
        true
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void Statement_base::s_get(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    Variant vr;

    ASYNC_METHOD_INSTANCE(Statement_base);
    ASYNC_METHOD_ENTER("Statement.get");

    METHOD_OVER(-1, 0);

    ARG_LIST(0);

    if (!cb.IsEmpty())
        hr = pInst->acb_get(v0, cb, args);
    else
        hr = pInst->ac_get(v0, vr);

    ASYNC_METHOD_RETURN();
}

inline void Statement_base::s_all(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<NArray> vr;

    ASYNC_METHOD_INSTANCE(Statement_base);
    ASYNC_METHOD_ENTER("Statement.all");

    METHOD_OVER(-1, 0);

    ARG_LIST(0);

    if (!cb.IsEmpty())
        hr = pInst->acb_all(v0, cb, args);
    else
        hr = pInst->ac_all(v0, vr);

    ASYNC_METHOD_RETURN();
}

inline void Statement_base::s_run(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    Variant vr;

    ASYNC_METHOD_INSTANCE(Statement_base);
    ASYNC_METHOD_ENTER("Statement.run");

    METHOD_OVER(-1, 0);

    ARG_LIST(0);

    if (!cb.IsEmpty())
        hr = pInst->acb_run(v0, cb, args);
    else
        hr = pInst->ac_run(v0, vr);

    ASYNC_METHOD_RETURN();
}

inline void Statement_base::s_iterate(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Iterator_base> vr;

    ASYNC_METHOD_INSTANCE(Statement_base);
    ASYNC_METHOD_ENTER("Statement.iterate");

    METHOD_OVER(-1, 0);

    ARG_LIST(0);

    if (!cb.IsEmpty())
        hr = pInst->acb_iterate(v0, cb, args);
    else
        hr = pInst->ac_iterate(v0, vr);

    ASYNC_METHOD_RETURN();
}

inline void Statement_base::s_columns(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<NArray> vr;

    ASYNC_METHOD_INSTANCE(Statement_base);
    ASYNC_METHOD_ENTER("Statement.columns");

    METHOD_OVER(0, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_columns(cb, args);
    else
        hr = pInst->ac_columns(vr);

    ASYNC_METHOD_RETURN();
}

inline void Statement_base::s_get_sourceSQL(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(Statement_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_sourceSQL(vr);

    METHOD_RETURN();
}

inline void Statement_base::s_close(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ASYNC_METHOD_INSTANCE(Statement_base);
    ASYNC_METHOD_ENTER("Statement.close");

    METHOD_OVER(0, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_close(cb, args);
    else
        hr = pInst->ac_close();

    ASYNC_METHOD_VOID();
}
}
