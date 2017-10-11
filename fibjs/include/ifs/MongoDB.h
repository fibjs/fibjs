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

namespace fibjs {

class MongoCollection_base;
class GridFS_base;
class MongoID_base;

class MongoDB_base : public object_base {
    DECLARE_CLASS(MongoDB_base);

public:
    // MongoDB_base
    virtual result_t getCollection(exlib::string name, obj_ptr<MongoCollection_base>& retVal) = 0;
    virtual result_t runCommand(v8::Local<v8::Object> cmd, v8::Local<v8::Object>& retVal) = 0;
    virtual result_t runCommand(exlib::string cmd, v8::Local<v8::Value> arg, v8::Local<v8::Object>& retVal) = 0;
    virtual result_t _named_getter(exlib::string property, obj_ptr<MongoCollection_base>& retVal) = 0;
    virtual result_t _named_enumerator(v8::Local<v8::Array>& retVal) = 0;
    virtual result_t get_fs(obj_ptr<GridFS_base>& retVal) = 0;
    virtual result_t oid(exlib::string hexStr, obj_ptr<MongoID_base>& retVal) = 0;
    virtual result_t close(AsyncEvent* ac) = 0;

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        Isolate* isolate = Isolate::current();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_getCollection(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_runCommand(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void i_NamedGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void i_NamedEnumerator(const v8::PropertyCallbackInfo<v8::Array>& args);
    static void s_get_fs(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_oid(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_close(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
    ASYNC_MEMBER0(MongoDB_base, close);
};
}

#include "MongoCollection.h"
#include "GridFS.h"
#include "MongoID.h"

namespace fibjs {
inline ClassInfo& MongoDB_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "getCollection", s_getCollection, false },
        { "runCommand", s_runCommand, false },
        { "oid", s_oid, false },
        { "close", s_close, false },
        { "closeSync", s_close, false }
    };

    static ClassData::ClassProperty s_property[] = {
        { "fs", s_get_fs, block_set, false }
    };

    static ClassData::ClassNamed s_named = {
        i_NamedGetter, i_NamedSetter, i_NamedDeleter, i_NamedEnumerator
    };

    static ClassData s_cd = {
        "MongoDB", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, NULL, &s_named,
        &object_base::class_info()
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void MongoDB_base::s_getCollection(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<MongoCollection_base> vr;

    METHOD_NAME("MongoDB.getCollection");
    METHOD_INSTANCE(MongoDB_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = pInst->getCollection(v0, vr);

    METHOD_RETURN();
}

inline void MongoDB_base::s_runCommand(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Object> vr;

    METHOD_NAME("MongoDB.runCommand");
    METHOD_INSTANCE(MongoDB_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Object>, 0);

    hr = pInst->runCommand(v0, vr);

    METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(v8::Local<v8::Value>, 1);

    hr = pInst->runCommand(v0, v1, vr);

    METHOD_RETURN();
}

inline void MongoDB_base::i_NamedGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    obj_ptr<MongoCollection_base> vr;

    METHOD_NAME("MongoDB.undefined[]");
    METHOD_INSTANCE(MongoDB_base);
    PROPERTY_ENTER();

    exlib::string k;
    GetArgumentValue(isolate, property, k);
    if (class_info().has(k.c_str()))
        return;

    hr = pInst->_named_getter(k, vr);
    if (hr == CALL_RETURN_NULL)
        return;

    METHOD_RETURN();
}

inline void MongoDB_base::i_NamedEnumerator(const v8::PropertyCallbackInfo<v8::Array>& args)
{
    v8::Local<v8::Array> vr;

    METHOD_NAME("MongoDB.undefined");
    METHOD_INSTANCE(MongoDB_base);
    PROPERTY_ENTER();

    hr = pInst->_named_enumerator(vr);

    METHOD_RETURN1();
}

inline void MongoDB_base::s_get_fs(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    obj_ptr<GridFS_base> vr;

    METHOD_NAME("MongoDB.fs");
    METHOD_INSTANCE(MongoDB_base);
    PROPERTY_ENTER();

    hr = pInst->get_fs(vr);

    METHOD_RETURN();
}

inline void MongoDB_base::s_oid(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<MongoID_base> vr;

    METHOD_NAME("MongoDB.oid");
    METHOD_INSTANCE(MongoDB_base);
    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(exlib::string, 0, "");

    hr = pInst->oid(v0, vr);

    METHOD_RETURN();
}

inline void MongoDB_base::s_close(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("MongoDB.close");
    METHOD_INSTANCE(MongoDB_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(0, 0);

    if (!cb.IsEmpty()) {
        pInst->acb_close(cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = pInst->ac_close();

    METHOD_VOID();
}
}

#endif
