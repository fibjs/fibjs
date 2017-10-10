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

namespace fibjs {

class EventEmitter_base;

class Service_base : public EventEmitter_base {
    DECLARE_CLASS(Service_base);

public:
    // Service_base
    static result_t _new(exlib::string name, v8::Local<v8::Function> worker, v8::Local<v8::Object> event, obj_ptr<Service_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    virtual result_t run(AsyncEvent* ac) = 0;
    virtual result_t get_name(exlib::string& retVal) = 0;
    virtual result_t set_name(exlib::string newVal) = 0;
    virtual result_t get_onstop(v8::Local<v8::Function>& retVal) = 0;
    virtual result_t set_onstop(v8::Local<v8::Function> newVal) = 0;
    virtual result_t get_onpause(v8::Local<v8::Function>& retVal) = 0;
    virtual result_t set_onpause(v8::Local<v8::Function> newVal) = 0;
    virtual result_t get_oncontinue(v8::Local<v8::Function>& retVal) = 0;
    virtual result_t set_oncontinue(v8::Local<v8::Function> newVal) = 0;
    static result_t install(exlib::string name, exlib::string cmd, exlib::string displayName, exlib::string description);
    static result_t remove(exlib::string name);
    static result_t start(exlib::string name);
    static result_t stop(exlib::string name);
    static result_t restart(exlib::string name);
    static result_t isInstalled(exlib::string name, bool& retVal);
    static result_t isRunning(exlib::string name, bool& retVal);

public:
    template <typename T>
    static void __new(const T& args);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_run(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_name(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_set_name(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_get_onstop(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_set_onstop(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_get_onpause(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_set_onpause(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_get_oncontinue(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_set_oncontinue(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_install(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_remove(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_start(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_stop(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_restart(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_isInstalled(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_isRunning(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
    ASYNC_MEMBER0(Service_base, run);
};
}

namespace fibjs {
inline ClassInfo& Service_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "run", s_run, false },
        { "runSync", s_run, false },
        { "install", s_install, true },
        { "remove", s_remove, true },
        { "start", s_start, true },
        { "stop", s_stop, true },
        { "restart", s_restart, true },
        { "isInstalled", s_isInstalled, true },
        { "isRunning", s_isRunning, true }
    };

    static ClassData::ClassProperty s_property[] = {
        { "name", s_get_name, s_set_name, false },
        { "onstop", s_get_onstop, s_set_onstop, false },
        { "onpause", s_get_onpause, s_set_onpause, false },
        { "oncontinue", s_get_oncontinue, s_set_oncontinue, false }
    };

    static ClassData s_cd = {
        "Service", false, s__new, NULL,
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

template <typename T>
void Service_base::__new(const T& args)
{
    obj_ptr<Service_base> vr;

    CONSTRUCT_ENTER("new Service()");

    METHOD_OVER(3, 2);

    ARG(exlib::string, 0);
    ARG(v8::Local<v8::Function>, 1);
    OPT_ARG(v8::Local<v8::Object>, 2, v8::Object::New(isolate));

    hr = _new(v0, v1, v2, vr, args.This());

    CONSTRUCT_RETURN();
}

inline void Service_base::s_run(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(Service_base);
    METHOD_ENTER("Service.run");

    ASYNC_METHOD_OVER(0, 0);

    if (!cb.IsEmpty()) {
        pInst->acb_run(cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = pInst->ac_run();

    METHOD_VOID();
}

inline void Service_base::s_get_name(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(Service_base);
    PROPERTY_ENTER("Service.name");

    hr = pInst->get_name(vr);

    METHOD_RETURN();
}

inline void Service_base::s_set_name(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_INSTANCE(Service_base);
    PROPERTY_ENTER("Service.name");
    PROPERTY_VAL(exlib::string);

    hr = pInst->set_name(v0);

    PROPERTY_SET_LEAVE();
}

inline void Service_base::s_get_onstop(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_INSTANCE(Service_base);
    PROPERTY_ENTER("Service.onstop");

    hr = pInst->get_onstop(vr);

    METHOD_RETURN();
}

inline void Service_base::s_set_onstop(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_INSTANCE(Service_base);
    PROPERTY_ENTER("Service.onstop");
    PROPERTY_VAL(v8::Local<v8::Function>);

    hr = pInst->set_onstop(v0);

    PROPERTY_SET_LEAVE();
}

inline void Service_base::s_get_onpause(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_INSTANCE(Service_base);
    PROPERTY_ENTER("Service.onpause");

    hr = pInst->get_onpause(vr);

    METHOD_RETURN();
}

inline void Service_base::s_set_onpause(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_INSTANCE(Service_base);
    PROPERTY_ENTER("Service.onpause");
    PROPERTY_VAL(v8::Local<v8::Function>);

    hr = pInst->set_onpause(v0);

    PROPERTY_SET_LEAVE();
}

inline void Service_base::s_get_oncontinue(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_INSTANCE(Service_base);
    PROPERTY_ENTER("Service.oncontinue");

    hr = pInst->get_oncontinue(vr);

    METHOD_RETURN();
}

inline void Service_base::s_set_oncontinue(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_INSTANCE(Service_base);
    PROPERTY_ENTER("Service.oncontinue");
    PROPERTY_VAL(v8::Local<v8::Function>);

    hr = pInst->set_oncontinue(v0);

    PROPERTY_SET_LEAVE();
}

inline void Service_base::s_install(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER("Service.install");

    METHOD_OVER(4, 2);

    ARG(exlib::string, 0);
    ARG(exlib::string, 1);
    OPT_ARG(exlib::string, 2, "");
    OPT_ARG(exlib::string, 3, "");

    hr = install(v0, v1, v2, v3);

    METHOD_VOID();
}

inline void Service_base::s_remove(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER("Service.remove");

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = remove(v0);

    METHOD_VOID();
}

inline void Service_base::s_start(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER("Service.start");

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = start(v0);

    METHOD_VOID();
}

inline void Service_base::s_stop(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER("Service.stop");

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = stop(v0);

    METHOD_VOID();
}

inline void Service_base::s_restart(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER("Service.restart");

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = restart(v0);

    METHOD_VOID();
}

inline void Service_base::s_isInstalled(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER("Service.isInstalled");

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = isInstalled(v0, vr);

    METHOD_RETURN();
}

inline void Service_base::s_isRunning(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER("Service.isRunning");

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = isRunning(v0, vr);

    METHOD_RETURN();
}
}

#endif
