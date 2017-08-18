/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _DBRow_base_H_
#define _DBRow_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs {

class DBRow_base : public object_base {
    DECLARE_CLASS(DBRow_base);

public:
    // DBRow_base
    virtual result_t _indexed_getter(uint32_t index, v8::Local<v8::Value>& retVal) = 0;
    virtual result_t _indexed_setter(uint32_t index, v8::Local<v8::Value> newVal) = 0;
    virtual result_t _named_getter(const char* property, v8::Local<v8::Value>& retVal) = 0;
    virtual result_t _named_enumerator(v8::Local<v8::Array>& retVal) = 0;
    virtual result_t _named_setter(const char* property, v8::Local<v8::Value> newVal) = 0;
    virtual result_t _named_deleter(const char* property, v8::Local<v8::Boolean>& retVal) = 0;

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        Isolate* isolate = Isolate::current();

        isolate->m_isolate->ThrowException(
            isolate->NewFromUtf8("not a constructor"));
    }

public:
    static void i_IndexedGetter(uint32_t index, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void i_IndexedSetter(uint32_t index, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void i_NamedGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void i_NamedEnumerator(const v8::PropertyCallbackInfo<v8::Array>& args);
    static void i_NamedSetter(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void i_NamedDeleter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Boolean>& args);
};
}

namespace fibjs {
inline ClassInfo& DBRow_base::class_info()
{
    static ClassData::ClassIndexed s_indexed = {
        i_IndexedGetter, i_IndexedSetter
    };

    static ClassData::ClassNamed s_named = {
        i_NamedGetter, i_NamedSetter, i_NamedDeleter, i_NamedEnumerator
    };

    static ClassData s_cd = {
        "DBRow", false, s__new, NULL,
        0, NULL, 0, NULL, 0, NULL, &s_indexed, &s_named,
        &object_base::class_info()
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void DBRow_base::i_IndexedGetter(uint32_t index, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Value> vr;

    METHOD_INSTANCE(DBRow_base);
    PROPERTY_ENTER();

    hr = pInst->_indexed_getter(index, vr);

    METHOD_RETURN();
}

inline void DBRow_base::i_IndexedSetter(uint32_t index, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(DBRow_base);
    PROPERTY_ENTER();

    PROPERTY_VAL(v8::Local<v8::Value>);
    hr = pInst->_indexed_setter(index, v0);

    METHOD_VOID();
}

inline void DBRow_base::i_NamedGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Value> vr;

    METHOD_INSTANCE(DBRow_base);
    PROPERTY_ENTER();

    v8::String::Utf8Value k(property);
    if (class_info().has(*k))
        return;

    hr = pInst->_named_getter(*k, vr);
    if (hr == CALL_RETURN_UNDEFINED)
        return;

    METHOD_RETURN();
}

inline void DBRow_base::i_NamedEnumerator(const v8::PropertyCallbackInfo<v8::Array>& args)
{
    v8::Local<v8::Array> vr;

    METHOD_INSTANCE(DBRow_base);
    PROPERTY_ENTER();

    hr = pInst->_named_enumerator(vr);

    METHOD_RETURN1();
}

inline void DBRow_base::i_NamedSetter(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(DBRow_base);
    PROPERTY_ENTER();

    PROPERTY_VAL(v8::Local<v8::Value>);
    v8::String::Utf8Value k(property);
    if (class_info().has(*k))
        return;

    hr = pInst->_named_setter(*k, v0);
    if (hr == CALL_RETURN_UNDEFINED)
        return;

    METHOD_VOID();
}

inline void DBRow_base::i_NamedDeleter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Boolean>& args)
{
    v8::Local<v8::Boolean> vr;

    METHOD_INSTANCE(DBRow_base);
    PROPERTY_ENTER();

    v8::String::Utf8Value k(property);
    if (class_info().has(*k)) {
        args.GetReturnValue().Set(v8::False(isolate));
        return;
    }

    hr = pInst->_named_deleter(*k, vr);
    METHOD_RETURN1();
}
}

#endif
