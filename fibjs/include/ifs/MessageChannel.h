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

class MessagePort_base;

class MessageChannel_base : public object_base {
    DECLARE_CLASS(MessageChannel_base);

public:
    // MessageChannel_base
    static result_t _new(obj_ptr<MessageChannel_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    virtual result_t get_port1(obj_ptr<MessagePort_base>& retVal) = 0;
    virtual result_t get_port2(obj_ptr<MessagePort_base>& retVal) = 0;

public:
    static void __new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static result_t load(v8::Local<v8::Value> v, obj_ptr<MessageChannel_base>& retVal);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_port1(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_port2(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

#include "ifs/MessagePort.h"

namespace fibjs {
inline ClassInfo& MessageChannel_base::class_info()
{
    static ClassData::ClassProperty s_property[] = {
        { "port1", s_get_port1, block_set, false },
        { "port2", s_get_port2, block_set, false }
    };

    static ClassData s_cd = {
        "MessageChannel", false, s__new, NULL,
        0, NULL, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &object_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void MessageChannel_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    CONSTRUCT_INIT();
    __new(args);
}

inline void MessageChannel_base::__new(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<MessageChannel_base> vr;

    CONSTRUCT_ENTER();

    METHOD_OVER(0, 0);

    hr = _new(vr, args.This());

    CONSTRUCT_RETURN();
}

inline result_t MessageChannel_base::load(v8::Local<v8::Value> v, obj_ptr<MessageChannel_base>& retVal)
{
    obj_ptr<MessageChannel_base> vr;

    LOAD_ENTER();

    LOAD_RETURN();
}

inline void MessageChannel_base::s_get_port1(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<MessagePort_base> vr;

    METHOD_INSTANCE(MessageChannel_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_port1(vr);

    METHOD_RETURN();
}

inline void MessageChannel_base::s_get_port2(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<MessagePort_base> vr;

    METHOD_INSTANCE(MessageChannel_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_port2(vr);

    METHOD_RETURN();
}
}
