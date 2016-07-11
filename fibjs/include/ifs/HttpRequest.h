/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _HttpRequest_base_H_
#define _HttpRequest_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "HttpMessage.h"

namespace fibjs
{

class HttpMessage_base;
class HttpCollection_base;

class HttpRequest_base : public HttpMessage_base
{
    DECLARE_CLASS(HttpRequest_base);

public:
    // HttpRequest_base
    static result_t _new(obj_ptr<HttpRequest_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    virtual result_t get_method(qstring& retVal) = 0;
    virtual result_t set_method(const char* newVal) = 0;
    virtual result_t get_address(qstring& retVal) = 0;
    virtual result_t set_address(const char* newVal) = 0;
    virtual result_t get_queryString(qstring& retVal) = 0;
    virtual result_t set_queryString(const char* newVal) = 0;
    virtual result_t get_cookies(obj_ptr<HttpCollection_base>& retVal) = 0;
    virtual result_t get_form(obj_ptr<HttpCollection_base>& retVal) = 0;
    virtual result_t get_query(obj_ptr<HttpCollection_base>& retVal) = 0;

public:
    template<typename T>
    static void __new(const T &args);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_method(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_set_method(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args);
    static void s_get_address(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_set_address(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args);
    static void s_get_queryString(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_set_queryString(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args);
    static void s_get_cookies(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get_form(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get_query(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
};

}

#include "HttpCollection.h"

namespace fibjs
{
    inline ClassInfo& HttpRequest_base::class_info()
    {
        static ClassData::ClassProperty s_property[] = 
        {
            {"method", s_get_method, s_set_method, false},
            {"address", s_get_address, s_set_address, false},
            {"queryString", s_get_queryString, s_set_queryString, false},
            {"cookies", s_get_cookies, block_set, false},
            {"form", s_get_form, block_set, false},
            {"query", s_get_query, block_set, false}
        };

        static ClassData s_cd = 
        { 
            "HttpRequest", s__new, NULL, 
            0, NULL, 0, NULL, 6, s_property, NULL, NULL,
            &HttpMessage_base::class_info()
        };

        static ClassInfo s_ci(s_cd);
        return s_ci;
    }

    inline void HttpRequest_base::s_get_method(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        qstring vr;

        PROPERTY_ENTER();
        PROPERTY_INSTANCE(HttpRequest_base);

        hr = pInst->get_method(vr);

        METHOD_RETURN();
    }

    inline void HttpRequest_base::s_set_method(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args)
    {
        PROPERTY_ENTER();
        PROPERTY_INSTANCE(HttpRequest_base);

        PROPERTY_VAL(arg_string);
        hr = pInst->set_method(v0);

        PROPERTY_SET_LEAVE();
    }

    inline void HttpRequest_base::s_get_address(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        qstring vr;

        PROPERTY_ENTER();
        PROPERTY_INSTANCE(HttpRequest_base);

        hr = pInst->get_address(vr);

        METHOD_RETURN();
    }

    inline void HttpRequest_base::s_set_address(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args)
    {
        PROPERTY_ENTER();
        PROPERTY_INSTANCE(HttpRequest_base);

        PROPERTY_VAL(arg_string);
        hr = pInst->set_address(v0);

        PROPERTY_SET_LEAVE();
    }

    inline void HttpRequest_base::s_get_queryString(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        qstring vr;

        PROPERTY_ENTER();
        PROPERTY_INSTANCE(HttpRequest_base);

        hr = pInst->get_queryString(vr);

        METHOD_RETURN();
    }

    inline void HttpRequest_base::s_set_queryString(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args)
    {
        PROPERTY_ENTER();
        PROPERTY_INSTANCE(HttpRequest_base);

        PROPERTY_VAL(arg_string);
        hr = pInst->set_queryString(v0);

        PROPERTY_SET_LEAVE();
    }

    inline void HttpRequest_base::s_get_cookies(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        obj_ptr<HttpCollection_base> vr;

        PROPERTY_ENTER();
        PROPERTY_INSTANCE(HttpRequest_base);

        hr = pInst->get_cookies(vr);

        METHOD_RETURN();
    }

    inline void HttpRequest_base::s_get_form(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        obj_ptr<HttpCollection_base> vr;

        PROPERTY_ENTER();
        PROPERTY_INSTANCE(HttpRequest_base);

        hr = pInst->get_form(vr);

        METHOD_RETURN();
    }

    inline void HttpRequest_base::s_get_query(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        obj_ptr<HttpCollection_base> vr;

        PROPERTY_ENTER();
        PROPERTY_INSTANCE(HttpRequest_base);

        hr = pInst->get_query(vr);

        METHOD_RETURN();
    }

    inline void HttpRequest_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();
        __new(args);
    }

    template<typename T>void HttpRequest_base::__new(const T& args)
    {
        obj_ptr<HttpRequest_base> vr;

        CONSTRUCT_ENTER(0, 0);

        hr = _new(vr, args.This());

        CONSTRUCT_RETURN();
    }

}

#endif

