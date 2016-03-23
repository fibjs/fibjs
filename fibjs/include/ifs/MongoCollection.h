/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _MongoCollection_base_H_
#define _MongoCollection_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs
{

class MongoCursor_base;

class MongoCollection_base : public object_base
{
    DECLARE_CLASS(MongoCollection_base);

public:
    // MongoCollection_base
    virtual result_t find(v8::Local<v8::Object> query, v8::Local<v8::Object> projection, obj_ptr<MongoCursor_base>& retVal) = 0;
    virtual result_t findOne(v8::Local<v8::Object> query, v8::Local<v8::Object> projection, v8::Local<v8::Object>& retVal) = 0;
    virtual result_t findAndModify(v8::Local<v8::Object> query, v8::Local<v8::Object>& retVal) = 0;
    virtual result_t insert(v8::Local<v8::Array> documents) = 0;
    virtual result_t insert(v8::Local<v8::Object> document) = 0;
    virtual result_t save(v8::Local<v8::Object> document) = 0;
    virtual result_t update(v8::Local<v8::Object> query, v8::Local<v8::Object> document, bool upsert, bool multi) = 0;
    virtual result_t update(v8::Local<v8::Object> query, v8::Local<v8::Object> document, v8::Local<v8::Object> options) = 0;
    virtual result_t remove(v8::Local<v8::Object> query) = 0;
    virtual result_t runCommand(v8::Local<v8::Object> cmd, v8::Local<v8::Object>& retVal) = 0;
    virtual result_t runCommand(const char* cmd, v8::Local<v8::Object> arg, v8::Local<v8::Object>& retVal) = 0;
    virtual result_t drop() = 0;
    virtual result_t ensureIndex(v8::Local<v8::Object> keys, v8::Local<v8::Object> options) = 0;
    virtual result_t reIndex(v8::Local<v8::Object>& retVal) = 0;
    virtual result_t dropIndex(const char* name, v8::Local<v8::Object>& retVal) = 0;
    virtual result_t dropIndexes(v8::Local<v8::Object>& retVal) = 0;
    virtual result_t getIndexes(obj_ptr<MongoCursor_base>& retVal) = 0;
    virtual result_t getCollection(const char* name, obj_ptr<MongoCollection_base>& retVal) = 0;
    virtual result_t _named_getter(const char* property, obj_ptr<MongoCollection_base>& retVal) = 0;
    virtual result_t _named_enumerator(v8::Local<v8::Array>& retVal) = 0;

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        Isolate* isolate = Isolate::current();

        isolate->m_isolate->ThrowException(
            isolate->NewFromUtf8("not a constructor"));
    }

public:
    static void s_find(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_findOne(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_findAndModify(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_insert(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_save(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_update(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_remove(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_runCommand(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_drop(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_ensureIndex(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_reIndex(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_dropIndex(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_dropIndexes(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_getIndexes(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_getCollection(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void i_NamedGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void i_NamedEnumerator(const v8::PropertyCallbackInfo<v8::Array> &args);
};

}

#include "MongoCursor.h"

namespace fibjs
{
    inline ClassInfo& MongoCollection_base::class_info()
    {
        static ClassData::ClassMethod s_method[] = 
        {
            {"find", s_find, false},
            {"findOne", s_findOne, false},
            {"findAndModify", s_findAndModify, false},
            {"insert", s_insert, false},
            {"save", s_save, false},
            {"update", s_update, false},
            {"remove", s_remove, false},
            {"runCommand", s_runCommand, false},
            {"drop", s_drop, false},
            {"ensureIndex", s_ensureIndex, false},
            {"reIndex", s_reIndex, false},
            {"dropIndex", s_dropIndex, false},
            {"dropIndexes", s_dropIndexes, false},
            {"getIndexes", s_getIndexes, false},
            {"getCollection", s_getCollection, false}
        };

        static ClassData::ClassNamed s_named = 
        {
            i_NamedGetter, i_NamedSetter, i_NamedDeleter, i_NamedEnumerator
        };

        static ClassData s_cd = 
        { 
            "MongoCollection", s__new, NULL, 
            15, s_method, 0, NULL, 0, NULL, NULL, &s_named,
            &object_base::class_info()
        };

        static ClassInfo s_ci(s_cd);
        return s_ci;
    }

    inline void MongoCollection_base::i_NamedGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        obj_ptr<MongoCollection_base> vr;

        PROPERTY_ENTER();
        PROPERTY_INSTANCE(MongoCollection_base);

        v8::String::Utf8Value k(property);
        if(class_info().has(*k))return;

        hr = pInst->_named_getter(*k, vr);
        if(hr == CALL_RETURN_NULL)return;

        METHOD_RETURN();
    }

    inline void MongoCollection_base::i_NamedEnumerator(const v8::PropertyCallbackInfo<v8::Array> &args)
    {
        v8::Local<v8::Array> vr;

        PROPERTY_ENTER();
        PROPERTY_INSTANCE(MongoCollection_base);

        hr = pInst->_named_enumerator(vr);

        METHOD_RETURN1();
    }

    inline void MongoCollection_base::s_find(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        obj_ptr<MongoCursor_base> vr;

        METHOD_INSTANCE(MongoCollection_base);
        METHOD_ENTER(2, 0);

        OPT_ARG(v8::Local<v8::Object>, 0, v8::Object::New(isolate));
        OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate));

        hr = pInst->find(v0, v1, vr);

        METHOD_RETURN();
    }

    inline void MongoCollection_base::s_findOne(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        v8::Local<v8::Object> vr;

        METHOD_INSTANCE(MongoCollection_base);
        METHOD_ENTER(2, 0);

        OPT_ARG(v8::Local<v8::Object>, 0, v8::Object::New(isolate));
        OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate));

        hr = pInst->findOne(v0, v1, vr);

        METHOD_RETURN();
    }

    inline void MongoCollection_base::s_findAndModify(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        v8::Local<v8::Object> vr;

        METHOD_INSTANCE(MongoCollection_base);
        METHOD_ENTER(1, 1);

        ARG(v8::Local<v8::Object>, 0);

        hr = pInst->findAndModify(v0, vr);

        METHOD_RETURN();
    }

    inline void MongoCollection_base::s_insert(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        METHOD_INSTANCE(MongoCollection_base);
        METHOD_ENTER(1, 1);

        ARG(v8::Local<v8::Array>, 0);

        hr = pInst->insert(v0);

        METHOD_OVER(1, 1);

        ARG(v8::Local<v8::Object>, 0);

        hr = pInst->insert(v0);

        METHOD_VOID();
    }

    inline void MongoCollection_base::s_save(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        METHOD_INSTANCE(MongoCollection_base);
        METHOD_ENTER(1, 1);

        ARG(v8::Local<v8::Object>, 0);

        hr = pInst->save(v0);

        METHOD_VOID();
    }

    inline void MongoCollection_base::s_update(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        METHOD_INSTANCE(MongoCollection_base);
        METHOD_ENTER(4, 2);

        ARG(v8::Local<v8::Object>, 0);
        ARG(v8::Local<v8::Object>, 1);
        OPT_ARG(bool, 2, false);
        OPT_ARG(bool, 3, false);

        hr = pInst->update(v0, v1, v2, v3);

        METHOD_OVER(3, 3);

        ARG(v8::Local<v8::Object>, 0);
        ARG(v8::Local<v8::Object>, 1);
        ARG(v8::Local<v8::Object>, 2);

        hr = pInst->update(v0, v1, v2);

        METHOD_VOID();
    }

    inline void MongoCollection_base::s_remove(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        METHOD_INSTANCE(MongoCollection_base);
        METHOD_ENTER(1, 1);

        ARG(v8::Local<v8::Object>, 0);

        hr = pInst->remove(v0);

        METHOD_VOID();
    }

    inline void MongoCollection_base::s_runCommand(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        v8::Local<v8::Object> vr;

        METHOD_INSTANCE(MongoCollection_base);
        METHOD_ENTER(1, 1);

        ARG(v8::Local<v8::Object>, 0);

        hr = pInst->runCommand(v0, vr);

        METHOD_OVER(2, 1);

        ARG(arg_string, 0);
        OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate));

        hr = pInst->runCommand(v0, v1, vr);

        METHOD_RETURN();
    }

    inline void MongoCollection_base::s_drop(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        METHOD_INSTANCE(MongoCollection_base);
        METHOD_ENTER(0, 0);

        hr = pInst->drop();

        METHOD_VOID();
    }

    inline void MongoCollection_base::s_ensureIndex(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        METHOD_INSTANCE(MongoCollection_base);
        METHOD_ENTER(2, 1);

        ARG(v8::Local<v8::Object>, 0);
        OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate));

        hr = pInst->ensureIndex(v0, v1);

        METHOD_VOID();
    }

    inline void MongoCollection_base::s_reIndex(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        v8::Local<v8::Object> vr;

        METHOD_INSTANCE(MongoCollection_base);
        METHOD_ENTER(0, 0);

        hr = pInst->reIndex(vr);

        METHOD_RETURN();
    }

    inline void MongoCollection_base::s_dropIndex(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        v8::Local<v8::Object> vr;

        METHOD_INSTANCE(MongoCollection_base);
        METHOD_ENTER(1, 1);

        ARG(arg_string, 0);

        hr = pInst->dropIndex(v0, vr);

        METHOD_RETURN();
    }

    inline void MongoCollection_base::s_dropIndexes(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        v8::Local<v8::Object> vr;

        METHOD_INSTANCE(MongoCollection_base);
        METHOD_ENTER(0, 0);

        hr = pInst->dropIndexes(vr);

        METHOD_RETURN();
    }

    inline void MongoCollection_base::s_getIndexes(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        obj_ptr<MongoCursor_base> vr;

        METHOD_INSTANCE(MongoCollection_base);
        METHOD_ENTER(0, 0);

        hr = pInst->getIndexes(vr);

        METHOD_RETURN();
    }

    inline void MongoCollection_base::s_getCollection(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        obj_ptr<MongoCollection_base> vr;

        METHOD_INSTANCE(MongoCollection_base);
        METHOD_ENTER(1, 1);

        ARG(arg_string, 0);

        hr = pInst->getCollection(v0, vr);

        METHOD_RETURN();
    }

}

#endif

