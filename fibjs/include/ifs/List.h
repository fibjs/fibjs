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
    static result_t _new(v8::Local<v8::Array> data, obj_ptr<List_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    virtual result_t _indexed_getter(uint32_t index, Variant& retVal) = 0;
    virtual result_t _indexed_setter(uint32_t index, Variant newVal) = 0;
    virtual result_t freeze() = 0;
    virtual result_t get_length(int32_t& retVal) = 0;
    virtual result_t resize(int32_t sz) = 0;
    virtual result_t push(Variant v) = 0;
    virtual result_t push(const v8::FunctionCallbackInfo<v8::Value>& args) = 0;
    virtual result_t pushArray(v8::Local<v8::Array> data) = 0;
    virtual result_t pop(Variant& retVal) = 0;
    virtual result_t slice(int32_t start, int32_t end, obj_ptr<List_base>& retVal) = 0;
    virtual result_t concat(const v8::FunctionCallbackInfo<v8::Value>& args, obj_ptr<List_base>& retVal) = 0;
    virtual result_t every(v8::Local<v8::Function> func, v8::Local<v8::Value> thisArg, bool& retVal) = 0;
    virtual result_t some(v8::Local<v8::Function> func, v8::Local<v8::Value> thisArg, bool& retVal) = 0;
    virtual result_t filter(v8::Local<v8::Function> func, v8::Local<v8::Value> thisArg, obj_ptr<List_base>& retVal) = 0;
    virtual result_t forEach(v8::Local<v8::Function> func, v8::Local<v8::Value> thisArg) = 0;
    virtual result_t map(v8::Local<v8::Function> func, v8::Local<v8::Value> thisArg, obj_ptr<List_base>& retVal) = 0;
    virtual result_t sort(v8::Local<v8::Function> func, obj_ptr<List_base>& retVal) = 0;
    virtual result_t sort(obj_ptr<List_base>& retVal) = 0;
    virtual result_t toArray(v8::Local<v8::Array>& retVal) = 0;

public:
    template<typename T>
    static void __new(const T &args);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void i_IndexedGetter(uint32_t index, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void i_IndexedSetter(uint32_t index, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_freeze(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_length(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_resize(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_push(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_pushArray(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_pop(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_slice(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_concat(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_every(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_some(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_filter(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_forEach(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_map(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_sort(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_toArray(const v8::FunctionCallbackInfo<v8::Value>& args);
};

}

namespace fibjs
{
    inline ClassInfo& List_base::class_info()
    {
        static ClassData::ClassMethod s_method[] = 
        {
            {"freeze", s_freeze, false},
            {"resize", s_resize, false},
            {"push", s_push, false},
            {"pushArray", s_pushArray, false},
            {"pop", s_pop, false},
            {"slice", s_slice, false},
            {"concat", s_concat, false},
            {"every", s_every, false},
            {"some", s_some, false},
            {"filter", s_filter, false},
            {"forEach", s_forEach, false},
            {"map", s_map, false},
            {"sort", s_sort, false},
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
            "List", s__new, NULL, 
            14, s_method, 0, NULL, 1, s_property, &s_indexed, NULL,
            &object_base::class_info()
        };

        static ClassInfo s_ci(s_cd);
        return s_ci;
    }

    inline void List_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();
        __new(args);
    }

    template<typename T>void List_base::__new(const T& args)
    {
        obj_ptr<List_base> vr;

        CONSTRUCT_ENTER();

        METHOD_OVER(0, 0);

        hr = _new(vr, args.This());

        METHOD_OVER(1, 1);

        ARG(v8::Local<v8::Array>, 0);

        hr = _new(v0, vr, args.This());

        CONSTRUCT_RETURN();
    }

    inline void List_base::i_IndexedGetter(uint32_t index, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        Variant vr;

        METHOD_INSTANCE(List_base);
        PROPERTY_ENTER();

        hr = pInst->_indexed_getter(index, vr);

        METHOD_RETURN();
    }

    inline void List_base::i_IndexedSetter(uint32_t index, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        METHOD_INSTANCE(List_base);
        PROPERTY_ENTER();

        PROPERTY_VAL(Variant);
        hr = pInst->_indexed_setter(index, v0);

        METHOD_VOID();
    }

    inline void List_base::s_freeze(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        METHOD_INSTANCE(List_base);
        METHOD_ENTER();

        METHOD_OVER(0, 0);

        hr = pInst->freeze();

        METHOD_VOID();
    }

    inline void List_base::s_get_length(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        int32_t vr;

        METHOD_INSTANCE(List_base);
        PROPERTY_ENTER();

        hr = pInst->get_length(vr);

        METHOD_RETURN();
    }

    inline void List_base::s_resize(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        METHOD_INSTANCE(List_base);
        METHOD_ENTER();

        METHOD_OVER(1, 1);

        ARG(int32_t, 0);

        hr = pInst->resize(v0);

        METHOD_VOID();
    }

    inline void List_base::s_push(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        METHOD_INSTANCE(List_base);
        METHOD_ENTER();

        METHOD_OVER(1, 1);

        ARG(Variant, 0);

        hr = pInst->push(v0);

        METHOD_OVER(-1, 0);

        hr = pInst->push(args);

        METHOD_VOID();
    }

    inline void List_base::s_pushArray(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        METHOD_INSTANCE(List_base);
        METHOD_ENTER();

        METHOD_OVER(1, 1);

        ARG(v8::Local<v8::Array>, 0);

        hr = pInst->pushArray(v0);

        METHOD_VOID();
    }

    inline void List_base::s_pop(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        Variant vr;

        METHOD_INSTANCE(List_base);
        METHOD_ENTER();

        METHOD_OVER(0, 0);

        hr = pInst->pop(vr);

        METHOD_RETURN();
    }

    inline void List_base::s_slice(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        obj_ptr<List_base> vr;

        METHOD_INSTANCE(List_base);
        METHOD_ENTER();

        METHOD_OVER(2, 0);

        OPT_ARG(int32_t, 0, 0);
        OPT_ARG(int32_t, 1, -1);

        hr = pInst->slice(v0, v1, vr);

        METHOD_RETURN();
    }

    inline void List_base::s_concat(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        obj_ptr<List_base> vr;

        METHOD_INSTANCE(List_base);
        METHOD_ENTER();

        METHOD_OVER(-1, 0);

        hr = pInst->concat(args, vr);

        METHOD_RETURN();
    }

    inline void List_base::s_every(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        bool vr;

        METHOD_INSTANCE(List_base);
        METHOD_ENTER();

        METHOD_OVER(2, 1);

        ARG(v8::Local<v8::Function>, 0);
        OPT_ARG(v8::Local<v8::Value>, 1, v8::Undefined(isolate));

        hr = pInst->every(v0, v1, vr);

        METHOD_RETURN();
    }

    inline void List_base::s_some(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        bool vr;

        METHOD_INSTANCE(List_base);
        METHOD_ENTER();

        METHOD_OVER(2, 1);

        ARG(v8::Local<v8::Function>, 0);
        OPT_ARG(v8::Local<v8::Value>, 1, v8::Undefined(isolate));

        hr = pInst->some(v0, v1, vr);

        METHOD_RETURN();
    }

    inline void List_base::s_filter(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        obj_ptr<List_base> vr;

        METHOD_INSTANCE(List_base);
        METHOD_ENTER();

        METHOD_OVER(2, 1);

        ARG(v8::Local<v8::Function>, 0);
        OPT_ARG(v8::Local<v8::Value>, 1, v8::Undefined(isolate));

        hr = pInst->filter(v0, v1, vr);

        METHOD_RETURN();
    }

    inline void List_base::s_forEach(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        METHOD_INSTANCE(List_base);
        METHOD_ENTER();

        METHOD_OVER(2, 1);

        ARG(v8::Local<v8::Function>, 0);
        OPT_ARG(v8::Local<v8::Value>, 1, v8::Undefined(isolate));

        hr = pInst->forEach(v0, v1);

        METHOD_VOID();
    }

    inline void List_base::s_map(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        obj_ptr<List_base> vr;

        METHOD_INSTANCE(List_base);
        METHOD_ENTER();

        METHOD_OVER(2, 1);

        ARG(v8::Local<v8::Function>, 0);
        OPT_ARG(v8::Local<v8::Value>, 1, v8::Undefined(isolate));

        hr = pInst->map(v0, v1, vr);

        METHOD_RETURN();
    }

    inline void List_base::s_sort(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        obj_ptr<List_base> vr;

        METHOD_INSTANCE(List_base);
        METHOD_ENTER();

        METHOD_OVER(1, 1);

        ARG(v8::Local<v8::Function>, 0);

        hr = pInst->sort(v0, vr);

        METHOD_OVER(0, 0);

        hr = pInst->sort(vr);

        METHOD_RETURN();
    }

    inline void List_base::s_toArray(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        v8::Local<v8::Array> vr;

        METHOD_INSTANCE(List_base);
        METHOD_ENTER();

        METHOD_OVER(0, 0);

        hr = pInst->toArray(vr);

        METHOD_RETURN();
    }

}

#endif

