/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _HttpHandler_base_H_
#define _HttpHandler_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "HandlerEx.h"

namespace fibjs
{

class HandlerEx_base;

class HttpHandler_base : public HandlerEx_base
{
    DECLARE_CLASS(HttpHandler_base);

public:
    // HttpHandler_base
    static result_t _new(v8::Local<v8::Value> hdlr, obj_ptr<HttpHandler_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    virtual result_t get_crossDomain(bool& retVal) = 0;
    virtual result_t set_crossDomain(bool newVal) = 0;
    virtual result_t get_forceGZIP(bool& retVal) = 0;
    virtual result_t set_forceGZIP(bool newVal) = 0;
    virtual result_t get_maxHeadersCount(int32_t& retVal) = 0;
    virtual result_t set_maxHeadersCount(int32_t newVal) = 0;
    virtual result_t get_maxUploadSize(int32_t& retVal) = 0;
    virtual result_t set_maxUploadSize(int32_t newVal) = 0;

public:
    template<typename T>
    static void __new(const T &args);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_crossDomain(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_set_crossDomain(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args);
    static void s_get_forceGZIP(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_set_forceGZIP(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args);
    static void s_get_maxHeadersCount(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_set_maxHeadersCount(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args);
    static void s_get_maxUploadSize(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_set_maxUploadSize(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args);
};

}

namespace fibjs
{
    inline ClassInfo& HttpHandler_base::class_info()
    {
        static ClassData::ClassProperty s_property[] = 
        {
            {"crossDomain", s_get_crossDomain, s_set_crossDomain, false},
            {"forceGZIP", s_get_forceGZIP, s_set_forceGZIP, false},
            {"maxHeadersCount", s_get_maxHeadersCount, s_set_maxHeadersCount, false},
            {"maxUploadSize", s_get_maxUploadSize, s_set_maxUploadSize, false}
        };

        static ClassData s_cd = 
        { 
            "HttpHandler", false, s__new, NULL, 
            0, NULL, 0, NULL, ARRAYSIZE(s_property), s_property, NULL, NULL,
            &HandlerEx_base::class_info()
        };

        static ClassInfo s_ci(s_cd);
        return s_ci;
    }

    inline void HttpHandler_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();
        __new(args);
    }

    template<typename T>void HttpHandler_base::__new(const T& args)
    {
        obj_ptr<HttpHandler_base> vr;

        CONSTRUCT_ENTER();

        METHOD_OVER(1, 1);

        ARG(v8::Local<v8::Value>, 0);

        hr = _new(v0, vr, args.This());

        CONSTRUCT_RETURN();
    }

    inline void HttpHandler_base::s_get_crossDomain(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        bool vr;

        METHOD_INSTANCE(HttpHandler_base);
        PROPERTY_ENTER();

        hr = pInst->get_crossDomain(vr);

        METHOD_RETURN();
    }

    inline void HttpHandler_base::s_set_crossDomain(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args)
    {
        METHOD_INSTANCE(HttpHandler_base);
        PROPERTY_ENTER();
        PROPERTY_VAL(bool);

        hr = pInst->set_crossDomain(v0);

        PROPERTY_SET_LEAVE();
    }

    inline void HttpHandler_base::s_get_forceGZIP(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        bool vr;

        METHOD_INSTANCE(HttpHandler_base);
        PROPERTY_ENTER();

        hr = pInst->get_forceGZIP(vr);

        METHOD_RETURN();
    }

    inline void HttpHandler_base::s_set_forceGZIP(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args)
    {
        METHOD_INSTANCE(HttpHandler_base);
        PROPERTY_ENTER();
        PROPERTY_VAL(bool);

        hr = pInst->set_forceGZIP(v0);

        PROPERTY_SET_LEAVE();
    }

    inline void HttpHandler_base::s_get_maxHeadersCount(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        int32_t vr;

        METHOD_INSTANCE(HttpHandler_base);
        PROPERTY_ENTER();

        hr = pInst->get_maxHeadersCount(vr);

        METHOD_RETURN();
    }

    inline void HttpHandler_base::s_set_maxHeadersCount(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args)
    {
        METHOD_INSTANCE(HttpHandler_base);
        PROPERTY_ENTER();
        PROPERTY_VAL(int32_t);

        hr = pInst->set_maxHeadersCount(v0);

        PROPERTY_SET_LEAVE();
    }

    inline void HttpHandler_base::s_get_maxUploadSize(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        int32_t vr;

        METHOD_INSTANCE(HttpHandler_base);
        PROPERTY_ENTER();

        hr = pInst->get_maxUploadSize(vr);

        METHOD_RETURN();
    }

    inline void HttpHandler_base::s_set_maxUploadSize(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args)
    {
        METHOD_INSTANCE(HttpHandler_base);
        PROPERTY_ENTER();
        PROPERTY_VAL(int32_t);

        hr = pInst->set_maxUploadSize(v0);

        PROPERTY_SET_LEAVE();
    }

}

#endif

