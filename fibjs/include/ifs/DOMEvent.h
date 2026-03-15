/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#pragma once

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs {

class DOMEvent_base : public object_base {
    DECLARE_CLASS(DOMEvent_base);

public:
    // DOMEvent_base
    static result_t _new(exlib::string type, v8::Local<v8::Object> eventInitDict, obj_ptr<DOMEvent_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    virtual result_t get_type(exlib::string& retVal) = 0;
    virtual result_t get_bubbles(bool& retVal) = 0;
    virtual result_t get_cancelable(bool& retVal) = 0;
    virtual result_t get_composed(bool& retVal) = 0;
    virtual result_t get_defaultPrevented(bool& retVal) = 0;
    virtual result_t get_target(v8::Local<v8::Value>& retVal) = 0;
    virtual result_t get_currentTarget(v8::Local<v8::Value>& retVal) = 0;
    virtual result_t get_timeStamp(double& retVal) = 0;
    virtual result_t stopPropagation() = 0;
    virtual result_t stopImmediatePropagation() = 0;
    virtual result_t preventDefault() = 0;

public:
    static void __new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static result_t load(v8::Local<v8::Value> v, obj_ptr<DOMEvent_base>& retVal);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_type(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_bubbles(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_cancelable(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_composed(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_defaultPrevented(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_target(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_currentTarget(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_timeStamp(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_stopPropagation(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_stopImmediatePropagation(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_preventDefault(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

namespace fibjs {
inline ClassInfo& DOMEvent_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "stopPropagation", s_stopPropagation, false, ClassData::ASYNC_SYNC },
        { "stopImmediatePropagation", s_stopImmediatePropagation, false, ClassData::ASYNC_SYNC },
        { "preventDefault", s_preventDefault, false, ClassData::ASYNC_SYNC }
    };

    static ClassData::ClassProperty s_property[] = {
        { "type", s_get_type, block_set, false },
        { "bubbles", s_get_bubbles, block_set, false },
        { "cancelable", s_get_cancelable, block_set, false },
        { "composed", s_get_composed, block_set, false },
        { "defaultPrevented", s_get_defaultPrevented, block_set, false },
        { "target", s_get_target, block_set, false },
        { "currentTarget", s_get_currentTarget, block_set, false },
        { "timeStamp", s_get_timeStamp, block_set, false }
    };

    static ClassData s_cd = {
        "DOMEvent", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &object_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void DOMEvent_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    CONSTRUCT_INIT();
    __new(args);
}

inline void DOMEvent_base::__new(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<DOMEvent_base> vr;

    CONSTRUCT_ENTER();

    METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate->m_isolate));

    hr = _new(v0, v1, vr, args.This());

    CONSTRUCT_RETURN();
}

inline result_t DOMEvent_base::load(v8::Local<v8::Value> v, obj_ptr<DOMEvent_base>& retVal)
{
    obj_ptr<DOMEvent_base> vr;

    LOAD_ENTER();

    METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate->m_isolate));

    hr = _new(v0, v1, vr, args.This());

    LOAD_RETURN();
}

inline void DOMEvent_base::s_get_type(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(DOMEvent_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_type(vr);

    METHOD_RETURN();
}

inline void DOMEvent_base::s_get_bubbles(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_INSTANCE(DOMEvent_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_bubbles(vr);

    METHOD_RETURN();
}

inline void DOMEvent_base::s_get_cancelable(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_INSTANCE(DOMEvent_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_cancelable(vr);

    METHOD_RETURN();
}

inline void DOMEvent_base::s_get_composed(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_INSTANCE(DOMEvent_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_composed(vr);

    METHOD_RETURN();
}

inline void DOMEvent_base::s_get_defaultPrevented(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_INSTANCE(DOMEvent_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_defaultPrevented(vr);

    METHOD_RETURN();
}

inline void DOMEvent_base::s_get_target(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Value> vr;

    METHOD_INSTANCE(DOMEvent_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_target(vr);

    METHOD_RETURN();
}

inline void DOMEvent_base::s_get_currentTarget(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Value> vr;

    METHOD_INSTANCE(DOMEvent_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_currentTarget(vr);

    METHOD_RETURN();
}

inline void DOMEvent_base::s_get_timeStamp(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    double vr;

    METHOD_INSTANCE(DOMEvent_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_timeStamp(vr);

    METHOD_RETURN();
}

inline void DOMEvent_base::s_stopPropagation(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(DOMEvent_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->stopPropagation();

    METHOD_VOID();
}

inline void DOMEvent_base::s_stopImmediatePropagation(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(DOMEvent_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->stopImmediatePropagation();

    METHOD_VOID();
}

inline void DOMEvent_base::s_preventDefault(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(DOMEvent_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->preventDefault();

    METHOD_VOID();
}
}
