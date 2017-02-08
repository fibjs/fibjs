/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _HttpMessage_base_H_
#define _HttpMessage_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "Message.h"

namespace fibjs
{

class Message_base;
class HttpCollection_base;
class List_base;
class Map_base;

class HttpMessage_base : public Message_base
{
    DECLARE_CLASS(HttpMessage_base);

public:
    // HttpMessage_base
    virtual result_t get_protocol(exlib::string& retVal) = 0;
    virtual result_t set_protocol(exlib::string newVal) = 0;
    virtual result_t get_headers(obj_ptr<HttpCollection_base>& retVal) = 0;
    virtual result_t get_keepAlive(bool& retVal) = 0;
    virtual result_t set_keepAlive(bool newVal) = 0;
    virtual result_t get_upgrade(bool& retVal) = 0;
    virtual result_t set_upgrade(bool newVal) = 0;
    virtual result_t get_maxHeadersCount(int32_t& retVal) = 0;
    virtual result_t set_maxHeadersCount(int32_t newVal) = 0;
    virtual result_t get_maxUploadSize(int32_t& retVal) = 0;
    virtual result_t set_maxUploadSize(int32_t newVal) = 0;
    virtual result_t hasHeader(exlib::string name, bool& retVal) = 0;
    virtual result_t firstHeader(exlib::string name, Variant& retVal) = 0;
    virtual result_t allHeader(exlib::string name, obj_ptr<List_base>& retVal) = 0;
    virtual result_t addHeader(Map_base* map) = 0;
    virtual result_t addHeader(exlib::string name, Variant value) = 0;
    virtual result_t setHeader(Map_base* map) = 0;
    virtual result_t setHeader(exlib::string name, Variant value) = 0;
    virtual result_t removeHeader(exlib::string name) = 0;

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        Isolate* isolate = Isolate::current();

