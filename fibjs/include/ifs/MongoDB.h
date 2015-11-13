/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _MongoDB_base_H_
#define _MongoDB_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs
{

class MongoCollection_base;
class GridFS_base;
class MongoID_base;

class MongoDB_base : public object_base
{
    DECLARE_CLASS(MongoDB_base);

public:
    // MongoDB_base
    virtual result_t getCollection(const char* name, obj_ptr<MongoCollection_base>& retVal, AsyncEvent* ac) = 0;
    virtual result_t runCommand(v8::Local<v8::Object> cmd, v8::Local<v8::Object>& retVal) = 0;
    virtual result_t runCommand(const char* cmd, v8::Local<v8::Value> arg, v8::Local<v8::Object>& retVal) = 0;
    virtual result_t _named_getter(const char* property, obj_ptr<MongoCollection_base>& retVal) = 0;
    virtual result_t _named_enumerator(v8::Local<v8::Array>& retVal) = 0;
    virtual result_t get_fs(obj_ptr<GridFS_base>& retVal) = 0;
    virtual result_t oid(const char* hexStr, obj_ptr<MongoID_base>& retVal, AsyncEvent* ac) = 0;
    virtual result_t close(AsyncEvent* ac) = 0;

public:
    static void s_getCollection(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_runCommand(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void i_NamedGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void i_NamedEnumerator(const v8::PropertyCallbackInfo<v8::Array> &args);
    static void s_get_fs(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_oid(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_close(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
    ASYNC_MEMBERVALUE2(MongoDB_base, getCollection, const char*, obj_ptr<MongoCollection_base>);
    ASYNC_MEMBERVALUE2(MongoDB_base, oid, const char*, obj_ptr<MongoID_base>);
    ASYNC_MEMBER0(MongoDB_base, close);
};

}

#include "MongoCollection.h"
#include "GridFS.h"
#include "MongoID.h"

namespace fibjs
{
    inline ClassInfo& MongoDB_base::class_info()
    {
        static ClassData::ClassMethod s_method[] = 
        {
            {"getCollection", s_getCollection, false},
            {"runCommand", s_runCommand, false},
            {"oid", s_oid, false},
            {"close", s_close, false}
        };

        static ClassData::ClassProperty s_property[] = 
        {
            {"fs", s_get_fs, block_set, false}
        };

        static ClassData::ClassNamed s_named = 
        {
            i_NamedGetter, i_NamedSetter, i_NamedDeleter, i_NamedEnumerator
        };

        static ClassData s_cd = 
        { 
            "MongoDB", NULL, 
            4, s_method, 0, NULL, 1, s_property, NULL, &s_named,
            &object_base::class_info()
        };

        static ClassInfo s_ci(s_cd);
        return s_ci;
    }

    inline void MongoDB_base::i_NamedGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        obj_ptr<MongoCollection_base> vr;

        PROPERTY_ENTER();
        PROPERTY_INSTANCE(MongoDB_base);

        v8::String::Utf8Value k(property);
        if(class_info().has(*k))return;

        hr = pInst->_named_getter(*k, vr);
        if(hr == CALL_RETURN_NULL)return;

        METHOD_RETURN();
    }

    inline void MongoDB_base::i_NamedEnumerator(const v8::PropertyCallbackInfo<v8::Array> &args)
    {
        v8::Local<v8::Array> vr;

        PROPERTY_ENTER();
        PROPERTY_INSTANCE(MongoDB_base);

        hr = pInst->_named_enumerator(vr);

        METHOD_RETURN1();
    }

    inline void MongoDB_base::s_get_fs(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        obj_ptr<GridFS_base> vr;

        PROPERTY_ENTER();
        PROPERTY_INSTANCE(MongoDB_base);

        hr = pInst->get_fs(vr);

        METHOD_RETURN();
    }

    inline void MongoDB_base::s_getCollection(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        obj_ptr<MongoCollection_base> vr;

        METHOD_INSTANCE(MongoDB_base);
        METHOD_ENTER(1, 1);

        ARG(arg_string, 0);

        hr = pInst->ac_getCollection(v0, vr);

        METHOD_RETURN();
    }

    inline void MongoDB_base::s_runCommand(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        v8::Local<v8::Object> vr;

        METHOD_INSTANCE(MongoDB_base);
        METHOD_ENTER(1, 1);

        ARG(v8::Local<v8::Object>, 0);

        hr = pInst->runCommand(v0, vr);

        METHOD_OVER(2, 2);

        ARG(arg_string, 0);
        ARG(v8::Local<v8::Value>, 1);

        hr = pInst->runCommand(v0, v1, vr);

        METHOD_RETURN();
    }

    inline void MongoDB_base::s_oid(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        obj_ptr<MongoID_base> vr;

        METHOD_INSTANCE(MongoDB_base);
        METHOD_ENTER(1, 0);

        OPT_ARG(arg_string, 0, "");

        hr = pInst->ac_oid(v0, vr);

        METHOD_RETURN();
    }

    inline void MongoDB_base::s_close(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        METHOD_INSTANCE(MongoDB_base);
        METHOD_ENTER(0, 0);

        hr = pInst->ac_close();

        METHOD_VOID();
    }

}

#endif

