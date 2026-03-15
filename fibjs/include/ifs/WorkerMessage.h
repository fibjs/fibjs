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
#include "ifs/Message.h"

namespace fibjs {

class Message_base;

class WorkerMessage_base : public Message_base {
    DECLARE_CLASS(WorkerMessage_base);

public:
    // WorkerMessage_base
    virtual result_t get_data(v8::Local<v8::Value>& retVal) = 0;

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        ThrowTypeError("not a constructor");
    }

    static result_t load(v8::Local<v8::Value> v, obj_ptr<WorkerMessage_base>& retVal)
    { return CALL_E_TYPEMISMATCH; }

public:
    static void s_get_data(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

namespace fibjs {
inline ClassInfo& WorkerMessage_base::class_info()
{
    static ClassData::ClassProperty s_property[] = {
        { "data", s_get_data, block_set, false }
    };

    static ClassData s_cd = {
        "WorkerMessage", false, s__new, NULL,
        0, NULL, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &Message_base::class_info(),
        true
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void WorkerMessage_base::s_get_data(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Value> vr;

    METHOD_INSTANCE(WorkerMessage_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_data(vr);

    METHOD_RETURN();
}
}
