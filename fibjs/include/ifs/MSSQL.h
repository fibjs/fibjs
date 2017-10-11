/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _MSSQL_base_H_
#define _MSSQL_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "DbConnection.h"

namespace fibjs {

class DbConnection_base;

class MSSQL_base : public DbConnection_base {
    DECLARE_CLASS(MSSQL_base);

public:
    // MSSQL_base
    virtual result_t use(exlib::string dbName, AsyncEvent* ac) = 0;

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        Isolate* isolate = Isolate::current();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_use(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
    ASYNC_MEMBER1(MSSQL_base, use, exlib::string);
};
}

namespace fibjs {
inline ClassInfo& MSSQL_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "use", s_use, false },
        { "useSync", s_use, false }
    };

    static ClassData s_cd = {
        "MSSQL", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, 0, NULL, NULL, NULL,
        &DbConnection_base::class_info()
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void MSSQL_base::s_use(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("MSSQL.use");
    METHOD_INSTANCE(MSSQL_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    if (!cb.IsEmpty()) {
        pInst->acb_use(v0, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = pInst->ac_use(v0);

    METHOD_VOID();
}
}

#endif
