/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _X509Req_base_H_
#define _X509Req_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs
{

class PKey_base;
class hash_base;
class Buffer_base;
class X509Cert_base;

class X509Req_base : public object_base
{
    DECLARE_CLASS(X509Req_base);

public:
    // X509Req_base
    static result_t _new(obj_ptr<X509Req_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    static result_t _new(exlib::string subject, PKey_base* key, int32_t hash, obj_ptr<X509Req_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    virtual result_t load(Buffer_base* derReq) = 0;
    virtual result_t load(exlib::string pemReq) = 0;
    virtual result_t loadFile(exlib::string filename) = 0;
    virtual result_t exportPem(exlib::string& retVal) = 0;
    virtual result_t exportDer(obj_ptr<Buffer_base>& retVal) = 0;
    virtual result_t sign(exlib::string issuer, PKey_base* key, v8::Local<v8::Object> opts, obj_ptr<X509Cert_base>& retVal, AsyncEvent* ac) = 0;
    virtual result_t get_subject(exlib::string& retVal) = 0;
    virtual result_t get_publicKey(obj_ptr<PKey_base>& retVal) = 0;

public:
    template<typename T>
    static void __new(const T &args);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_load(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_loadFile(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_exportPem(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_exportDer(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_sign(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_subject(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get_publicKey(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);

public:
    ASYNC_MEMBERVALUE4(X509Req_base, sign, exlib::string, PKey_base*, v8::Local<v8::Object>, obj_ptr<X509Cert_base>);
};

}

#include "PKey.h"
#include "hash.h"
#include "Buffer.h"
#include "X509Cert.h"

namespace fibjs
{
    inline ClassInfo& X509Req_base::class_info()
    {
        static ClassData::ClassMethod s_method[] = 
        {
            {"load", s_load, false},
            {"loadFile", s_loadFile, false},
            {"exportPem", s_exportPem, false},
            {"exportDer", s_exportDer, false},
            {"sign", s_sign, false}
        };

        static ClassData::ClassProperty s_property[] = 
        {
            {"subject", s_get_subject, block_set, false},
            {"publicKey", s_get_publicKey, block_set, false}
        };

        static ClassData s_cd = 
        { 
            "X509Req", s__new, NULL, 
            5, s_method, 0, NULL, 2, s_property, NULL, NULL,
            &object_base::class_info()
        };

        static ClassInfo s_ci(s_cd);
        return s_ci;
    }

    inline void X509Req_base::s_get_subject(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        exlib::string vr;

        METHOD_INSTANCE(X509Req_base);
        PROPERTY_ENTER();

        hr = pInst->get_subject(vr);

        METHOD_RETURN();
    }

    inline void X509Req_base::s_get_publicKey(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        obj_ptr<PKey_base> vr;

        METHOD_INSTANCE(X509Req_base);
        PROPERTY_ENTER();

        hr = pInst->get_publicKey(vr);

        METHOD_RETURN();
    }

    inline void X509Req_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();
        __new(args);
    }

    template<typename T>void X509Req_base::__new(const T& args)
    {
        obj_ptr<X509Req_base> vr;

        CONSTRUCT_ENTER();

        METHOD_OVER(0, 0);

        hr = _new(vr, args.This());

        METHOD_OVER(3, 2);

        ARG(exlib::string, 0);
        ARG(obj_ptr<PKey_base>, 1);
        OPT_ARG(int32_t, 2, hash_base::_SHA1);

        hr = _new(v0, v1, v2, vr, args.This());

        CONSTRUCT_RETURN();
    }

    inline void X509Req_base::s_load(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        METHOD_INSTANCE(X509Req_base);
        METHOD_ENTER();

        METHOD_OVER(1, 1);

        ARG(obj_ptr<Buffer_base>, 0);

        hr = pInst->load(v0);

        METHOD_OVER(1, 1);

        ARG(exlib::string, 0);

        hr = pInst->load(v0);

        METHOD_VOID();
    }

    inline void X509Req_base::s_loadFile(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        METHOD_INSTANCE(X509Req_base);
        METHOD_ENTER();

        METHOD_OVER(1, 1);

        ARG(exlib::string, 0);

        hr = pInst->loadFile(v0);

        METHOD_VOID();
    }

    inline void X509Req_base::s_exportPem(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        exlib::string vr;

        METHOD_INSTANCE(X509Req_base);
        METHOD_ENTER();

        METHOD_OVER(0, 0);

        hr = pInst->exportPem(vr);

        METHOD_RETURN();
    }

    inline void X509Req_base::s_exportDer(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        obj_ptr<Buffer_base> vr;

        METHOD_INSTANCE(X509Req_base);
        METHOD_ENTER();

        METHOD_OVER(0, 0);

        hr = pInst->exportDer(vr);

        METHOD_RETURN();
    }

    inline void X509Req_base::s_sign(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        obj_ptr<X509Cert_base> vr;

        METHOD_INSTANCE(X509Req_base);
        METHOD_ENTER();

        ASYNC_METHOD_OVER(3, 2);

        ARG(exlib::string, 0);
        ARG(obj_ptr<PKey_base>, 1);
        OPT_ARG(v8::Local<v8::Object>, 2, v8::Object::New(isolate));

        if(!cb.IsEmpty()) {
            pInst->acb_sign(v0, v1, v2, cb);
            hr = CALL_RETURN_NULL;
        } else
            hr = pInst->ac_sign(v0, v1, v2, vr);

        METHOD_RETURN();
    }

}

#endif

