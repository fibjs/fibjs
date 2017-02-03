/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _HandlerEx_base_H_
#define _HandlerEx_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "Handler.h"

namespace fibjs
{

class Handler_base;
class Stats_base;

class HandlerEx_base : public Handler_base
{
    DECLARE_CLASS(HandlerEx_base);

public:
    // HandlerEx_base
    virtual result_t onerror(v8::Local<v8::Object> hdlrs) = 0;
    virtual result_t get_handler(obj_ptr<Handler_base>& retVal) = 0;
    virtual result_t set_handler(Handler_base* newVal) = 0;
    virtual result_t get_stats(obj_ptr<Stats_base>& retVal) = 0;

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        Isolate* isolate = Isolate::current();

        isolate->m_isolate->ThrowException(
            isolate->NewFromUtf8("not a constructor"));
    }

public:
    static void s_onerror(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_handler(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_set_handler(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args);
    static void s_get_stats(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
};

}

#include "Stats.h"

namespace fibjs
{
    inline ClassInfo& HandlerEx_base::class_info()
    {
        static ClassData::ClassMethod s_method[] = 
        {
            {"onerror", s_onerror, false}
        };

        static ClassData::ClassProperty s_property[] = 
        {
            {"handler", s_get_handler, s_set_handler, false},
            {"stats", s_get_stats, block_set, false}
        };

        static ClassData s_cd = 
        { 
            "HandlerEx", s__new, NULL, 
            1, s_method, 0, NULL, 2, s_property, NULL, NULL,
            &Handler_base::class_info()
        };

        static ClassInfo s_ci(s_cd);
        return s_ci;
    }

    inline void HandlerEx_base::s_onerror(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        METHOD_INSTANCE(HandlerEx_base);
        METHOD_ENTER();

        METHOD_OVER(1, 1);

        ARG(v8::Local<v8::Object>, 0);

        hr = pInst->onerror(v0);

        METHOD_VOID();
    }

    inline void HandlerEx_base::s_get_handler(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        obj_ptr<Handler_base> vr;

        METHOD_INSTANCE(HandlerEx_base);
        PROPERTY_ENTER();

        hr = pInst->get_handler(vr);

        METHOD_RETURN();
    }

    inline void HandlerEx_base::s_set_handler(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args)
    {
        METHOD_INSTANCE(HandlerEx_base);
        PROPERTY_ENTER();
        PROPERTY_VAL(obj_ptr<Handler_base>);

        hr = pInst->set_handler(v0);

        PROPERTY_SET_LEAVE();
    }

    inline void HandlerEx_base::s_get_stats(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        obj_ptr<Stats_base> vr;

        METHOD_INSTANCE(HandlerEx_base);
        PROPERTY_ENTER();

        hr = pInst->get_stats(vr);

        METHOD_RETURN();
    }

}

#endif

