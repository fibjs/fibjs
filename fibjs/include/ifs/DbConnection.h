/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _DbConnection_base_H_
#define _DbConnection_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs
{

class DBResult_base;

class DbConnection_base : public object_base
{
    DECLARE_CLASS(DbConnection_base);

public:
    // DbConnection_base
    virtual result_t close(AsyncEvent* ac) = 0;
    virtual result_t begin(AsyncEvent* ac) = 0;
    virtual result_t commit(AsyncEvent* ac) = 0;
    virtual result_t rollback(AsyncEvent* ac) = 0;
    virtual result_t execute(const char* sql, obj_ptr<DBResult_base>& retVal, AsyncEvent* ac) = 0;
    virtual result_t execute(const char* sql, const v8::FunctionCallbackInfo<v8::Value>& args, obj_ptr<DBResult_base>& retVal) = 0;
    virtual result_t format(const char* sql, const v8::FunctionCallbackInfo<v8::Value>& args, qstring& retVal) = 0;

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        Isolate* isolate = Isolate::current();

        isolate->m_isolate->ThrowException(
            isolate->NewFromUtf8("not a constructor"));
    }

public:
    static void s_close(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_begin(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_commit(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_rollback(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_execute(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_format(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
    ASYNC_MEMBER0(DbConnection_base, close);
    ASYNC_MEMBER0(DbConnection_base, begin);
    ASYNC_MEMBER0(DbConnection_base, commit);
    ASYNC_MEMBER0(DbConnection_base, rollback);
    ASYNC_MEMBERVALUE2(DbConnection_base, execute, const char*, obj_ptr<DBResult_base>);
};

}

#include "DBResult.h"

namespace fibjs
{
    inline ClassInfo& DbConnection_base::class_info()
    {
        static ClassData::ClassMethod s_method[] = 
        {
            {"close", s_close, false},
            {"begin", s_begin, false},
            {"commit", s_commit, false},
            {"rollback", s_rollback, false},
            {"execute", s_execute, false},
            {"format", s_format, false}
        };

        static ClassData s_cd = 
        { 
            "DbConnection", s__new, NULL, 
            6, s_method, 0, NULL, 0, NULL, NULL, NULL,
            &object_base::class_info()
        };

        static ClassInfo s_ci(s_cd);
        return s_ci;
    }


    inline void DbConnection_base::s_close(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        METHOD_INSTANCE(DbConnection_base);
        ASYNC_METHOD_ENTER(0, 0);

        if(!cb.IsEmpty()) {
            pInst->acb_close(cb);
            hr = CALL_RETURN_NULL;
        } else
            hr = pInst->ac_close();

        METHOD_VOID();
    }

    inline void DbConnection_base::s_begin(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        METHOD_INSTANCE(DbConnection_base);
        ASYNC_METHOD_ENTER(0, 0);

        if(!cb.IsEmpty()) {
            pInst->acb_begin(cb);
            hr = CALL_RETURN_NULL;
        } else
            hr = pInst->ac_begin();

        METHOD_VOID();
    }

    inline void DbConnection_base::s_commit(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        METHOD_INSTANCE(DbConnection_base);
        ASYNC_METHOD_ENTER(0, 0);

        if(!cb.IsEmpty()) {
            pInst->acb_commit(cb);
            hr = CALL_RETURN_NULL;
        } else
            hr = pInst->ac_commit();

        METHOD_VOID();
    }

    inline void DbConnection_base::s_rollback(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        METHOD_INSTANCE(DbConnection_base);
        ASYNC_METHOD_ENTER(0, 0);

        if(!cb.IsEmpty()) {
            pInst->acb_rollback(cb);
            hr = CALL_RETURN_NULL;
        } else
            hr = pInst->ac_rollback();

        METHOD_VOID();
    }

    inline void DbConnection_base::s_execute(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        obj_ptr<DBResult_base> vr;

        METHOD_INSTANCE(DbConnection_base);
        ASYNC_METHOD_ENTER(1, 1);

        ARG(arg_string, 0);

        if(!cb.IsEmpty()) {
            pInst->acb_execute(v0, vr, cb);
            hr = CALL_RETURN_NULL;
        } else
            hr = pInst->ac_execute(v0, vr);

        METHOD_OVER(-1, 1);

        ARG(arg_string, 0);

        hr = pInst->execute(v0, args, vr);

        METHOD_RETURN();
    }

    inline void DbConnection_base::s_format(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        qstring vr;

        METHOD_INSTANCE(DbConnection_base);
        METHOD_ENTER(-1, 1);

        ARG(arg_string, 0);

        hr = pInst->format(v0, args, vr);

        METHOD_RETURN();
    }

}

#endif

