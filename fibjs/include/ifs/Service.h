/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _Service_base_H_
#define _Service_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "Trigger.h"

namespace fibjs
{

class Trigger_base;

class Service_base : public Trigger_base
{
    DECLARE_CLASS(Service_base);

public:
    // Service_base
    static result_t _new(exlib::string name, v8::Local<v8::Function> worker, v8::Local<v8::Object> event, obj_ptr<Service_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    virtual result_t get_name(exlib::string& retVal) = 0;
    virtual result_t set_name(exlib::string newVal) = 0;
    virtual result_t install(exlib::string cmd, exlib::string displayName, exlib::string description) = 0;
    virtual result_t remove() = 0;
    virtual result_t start() = 0;
    virtual result_t stop() = 0;
    virtual result_t restart() = 0;
    virtual result_t run(AsyncEvent* ac) = 0;
    virtual result_t isInstalled(bool& retVal) = 0;
    virtual result_t isRunning(bool& retVal) = 0;
    virtual result_t onstop(v8::Local<v8::Function> func, int32_t& retVal) = 0;
    virtual result_t onpause(v8::Local<v8::Function> func, int32_t& retVal) = 0;
    virtual result_t oncontinue(v8::Local<v8::Function> func, int32_t& retVal) = 0;

public:
    template<typename T>
    static void __new(const T &args);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_name(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_set_name(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args);
    static void s_install(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_remove(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_start(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_stop(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_restart(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_run(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_isInstalled(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_isRunning(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_onstop(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_onpause(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_oncontinue(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
    ASYNC_MEMBER0(Service_base, run);
};

}

namespace fibjs
{
    inline ClassInfo& Service_base::class_info()
    {
        static ClassData::ClassMethod s_method[] = 
        {
            {"install", s_install, false},
            {"remove", s_remove, false},
            {"start", s_start, false},
            {"stop", s_stop, false},
            {"restart", s_restart, false},
            {"run", s_run, false},
            {"isInstalled", s_isInstalled, false},
            {"isRunning", s_isRunning, false},
            {"onstop", s_onstop, false},
            {"onpause", s_onpause, false},
            {"oncontinue", s_oncontinue, false}
        };

        static ClassData::ClassProperty s_property[] = 
        {
            {"name", s_get_name, s_set_name, false}
        };

        static ClassData s_cd = 
        { 
            "Service", s__new, NULL, 
            11, s_method, 0, NULL, 1, s_property, NULL, NULL,
            &Trigger_base::class_info()
        };

        static ClassInfo s_ci(s_cd);
        return s_ci;
    }

    inline void Service_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();
        __new(args);
    }

    template<typename T>void Service_base::__new(const T& args)
    {
        obj_ptr<Service_base> vr;

        CONSTRUCT_ENTER();

        METHOD_OVER(3, 2);

        ARG(exlib::string, 0);
        ARG(v8::Local<v8::Function>, 1);
        OPT_ARG(v8::Local<v8::Object>, 2, v8::Object::New(isolate));

        hr = _new(v0, v1, v2, vr, args.This());

        CONSTRUCT_RETURN();
    }

    inline void Service_base::s_get_name(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        exlib::string vr;

        METHOD_INSTANCE(Service_base);
        PROPERTY_ENTER();

        hr = pInst->get_name(vr);

        METHOD_RETURN();
    }

    inline void Service_base::s_set_name(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args)
    {
        METHOD_INSTANCE(Service_base);
        PROPERTY_ENTER();
        PROPERTY_VAL(exlib::string);

        hr = pInst->set_name(v0);

        PROPERTY_SET_LEAVE();
    }

    inline void Service_base::s_install(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        METHOD_INSTANCE(Service_base);
        METHOD_ENTER();

        METHOD_OVER(3, 1);

        ARG(exlib::string, 0);
        OPT_ARG(exlib::string, 1, "");
        OPT_ARG(exlib::string, 2, "");

        hr = pInst->install(v0, v1, v2);

        METHOD_VOID();
    }

    inline void Service_base::s_remove(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        METHOD_INSTANCE(Service_base);
        METHOD_ENTER();

        METHOD_OVER(0, 0);

        hr = pInst->remove();

        METHOD_VOID();
    }

    inline void Service_base::s_start(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        METHOD_INSTANCE(Service_base);
        METHOD_ENTER();

        METHOD_OVER(0, 0);

        hr = pInst->start();

        METHOD_VOID();
    }

    inline void Service_base::s_stop(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        METHOD_INSTANCE(Service_base);
        METHOD_ENTER();

        METHOD_OVER(0, 0);

        hr = pInst->stop();

        METHOD_VOID();
    }

    inline void Service_base::s_restart(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        METHOD_INSTANCE(Service_base);
        METHOD_ENTER();

        METHOD_OVER(0, 0);

        hr = pInst->restart();

        METHOD_VOID();
    }

    inline void Service_base::s_run(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        METHOD_INSTANCE(Service_base);
        METHOD_ENTER();

        ASYNC_METHOD_OVER(0, 0);

        if(!cb.IsEmpty()) {
            pInst->acb_run(cb);
            hr = CALL_RETURN_NULL;
        } else
            hr = pInst->ac_run();

        METHOD_VOID();
    }

    inline void Service_base::s_isInstalled(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        bool vr;

        METHOD_INSTANCE(Service_base);
        METHOD_ENTER();

        METHOD_OVER(0, 0);

        hr = pInst->isInstalled(vr);

        METHOD_RETURN();
    }

    inline void Service_base::s_isRunning(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        bool vr;

        METHOD_INSTANCE(Service_base);
        METHOD_ENTER();

        METHOD_OVER(0, 0);

        hr = pInst->isRunning(vr);

        METHOD_RETURN();
    }

    inline void Service_base::s_onstop(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        int32_t vr;

        METHOD_INSTANCE(Service_base);
        METHOD_ENTER();

        METHOD_OVER(1, 1);

        ARG(v8::Local<v8::Function>, 0);

        hr = pInst->onstop(v0, vr);

        METHOD_RETURN();
    }

    inline void Service_base::s_onpause(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        int32_t vr;

        METHOD_INSTANCE(Service_base);
        METHOD_ENTER();

        METHOD_OVER(1, 1);

        ARG(v8::Local<v8::Function>, 0);

        hr = pInst->onpause(v0, vr);

        METHOD_RETURN();
    }

    inline void Service_base::s_oncontinue(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        int32_t vr;

        METHOD_INSTANCE(Service_base);
        METHOD_ENTER();

        METHOD_OVER(1, 1);

        ARG(v8::Local<v8::Function>, 0);

        hr = pInst->oncontinue(v0, vr);

        METHOD_RETURN();
    }

}

#endif

