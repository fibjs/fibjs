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

class Odbc_base : public DbConnection_base {
    DECLARE_CLASS(Odbc_base);

public:
    // Odbc_base
    virtual result_t get_codec(exlib::string& retVal) = 0;
    virtual result_t set_codec(exlib::string newVal) = 0;

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_get_codec(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_codec(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

namespace fibjs {
inline ClassInfo& Odbc_base::class_info()
{
    static ClassData::ClassProperty s_property[] = {
        { "codec", s_get_codec, s_set_codec, false }
    };

    static ClassData s_cd = {
        "Odbc", false, s__new, NULL,
        0, NULL, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &DbConnection_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void Odbc_base::s_get_codec(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(Odbc_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_codec(vr);

    METHOD_RETURN();
}

inline void Odbc_base::s_set_codec(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(Odbc_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = pInst->set_codec(v0);

    METHOD_VOID();
}
}
