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

namespace fibjs {

class HttpCollection_base : public object_base {
    DECLARE_CLASS(HttpCollection_base);

public:
    // HttpCollection_base
    virtual result_t clear() = 0;
    virtual result_t has(exlib::string name, bool& retVal) = 0;
    virtual result_t first(exlib::string name, Variant& retVal) = 0;
    virtual result_t get(exlib::string name, Variant& retVal) = 0;
    virtual result_t all(exlib::string name, obj_ptr<NObject>& retVal) = 0;
    virtual result_t add(v8::Local<v8::Object> map) = 0;
    virtual result_t add(exlib::string name, v8::Local<v8::Array> values) = 0;
    virtual result_t add(exlib::string name, Variant value) = 0;
    virtual result_t set(v8::Local<v8::Object> map) = 0;
    virtual result_t set(exlib::string name, v8::Local<v8::Array> values) = 0;
    virtual result_t set(exlib::string name, Variant value) = 0;
    virtual result_t remove(exlib::string name) = 0;
    virtual result_t _delete(exlib::string name) = 0;
    virtual result_t sort() = 0;
    virtual result_t keys(obj_ptr<NArray>& retVal) = 0;
    virtual result_t values(obj_ptr<NArray>& retVal) = 0;
    virtual result_t _named_getter(exlib::string property, Variant& retVal) = 0;
    virtual result_t _named_enumerator(v8::Local<v8::Array>& retVal) = 0;
    virtual result_t _named_setter(exlib::string property, Variant newVal) = 0;
    virtual result_t _named_deleter(exlib::string property, v8::Local<v8::Boolean>& retVal) = 0;

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_clear(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_has(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_first(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_all(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_add(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_remove(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s__delete(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_sort(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_keys(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_values(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void i_NamedGetter(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void i_NamedEnumerator(const v8::PropertyCallbackInfo<v8::Array>& args);
    static void i_NamedSetter(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void i_NamedDeleter(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Boolean>& args);
};
}

namespace fibjs {
inline ClassInfo& HttpCollection_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "clear", s_clear, false, false },
        { "has", s_has, false, false },
        { "first", s_first, false, false },
        { "get", s_get, false, false },
        { "all", s_all, false, false },
        { "add", s_add, false, false },
        { "set", s_set, false, false },
        { "remove", s_remove, false, false },
        { "delete", s__delete, false, false },
        { "sort", s_sort, false, false },
        { "keys", s_keys, false, false },
        { "values", s_values, false, false }
    };

    static ClassData::ClassNamed s_named = {
        i_NamedGetter, i_NamedSetter, i_NamedDeleter, i_NamedEnumerator
    };

    static ClassData s_cd = {
        "HttpCollection", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, 0, NULL, 0, NULL, NULL, &s_named,
        &object_base::class_info()
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void HttpCollection_base::s_clear(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(HttpCollection_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->clear();

    METHOD_VOID();
}

inline void HttpCollection_base::s_has(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_INSTANCE(HttpCollection_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = pInst->has(v0, vr);

    METHOD_RETURN();
}

inline void HttpCollection_base::s_first(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    Variant vr;

    METHOD_INSTANCE(HttpCollection_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = pInst->first(v0, vr);

    METHOD_RETURN();
}

inline void HttpCollection_base::s_get(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    Variant vr;

    METHOD_INSTANCE(HttpCollection_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = pInst->get(v0, vr);

    METHOD_RETURN();
}

inline void HttpCollection_base::s_all(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<NObject> vr;

    METHOD_INSTANCE(HttpCollection_base);
    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(exlib::string, 0, "");

    hr = pInst->all(v0, vr);

    METHOD_RETURN();
}

inline void HttpCollection_base::s_add(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(HttpCollection_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Object>, 0);

    hr = pInst->add(v0);

    METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(v8::Local<v8::Array>, 1);

    hr = pInst->add(v0, v1);

    METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(Variant, 1);

    hr = pInst->add(v0, v1);

    METHOD_VOID();
}

inline void HttpCollection_base::s_set(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(HttpCollection_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Object>, 0);

    hr = pInst->set(v0);

    METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(v8::Local<v8::Array>, 1);

    hr = pInst->set(v0, v1);

    METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(Variant, 1);

    hr = pInst->set(v0, v1);

    METHOD_VOID();
}

inline void HttpCollection_base::s_remove(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(HttpCollection_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = pInst->remove(v0);

    METHOD_VOID();
}

inline void HttpCollection_base::s__delete(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(HttpCollection_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = pInst->_delete(v0);

    METHOD_VOID();
}

inline void HttpCollection_base::s_sort(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(HttpCollection_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->sort();

    METHOD_VOID();
}

inline void HttpCollection_base::s_keys(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<NArray> vr;

    METHOD_INSTANCE(HttpCollection_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->keys(vr);

    METHOD_RETURN();
}

inline void HttpCollection_base::s_values(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<NArray> vr;

    METHOD_INSTANCE(HttpCollection_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->values(vr);

    METHOD_RETURN();
}

inline void HttpCollection_base::i_NamedGetter(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    Variant vr;

    METHOD_INSTANCE(HttpCollection_base);
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

inline void HttpCollection_base::i_NamedEnumerator(const v8::PropertyCallbackInfo<v8::Array>& args)
{
    v8::Local<v8::Array> vr;

    METHOD_INSTANCE(HttpCollection_base);
    PROPERTY_ENTER();

    hr = pInst->_named_enumerator(vr);

    METHOD_RETURN1();
}

inline void HttpCollection_base::i_NamedSetter(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(HttpCollection_base);
    PROPERTY_ENTER();

    PROPERTY_VAL(Variant);
    exlib::string k;
    GetArgumentValue(isolate, property, k);
    if (class_info().has(k.c_str()))
        return;

    hr = pInst->_named_setter(k, v0);
    if (hr == CALL_RETURN_NULL)
        return;

    METHOD_VOID();
}

inline void HttpCollection_base::i_NamedDeleter(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Boolean>& args)
{
    v8::Local<v8::Boolean> vr;

    METHOD_INSTANCE(HttpCollection_base);
    PROPERTY_ENTER();

    exlib::string k;
    GetArgumentValue(isolate, property, k);
    if (class_info().has(k.c_str())) {
        args.GetReturnValue().Set(v8::False(isolate->m_isolate));
        return;
    }

    hr = pInst->_named_deleter(k, vr);
    METHOD_RETURN1();
}
}
