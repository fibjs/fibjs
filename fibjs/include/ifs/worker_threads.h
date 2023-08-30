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

class Worker_base;

class worker_threads_base : public object_base {
    DECLARE_CLASS(worker_threads_base);

public:
    // worker_threads_base
    static result_t get_isMainThread(bool& retVal);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_static_get_isMainThread(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
};
}

#include "ifs/Worker.h"

namespace fibjs {
inline ClassInfo& worker_threads_base::class_info()
{
    static ClassData::ClassObject s_object[] = {
        { "Worker", Worker_base::class_info }
    };

    static ClassData::ClassProperty s_property[] = {
        { "isMainThread", s_static_get_isMainThread, block_set, true }
    };

    static ClassData s_cd = {
        "worker_threads", true, s__new, NULL,
        0, NULL, ARRAYSIZE(s_object), s_object, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &object_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void worker_threads_base::s_static_get_isMainThread(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    bool vr;

    PROPERTY_ENTER();

    hr = get_isMainThread(vr);

    METHOD_RETURN();
}
}
