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
#include "EventEmitter.h"

namespace fibjs
{

class EventEmitter_base;

class Service_base : public EventEmitter_base
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
    virtual result_t get_onstop(v8::Local<v8::Function>& retVal) = 0;
    virtual result_t set_onstop(v8::Local<v8::Function> newVal) = 0;
    virtual result_t get_onpause(v8::Local<v8::Function>& retVal) = 0;
    virtual result_t set_onpause(v8::Local<v8::Function> newVal) = 0;
    virtual result_t get_oncontinue(v8::Local<v8::Function>& retVal) = 0;
    virtual result_t set_oncontinue(v8::Local<v8::Function> newVal) = 0;

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
    static void s_get_onstop(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_set_onstop(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args);
    static void s_get_onpause(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_set_onpause(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args);
    static void s_get_oncontinue(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_set_oncontinue(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args);

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
            {"isRunning", s_isRunning, false}
        };

        static ClassData::ClassProperty s_property[] = 
        {
            {"name", s_get_name, s_set_name, false},
            {"onstop", s_get_onstop, s_set_onstop, false},
            {"onpause", s_get_onpause, s_set_onpause, false},
            {"oncontinue", s_get_oncontinue, s_set_oncontinue, false}
        };

        static ClassData s_cd = 
        { 
            "Service", s__new, NULL, 
            ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, NULL, NULL,
            &EventEmitter_base::class_info()
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

    inline void Service_base::s_get_onstop(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        v8::Local<v8::Function> vr;

        METHOD_INSTANCE(Service_base);
        PROPERTY_ENTER();

        hr = pInst->get_onstop(vr);

        METHOD_RETURN();
    }

    inline void Service_base::s_set_onstop(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args)
    {
        METHOD_INSTANCE(Service_base);
        PROPERTY_ENTER();
        PROPERTY_VAL(v8::Local<v8::Function>);

        hr = pInst->set_onstop(v0);

        PROPERTY_SET_LEAVE();
    }

    inline void Service_base::s_get_onpause(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        v8::Local<v8::Function> vr;

        METHOD_INSTANCE(Service_base);
        PROPERTY_ENTER();

        hr = pInst->get_onpause(vr);

        METHOD_RETURN();
    }

    inline void Service_base::s_set_onpause(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args)
    {
        METHOD_INSTANCE(Service_base);
        PROPERTY_ENTER();
        PROPERTY_VAL(v8::Local<v8::Function>);

        hr = pInst->set_onpause(v0);

        PROPERTY_SET_LEAVE();
    }

    inline void Service_base::s_get_oncontinue(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        v8::Local<v8::Function> vr;

        METHOD_INSTANCE(Service_base);
        PROPERTY_ENTER();

        hr = pInst->get_oncontinue(vr);

        METHOD_RETURN();
    }

    inline void Service_base::s_set_oncontinue(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args)
    {
        METHOD_INSTANCE(Service_base);
        PROPERTY_ENTER();
        PROPERTY_VAL(v8::Local<v8::Function>);

        hr = pInst->set_oncontinue(v0);

        PROPERTY_SET_LEAVE();
    }

}

#endif

