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
#include "ifs/DbConnection.h"

namespace fibjs {

class DbConnection_base;

class MySQL_base : public DbConnection_base {
    DECLARE_CLASS(MySQL_base);

public:
    // MySQL_base
    virtual result_t get_rxBufferSize(int32_t& retVal) = 0;
    virtual result_t set_rxBufferSize(int32_t newVal) = 0;
    virtual result_t get_txBufferSize(int32_t& retVal) = 0;
    virtual result_t set_txBufferSize(int32_t newVal) = 0;

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_get_rxBufferSize(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_rxBufferSize(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_txBufferSize(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_txBufferSize(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

namespace fibjs {
inline ClassInfo& MySQL_base::class_info()
{
    static ClassData::ClassProperty s_property[] = {
        { "rxBufferSize", s_get_rxBufferSize, s_set_rxBufferSize, false },
        { "txBufferSize", s_get_txBufferSize, s_set_txBufferSize, false }
    };

    static ClassData s_cd = {
        "MySQL", false, s__new, NULL,
        0, NULL, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &DbConnection_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void MySQL_base::s_get_rxBufferSize(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(MySQL_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_rxBufferSize(vr);

    METHOD_RETURN();
}

inline void MySQL_base::s_set_rxBufferSize(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(MySQL_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(int32_t, 0);

    hr = pInst->set_rxBufferSize(v0);

    METHOD_VOID();
}

inline void MySQL_base::s_get_txBufferSize(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(MySQL_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_txBufferSize(vr);

    METHOD_RETURN();
}

inline void MySQL_base::s_set_txBufferSize(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(MySQL_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(int32_t, 0);

    hr = pInst->set_txBufferSize(v0);

    METHOD_VOID();
}
}