        isolate->m_isolate->ThrowException(
            isolate->NewFromUtf8("not a constructor"));
    }

public:
    static void s_get_protocol(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_set_protocol(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args);
    static void s_get_headers(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get_keepAlive(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_set_keepAlive(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args);
    static void s_get_upgrade(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_set_upgrade(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args);
    static void s_get_maxHeadersCount(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_set_maxHeadersCount(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args);
    static void s_get_maxUploadSize(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_set_maxUploadSize(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args);
    static void s_hasHeader(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_firstHeader(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_allHeader(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_addHeader(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_setHeader(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_removeHeader(const v8::FunctionCallbackInfo<v8::Value>& args);
};

}

#include "HttpCollection.h"
#include "List.h"
#include "Map.h"

namespace fibjs
{
    inline ClassInfo& HttpMessage_base::class_info()
    {
        static ClassData::ClassMethod s_method[] = 
        {
            {"hasHeader", s_hasHeader, false},
            {"firstHeader", s_firstHeader, false},
            {"allHeader", s_allHeader, false},
            {"addHeader", s_addHeader, false},
            {"setHeader", s_setHeader, false},
            {"removeHeader", s_removeHeader, false}
        };

        static ClassData::ClassProperty s_property[] = 
        {
            {"protocol", s_get_protocol, s_set_protocol, false},
            {"headers", s_get_headers, block_set, false},
            {"keepAlive", s_get_keepAlive, s_set_keepAlive, false},
            {"upgrade", s_get_upgrade, s_set_upgrade, false},
            {"maxHeadersCount", s_get_maxHeadersCount, s_set_maxHeadersCount, false},
            {"maxUploadSize", s_get_maxUploadSize, s_set_maxUploadSize, false}
        };

        static ClassData s_cd = 
        { 
            "HttpMessage", s__new, NULL, 
            ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, NULL, NULL,
            &Message_base::class_info()
        };

        static ClassInfo s_ci(s_cd);
        return s_ci;
    }

    inline void HttpMessage_base::s_get_protocol(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        exlib::string vr;

        METHOD_INSTANCE(HttpMessage_base);
        PROPERTY_ENTER();

        hr = pInst->get_protocol(vr);

        METHOD_RETURN();
    }

    inline void HttpMessage_base::s_set_protocol(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args)
    {
        METHOD_INSTANCE(HttpMessage_base);
        PROPERTY_ENTER();
        PROPERTY_VAL(exlib::string);

        hr = pInst->set_protocol(v0);

        PROPERTY_SET_LEAVE();
    }

    inline void HttpMessage_base::s_get_headers(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        obj_ptr<HttpCollection_base> vr;

        METHOD_INSTANCE(HttpMessage_base);
        PROPERTY_ENTER();

        hr = pInst->get_headers(vr);

        METHOD_RETURN();
    }

    inline void HttpMessage_base::s_get_keepAlive(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        bool vr;

        METHOD_INSTANCE(HttpMessage_base);
        PROPERTY_ENTER();

        hr = pInst->get_keepAlive(vr);

        METHOD_RETURN();
    }

    inline void HttpMessage_base::s_set_keepAlive(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args)
    {
        METHOD_INSTANCE(HttpMessage_base);
        PROPERTY_ENTER();
        PROPERTY_VAL(bool);

        hr = pInst->set_keepAlive(v0);

        PROPERTY_SET_LEAVE();
    }

    inline void HttpMessage_base::s_get_upgrade(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        bool vr;

        METHOD_INSTANCE(HttpMessage_base);
        PROPERTY_ENTER();

        hr = pInst->get_upgrade(vr);

        METHOD_RETURN();
    }

    inline void HttpMessage_base::s_set_upgrade(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args)
    {
        METHOD_INSTANCE(HttpMessage_base);
        PROPERTY_ENTER();
        PROPERTY_VAL(bool);

        hr = pInst->set_upgrade(v0);

        PROPERTY_SET_LEAVE();
    }

    inline void HttpMessage_base::s_get_maxHeadersCount(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        int32_t vr;

        METHOD_INSTANCE(HttpMessage_base);
        PROPERTY_ENTER();

        hr = pInst->get_maxHeadersCount(vr);

        METHOD_RETURN();
    }

    inline void HttpMessage_base::s_set_maxHeadersCount(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args)
    {
        METHOD_INSTANCE(HttpMessage_base);
        PROPERTY_ENTER();
        PROPERTY_VAL(int32_t);

        hr = pInst->set_maxHeadersCount(v0);

        PROPERTY_SET_LEAVE();
    }

    inline void HttpMessage_base::s_get_maxUploadSize(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        int32_t vr;

        METHOD_INSTANCE(HttpMessage_base);
        PROPERTY_ENTER();

        hr = pInst->get_maxUploadSize(vr);

        METHOD_RETURN();
    }

    inline void HttpMessage_base::s_set_maxUploadSize(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args)
    {
        METHOD_INSTANCE(HttpMessage_base);
        PROPERTY_ENTER();
        PROPERTY_VAL(int32_t);

        hr = pInst->set_maxUploadSize(v0);

        PROPERTY_SET_LEAVE();
    }

    inline void HttpMessage_base::s_hasHeader(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        bool vr;

        METHOD_INSTANCE(HttpMessage_base);
        METHOD_ENTER();

        METHOD_OVER(1, 1);

        ARG(exlib::string, 0);

        hr = pInst->hasHeader(v0, vr);

        METHOD_RETURN();
    }

    inline void HttpMessage_base::s_firstHeader(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        Variant vr;

        METHOD_INSTANCE(HttpMessage_base);
        METHOD_ENTER();

        METHOD_OVER(1, 1);

        ARG(exlib::string, 0);

        hr = pInst->firstHeader(v0, vr);

        METHOD_RETURN();
    }

    inline void HttpMessage_base::s_allHeader(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        obj_ptr<List_base> vr;

        METHOD_INSTANCE(HttpMessage_base);
        METHOD_ENTER();

        METHOD_OVER(1, 1);

        ARG(exlib::string, 0);

        hr = pInst->allHeader(v0, vr);

        METHOD_RETURN();
    }

    inline void HttpMessage_base::s_addHeader(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        METHOD_INSTANCE(HttpMessage_base);
        METHOD_ENTER();

        METHOD_OVER(1, 1);

        ARG(obj_ptr<Map_base>, 0);

        hr = pInst->addHeader(v0);

        METHOD_OVER(2, 2);

        ARG(exlib::string, 0);
        ARG(Variant, 1);

        hr = pInst->addHeader(v0, v1);

        METHOD_VOID();
    }

    inline void HttpMessage_base::s_setHeader(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        METHOD_INSTANCE(HttpMessage_base);
        METHOD_ENTER();

        METHOD_OVER(1, 1);

        ARG(obj_ptr<Map_base>, 0);

        hr = pInst->setHeader(v0);

        METHOD_OVER(2, 2);

        ARG(exlib::string, 0);
        ARG(Variant, 1);

        hr = pInst->setHeader(v0, v1);

        METHOD_VOID();
    }

    inline void HttpMessage_base::s_removeHeader(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        METHOD_INSTANCE(HttpMessage_base);
        METHOD_ENTER();

        METHOD_OVER(1, 1);

        ARG(exlib::string, 0);

        hr = pInst->removeHeader(v0);

        METHOD_VOID();
    }

}

#endif

