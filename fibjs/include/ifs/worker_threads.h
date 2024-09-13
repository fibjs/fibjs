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
    static result_t get_parentPort(obj_ptr<Worker_base>& retVal);
    static result_t get_workerData(v8::Local<v8::Value>& retVal);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_static_get_isMainThread(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_get_parentPort(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_get_workerData(const v8::FunctionCallbackInfo<v8::Value>& args);
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
        { "isMainThread", s_static_get_isMainThread, block_set, true },
        { "parentPort", s_static_get_parentPort, block_set, true },
        { "workerData", s_static_get_workerData, block_set, true }
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

inline void worker_threads_base::s_static_get_isMainThread(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = get_isMainThread(vr);

    METHOD_RETURN();
}

inline void worker_threads_base::s_static_get_parentPort(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Worker_base> vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = get_parentPort(vr);

    METHOD_RETURN();
}

inline void worker_threads_base::s_static_get_workerData(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Value> vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = get_workerData(vr);

    METHOD_RETURN();
}
}
