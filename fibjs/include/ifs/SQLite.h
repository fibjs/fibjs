/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _SQLite_base_H_
#define _SQLite_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "DbConnection.h"

namespace fibjs
{

class DbConnection_base;

class SQLite_base : public DbConnection_base
{
    DECLARE_CLASS(SQLite_base);

public:
    // SQLite_base
    virtual result_t get_fileName(exlib::string& retVal) = 0;
    virtual result_t get_timeout(int32_t& retVal) = 0;
    virtual result_t set_timeout(int32_t newVal) = 0;
    virtual result_t backup(exlib::string fileName, AsyncEvent* ac) = 0;

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        Isolate* isolate = Isolate::current();

        isolate->m_isolate->ThrowException(
            isolate->NewFromUtf8("not a constructor"));
    }

public:
    static void s_get_fileName(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get_timeout(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_set_timeout(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args);
    static void s_backup(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
    ASYNC_MEMBER1(SQLite_base, backup, exlib::string);
};

}

namespace fibjs
{
    inline ClassInfo& SQLite_base::class_info()
    {
        static ClassData::ClassMethod s_method[] = 
        {
            {"backup", s_backup, false}
        };

        static ClassData::ClassProperty s_property[] = 
        {
            {"fileName", s_get_fileName, block_set, false},
            {"timeout", s_get_timeout, s_set_timeout, false}
        };

        static ClassData s_cd = 
        { 
            "SQLite", s__new, NULL, 
            ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, NULL, NULL,
            &DbConnection_base::class_info()
        };

        static ClassInfo s_ci(s_cd);
        return s_ci;
    }

    inline void SQLite_base::s_get_fileName(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        exlib::string vr;

        METHOD_INSTANCE(SQLite_base);
        PROPERTY_ENTER();

        hr = pInst->get_fileName(vr);

        METHOD_RETURN();
    }

    inline void SQLite_base::s_get_timeout(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        int32_t vr;

        METHOD_INSTANCE(SQLite_base);
        PROPERTY_ENTER();

        hr = pInst->get_timeout(vr);

        METHOD_RETURN();
    }

    inline void SQLite_base::s_set_timeout(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args)
    {
        METHOD_INSTANCE(SQLite_base);
        PROPERTY_ENTER();
        PROPERTY_VAL(int32_t);

        hr = pInst->set_timeout(v0);

        PROPERTY_SET_LEAVE();
    }

    inline void SQLite_base::s_backup(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        METHOD_INSTANCE(SQLite_base);
        METHOD_ENTER();

        ASYNC_METHOD_OVER(1, 1);

        ARG(exlib::string, 0);

        if(!cb.IsEmpty()) {
            pInst->acb_backup(v0, cb);
            hr = CALL_RETURN_NULL;
        } else
            hr = pInst->ac_backup(v0);

        METHOD_VOID();
    }

}

#endif

