/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _DBResult_base_H_
#define _DBResult_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "List.h"

namespace fibjs
{

class List_base;

class DBResult_base : public List_base
{
    DECLARE_CLASS(DBResult_base);

public:
    // DBResult_base
    virtual result_t get_insertId(int64_t& retVal) = 0;
    virtual result_t get_affected(int64_t& retVal) = 0;
    virtual result_t get_fields(v8::Local<v8::Array>& retVal) = 0;

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        Isolate* isolate = Isolate::current();

        isolate->m_isolate->ThrowException(
            isolate->NewFromUtf8("not a constructor"));
    }

public:
    static void s_get_insertId(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get_affected(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get_fields(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
};

}

namespace fibjs
{
    inline ClassInfo& DBResult_base::class_info()
    {
        static ClassData::ClassProperty s_property[] = 
        {
            {"insertId", s_get_insertId, block_set, false},
            {"affected", s_get_affected, block_set, false},
            {"fields", s_get_fields, block_set, false}
        };

        static ClassData s_cd = 
        { 
            "DBResult", s__new, NULL, 
            0, NULL, 0, NULL, ARRAYSIZE(s_property), s_property, NULL, NULL,
            &List_base::class_info()
        };

        static ClassInfo s_ci(s_cd);
        return s_ci;
    }

    inline void DBResult_base::s_get_insertId(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        int64_t vr;

        METHOD_INSTANCE(DBResult_base);
        PROPERTY_ENTER();

        hr = pInst->get_insertId(vr);

        METHOD_RETURN();
    }

    inline void DBResult_base::s_get_affected(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        int64_t vr;

        METHOD_INSTANCE(DBResult_base);
        PROPERTY_ENTER();

        hr = pInst->get_affected(vr);

        METHOD_RETURN();
    }

    inline void DBResult_base::s_get_fields(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        v8::Local<v8::Array> vr;

        METHOD_INSTANCE(DBResult_base);
        PROPERTY_ENTER();

        hr = pInst->get_fields(vr);

        METHOD_RETURN();
    }

}

#endif

