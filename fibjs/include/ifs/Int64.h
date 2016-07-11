/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _Int64_base_H_
#define _Int64_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs
{

class Int64_base : public object_base
{
    DECLARE_CLASS(Int64_base);

public:
    // Int64_base
    static result_t _new(int64_t num, obj_ptr<Int64_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    static result_t _new(int64_t hi, int64_t lo, obj_ptr<Int64_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    static result_t _new(Int64_base* num, obj_ptr<Int64_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    static result_t _new(const char* num, int32_t base, obj_ptr<Int64_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    virtual result_t get_hi(int64_t& retVal) = 0;
    virtual result_t set_hi(int64_t newVal) = 0;
    virtual result_t get_lo(int64_t& retVal) = 0;
    virtual result_t set_lo(int64_t newVal) = 0;
    virtual result_t equal(Int64_base* num, bool& retVal) = 0;
    virtual result_t compare(Int64_base* num, int32_t& retVal) = 0;
    virtual result_t shiftLeft(int32_t bits, obj_ptr<Int64_base>& retVal) = 0;
    virtual result_t shiftRight(int32_t bits, obj_ptr<Int64_base>& retVal) = 0;
    virtual result_t _and(Int64_base* num, obj_ptr<Int64_base>& retVal) = 0;
    virtual result_t _or(Int64_base* num, obj_ptr<Int64_base>& retVal) = 0;
    virtual result_t _xor(Int64_base* num, obj_ptr<Int64_base>& retVal) = 0;
    virtual result_t add(Int64_base* num, obj_ptr<Int64_base>& retVal) = 0;
    virtual result_t sub(Int64_base* num, obj_ptr<Int64_base>& retVal) = 0;
    virtual result_t toNumber(double& retVal) = 0;
    virtual result_t toString(int32_t base, qstring& retVal) = 0;

public:
    template<typename T>
    static void __new(const T &args);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_hi(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_set_hi(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args);
    static void s_get_lo(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_set_lo(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args);
    static void s_equal(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_compare(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_shiftLeft(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_shiftRight(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_and(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_or(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_xor(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_add(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_sub(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_toNumber(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_toString(const v8::FunctionCallbackInfo<v8::Value>& args);
};

}

namespace fibjs
{
    inline ClassInfo& Int64_base::class_info()
    {
        static ClassData::ClassMethod s_method[] = 
        {
            {"equal", s_equal, false},
            {"compare", s_compare, false},
            {"shiftLeft", s_shiftLeft, false},
            {"shiftRight", s_shiftRight, false},
            {"and", s_and, false},
            {"or", s_or, false},
            {"xor", s_xor, false},
            {"add", s_add, false},
            {"sub", s_sub, false},
            {"toNumber", s_toNumber, false},
            {"toString", s_toString, false}
        };

        static ClassData::ClassProperty s_property[] = 
        {
            {"hi", s_get_hi, s_set_hi, false},
            {"lo", s_get_lo, s_set_lo, false}
        };

        static ClassData s_cd = 
        { 
            "Int64", s__new, NULL, 
            11, s_method, 0, NULL, 2, s_property, NULL, NULL,
            &object_base::class_info()
        };

        static ClassInfo s_ci(s_cd);
        return s_ci;
    }

    inline void Int64_base::s_get_hi(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        int64_t vr;

        PROPERTY_ENTER();
        PROPERTY_INSTANCE(Int64_base);

        hr = pInst->get_hi(vr);

        METHOD_RETURN();
    }

    inline void Int64_base::s_set_hi(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args)
    {
        PROPERTY_ENTER();
        PROPERTY_INSTANCE(Int64_base);

        PROPERTY_VAL(int64_t);
        hr = pInst->set_hi(v0);

        PROPERTY_SET_LEAVE();
    }

    inline void Int64_base::s_get_lo(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        int64_t vr;

        PROPERTY_ENTER();
        PROPERTY_INSTANCE(Int64_base);

        hr = pInst->get_lo(vr);

        METHOD_RETURN();
    }

    inline void Int64_base::s_set_lo(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args)
    {
        PROPERTY_ENTER();
        PROPERTY_INSTANCE(Int64_base);

        PROPERTY_VAL(int64_t);
        hr = pInst->set_lo(v0);

        PROPERTY_SET_LEAVE();
    }

    inline void Int64_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();
        __new(args);
    }

    template<typename T>void Int64_base::__new(const T& args)
    {
        obj_ptr<Int64_base> vr;

        CONSTRUCT_ENTER(1, 0);

        OPT_ARG(int64_t, 0, 0);

        hr = _new(v0, vr, args.This());

        METHOD_OVER(2, 2);

        ARG(int64_t, 0);
        ARG(int64_t, 1);

        hr = _new(v0, v1, vr, args.This());

        METHOD_OVER(1, 1);

        STRICT_ARG(obj_ptr<Int64_base>, 0);

        hr = _new(v0, vr, args.This());

        METHOD_OVER(2, 1);

        ARG(arg_string, 0);
        OPT_ARG(int32_t, 1, 10);

        hr = _new(v0, v1, vr, args.This());

        CONSTRUCT_RETURN();
    }

    inline void Int64_base::s_equal(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        bool vr;

        METHOD_INSTANCE(Int64_base);
        METHOD_ENTER(1, 1);

        ARG(obj_ptr<Int64_base>, 0);

        hr = pInst->equal(v0, vr);

        METHOD_RETURN();
    }

    inline void Int64_base::s_compare(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        int32_t vr;

        METHOD_INSTANCE(Int64_base);
        METHOD_ENTER(1, 1);

        ARG(obj_ptr<Int64_base>, 0);

        hr = pInst->compare(v0, vr);

        METHOD_RETURN();
    }

    inline void Int64_base::s_shiftLeft(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        obj_ptr<Int64_base> vr;

        METHOD_INSTANCE(Int64_base);
        METHOD_ENTER(1, 1);

        ARG(int32_t, 0);

        hr = pInst->shiftLeft(v0, vr);

        METHOD_RETURN();
    }

    inline void Int64_base::s_shiftRight(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        obj_ptr<Int64_base> vr;

        METHOD_INSTANCE(Int64_base);
        METHOD_ENTER(1, 1);

        ARG(int32_t, 0);

        hr = pInst->shiftRight(v0, vr);

        METHOD_RETURN();
    }

    inline void Int64_base::s_and(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        obj_ptr<Int64_base> vr;

        METHOD_INSTANCE(Int64_base);
        METHOD_ENTER(1, 1);

        ARG(obj_ptr<Int64_base>, 0);

        hr = pInst->_and(v0, vr);

        METHOD_RETURN();
    }

    inline void Int64_base::s_or(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        obj_ptr<Int64_base> vr;

        METHOD_INSTANCE(Int64_base);
        METHOD_ENTER(1, 1);

        ARG(obj_ptr<Int64_base>, 0);

        hr = pInst->_or(v0, vr);

        METHOD_RETURN();
    }

    inline void Int64_base::s_xor(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        obj_ptr<Int64_base> vr;

        METHOD_INSTANCE(Int64_base);
        METHOD_ENTER(1, 1);

        ARG(obj_ptr<Int64_base>, 0);

        hr = pInst->_xor(v0, vr);

        METHOD_RETURN();
    }

    inline void Int64_base::s_add(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        obj_ptr<Int64_base> vr;

        METHOD_INSTANCE(Int64_base);
        METHOD_ENTER(1, 1);

        ARG(obj_ptr<Int64_base>, 0);

        hr = pInst->add(v0, vr);

        METHOD_RETURN();
    }

    inline void Int64_base::s_sub(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        obj_ptr<Int64_base> vr;

        METHOD_INSTANCE(Int64_base);
        METHOD_ENTER(1, 1);

        ARG(obj_ptr<Int64_base>, 0);

        hr = pInst->sub(v0, vr);

        METHOD_RETURN();
    }

    inline void Int64_base::s_toNumber(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        double vr;

        METHOD_INSTANCE(Int64_base);
        METHOD_ENTER(0, 0);

        hr = pInst->toNumber(vr);

        METHOD_RETURN();
    }

    inline void Int64_base::s_toString(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        qstring vr;

        METHOD_INSTANCE(Int64_base);
        METHOD_ENTER(1, 0);

        OPT_ARG(int32_t, 0, 10);

        hr = pInst->toString(v0, vr);

        METHOD_RETURN();
    }

}

#endif

