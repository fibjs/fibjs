/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _List_base_H_
#define _List_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs
{

class List_base : public object_base
{
    DECLARE_CLASS(List_base);

public:
    // List_base
    static result_t _new(obj_ptr<List_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    virtual result_t _indexed_getter(uint32_t index, Variant& retVal) = 0;
    virtual result_t _indexed_setter(uint32_t index, Variant newVal) = 0;
    virtual result_t get_length(int32_t& retVal) = 0;
    virtual result_t resize(int32_t sz) = 0;
    virtual result_t push(Variant v) = 0;
    virtual result_t push(const v8::FunctionCallbackInfo<v8::Value>& args) = 0;
    virtual result_t pop(Variant& retVal) = 0;
    virtual result_t slice(int32_t start, int32_t end, obj_ptr<List_base>& retVal) = 0;
    virtual result_t concat(const v8::FunctionCallbackInfo<v8::Value>& args, obj_ptr<List_base>& retVal) = 0;
    virtual result_t every(v8::Local<v8::Function> func, v8::Local<v8::Object> thisp, bool& retVal) = 0;
    virtual result_t some(v8::Local<v8::Function> func, v8::Local<v8::Object> thisp, bool& retVal) = 0;
    virtual result_t filter(v8::Local<v8::Function> func, v8::Local<v8::Object> thisp, obj_ptr<List_base>& retVal) = 0;
    virtual result_t forEach(v8::Local<v8::Function> func, v8::Local<v8::Object> thisp) = 0;
    virtual result_t map(v8::Local<v8::Function> func, v8::Local<v8::Object> thisp, obj_ptr<List_base>& retVal) = 0;
    virtual result_t toArray(v8::Local<v8::Array>& retVal) = 0;

public:
    template<typename T>
    static void __new(const T &args);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void i_IndexedGetter(uint32_t index, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void i_IndexedSetter(uint32_t index, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get_length(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_resize(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_push(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_pop(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_slice(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_concat(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_every(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_some(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_filter(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_forEach(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_map(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_toArray(const v8::FunctionCallbackInfo<v8::Value>& args);
};

}

namespace fibjs
{
    inline ClassInfo& List_base::class_info()
    {
        static ClassData::ClassMethod s_method[] = 
        {
            {"resize", s_resize, false},
            {"push", s_push, false},
            {"pop", s_pop, false},
            {"slice", s_slice, false},
            {"concat", s_concat, false},
            {"every", s_every, false},
            {"some", s_some, false},
            {"filter", s_filter, false},
            {"forEach", s_forEach, false},
            {"map", s_map, false},
            {"toArray", s_toArray, false}
        };

        static ClassData::ClassProperty s_property[] = 
        {
            {"length", s_get_length, block_set, false}
        };

        static ClassData::ClassIndexed s_indexed = 
        {
            i_IndexedGetter, i_IndexedSetter
        };

        static ClassData s_cd = 
        { 
            "List", s__new, 
            11, s_method, 0, NULL, 1, s_property, &s_indexed, NULL,
            &object_base::class_info()
        };

        static ClassInfo s_ci(s_cd);
        return s_ci;
    }

    inline void List_base::i_IndexedGetter(uint32_t index, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        Variant vr;

        PROPERTY_ENTER();
        PROPERTY_INSTANCE(List_base);

        hr = pInst->_indexed_getter(index, vr);

        METHOD_RETURN();
    }

    inline void List_base::i_IndexedSetter(uint32_t index, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        PROPERTY_ENTER();
        PROPERTY_INSTANCE(List_base);

        PROPERTY_VAL(Variant);
        hr = pInst->_indexed_setter(index, v0);

        METHOD_VOID();
    }

    inline void List_base::s_get_length(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        int32_t vr;

        PROPERTY_ENTER();
        PROPERTY_INSTANCE(List_base);

        hr = pInst->get_length(vr);

        METHOD_RETURN();
    }

    inline void List_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();
        __new(args);
    }

    template<typename T>void List_base::__new(const T& args)
    {
        obj_ptr<List_base> vr;

        CONSTRUCT_ENTER(0, 0);

        hr = _new(vr, args.This());

        CONSTRUCT_RETURN();
    }

    inline void List_base::s_resize(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        METHOD_INSTANCE(List_base);
        METHOD_ENTER(1, 1);

        ARG(int32_t, 0);

        hr = pInst->resize(v0);

        METHOD_VOID();
    }

    inline void List_base::s_push(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        METHOD_INSTANCE(List_base);
        METHOD_ENTER(1, 1);

        ARG(Variant, 0);

        hr = pInst->push(v0);

        METHOD_OVER(-1, 0);

        hr = pInst->push(args);

        METHOD_VOID();
    }

    inline void List_base::s_pop(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        Variant vr;

        METHOD_INSTANCE(List_base);
        METHOD_ENTER(0, 0);

        hr = pInst->pop(vr);

        METHOD_RETURN();
    }

    inline void List_base::s_slice(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        obj_ptr<List_base> vr;

        METHOD_INSTANCE(List_base);
        METHOD_ENTER(2, 0);

        OPT_ARG(int32_t, 0, 0);
        OPT_ARG(int32_t, 1, -1);

        hr = pInst->slice(v0, v1, vr);

        METHOD_RETURN();
    }

    inline void List_base::s_concat(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        obj_ptr<List_base> vr;

        METHOD_INSTANCE(List_base);
        METHOD_ENTER(-1, 0);

        hr = pInst->concat(args, vr);

        METHOD_RETURN();
    }

    inline void List_base::s_every(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        bool vr;

        METHOD_INSTANCE(List_base);
        METHOD_ENTER(2, 1);

        ARG(v8::Local<v8::Function>, 0);
        OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(Isolate::now()->m_isolate));

        hr = pInst->every(v0, v1, vr);

        METHOD_RETURN();
    }

    inline void List_base::s_some(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        bool vr;

        METHOD_INSTANCE(List_base);
        METHOD_ENTER(2, 1);

        ARG(v8::Local<v8::Function>, 0);
        OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(Isolate::now()->m_isolate));

        hr = pInst->some(v0, v1, vr);

        METHOD_RETURN();
    }

    inline void List_base::s_filter(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        obj_ptr<List_base> vr;

        METHOD_INSTANCE(List_base);
        METHOD_ENTER(2, 1);

        ARG(v8::Local<v8::Function>, 0);
        OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(Isolate::now()->m_isolate));

        hr = pInst->filter(v0, v1, vr);

        METHOD_RETURN();
    }

    inline void List_base::s_forEach(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        METHOD_INSTANCE(List_base);
        METHOD_ENTER(2, 1);

        ARG(v8::Local<v8::Function>, 0);
        OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(Isolate::now()->m_isolate));

        hr = pInst->forEach(v0, v1);

        METHOD_VOID();
    }

    inline void List_base::s_map(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        obj_ptr<List_base> vr;

        METHOD_INSTANCE(List_base);
        METHOD_ENTER(2, 1);

        ARG(v8::Local<v8::Function>, 0);
        OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(Isolate::now()->m_isolate));

        hr = pInst->map(v0, v1, vr);

        METHOD_RETURN();
    }

    inline void List_base::s_toArray(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        v8::Local<v8::Array> vr;

        METHOD_INSTANCE(List_base);
        METHOD_ENTER(0, 0);

        hr = pInst->toArray(vr);

        METHOD_RETURN();
    }

}

#endif

