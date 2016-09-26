/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _WebView_base_H_
#define _WebView_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "Trigger.h"

namespace fibjs
{

class Trigger_base;

class WebView_base : public Trigger_base
{
    DECLARE_CLASS(WebView_base);

public:
    // WebView_base
    virtual result_t onclose(v8::Local<v8::Function> func, int32_t& retVal) = 0;

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        Isolate* isolate = Isolate::current();

        isolate->m_isolate->ThrowException(
            isolate->NewFromUtf8("not a constructor"));
    }

public:
    static void s_onclose(const v8::FunctionCallbackInfo<v8::Value>& args);
};

}


namespace fibjs
{
    inline ClassInfo& WebView_base::class_info()
    {
        static ClassData::ClassMethod s_method[] = 
        {
            {"onclose", s_onclose, false}
        };

        static ClassData s_cd = 
        { 
            "WebView", s__new, NULL, 
            1, s_method, 0, NULL, 0, NULL, NULL, NULL,
            &Trigger_base::class_info()
        };

        static ClassInfo s_ci(s_cd);
        return s_ci;
    }


    inline void WebView_base::s_onclose(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        int32_t vr;

        METHOD_INSTANCE(WebView_base);
        METHOD_ENTER(1, 1);

        ARG(v8::Local<v8::Function>, 0);

        hr = pInst->onclose(v0, vr);

        METHOD_RETURN();
    }

}

#endif

