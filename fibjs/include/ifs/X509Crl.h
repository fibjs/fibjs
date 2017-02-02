/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _X509Crl_base_H_
#define _X509Crl_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs
{

class Buffer_base;

class X509Crl_base : public object_base
{
    DECLARE_CLASS(X509Crl_base);

public:
    // X509Crl_base
    static result_t _new(obj_ptr<X509Crl_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    virtual result_t load(Buffer_base* derCrl) = 0;
    virtual result_t load(exlib::string pemCrl) = 0;
    virtual result_t loadFile(exlib::string filename) = 0;
    virtual result_t dump(v8::Local<v8::Array>& retVal) = 0;
    virtual result_t clear() = 0;

public:
    template<typename T>
    static void __new(const T &args);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_load(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_loadFile(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_dump(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_clear(const v8::FunctionCallbackInfo<v8::Value>& args);
};

}

#include "Buffer.h"

namespace fibjs
{
    inline ClassInfo& X509Crl_base::class_info()
    {
        static ClassData::ClassMethod s_method[] = 
        {
            {"load", s_load, false},
            {"loadFile", s_loadFile, false},
            {"dump", s_dump, false},
            {"clear", s_clear, false}
        };

        static ClassData s_cd = 
        { 
            "X509Crl", s__new, NULL, 
            4, s_method, 0, NULL, 0, NULL, NULL, NULL,
            &object_base::class_info()
        };

        static ClassInfo s_ci(s_cd);
        return s_ci;
    }


    inline void X509Crl_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();
        __new(args);
    }

    template<typename T>void X509Crl_base::__new(const T& args)
    {
        obj_ptr<X509Crl_base> vr;

        CONSTRUCT_ENTER();

        METHOD_OVER(0, 0);

        hr = _new(vr, args.This());

        CONSTRUCT_RETURN();
    }

    inline void X509Crl_base::s_load(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        METHOD_INSTANCE(X509Crl_base);
        METHOD_ENTER();

        METHOD_OVER(1, 1);

        ARG(obj_ptr<Buffer_base>, 0);

        hr = pInst->load(v0);

        METHOD_OVER(1, 1);

        ARG(exlib::string, 0);

        hr = pInst->load(v0);

        METHOD_VOID();
    }

    inline void X509Crl_base::s_loadFile(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        METHOD_INSTANCE(X509Crl_base);
        METHOD_ENTER();

        METHOD_OVER(1, 1);

        ARG(exlib::string, 0);

        hr = pInst->loadFile(v0);

        METHOD_VOID();
    }

    inline void X509Crl_base::s_dump(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        v8::Local<v8::Array> vr;

        METHOD_INSTANCE(X509Crl_base);
        METHOD_ENTER();

        METHOD_OVER(0, 0);

        hr = pInst->dump(vr);

        METHOD_RETURN();
    }

    inline void X509Crl_base::s_clear(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        METHOD_INSTANCE(X509Crl_base);
        METHOD_ENTER();

        METHOD_OVER(0, 0);

        hr = pInst->clear();

        METHOD_VOID();
    }

}

#endif

