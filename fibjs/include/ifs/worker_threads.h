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
class MessagePort_base;
class MessageChannel_base;

class worker_threads_base : public object_base {
    DECLARE_CLASS(worker_threads_base);

public:
    // worker_threads_base
    static result_t get_isMainThread(bool& retVal);
    static result_t get_threadId(int32_t& retVal);
    static result_t get_parentPort(obj_ptr<MessagePort_base>& retVal);
    static result_t get_workerData(v8::Local<v8::Value>& retVal);
    static result_t receiveMessageOnPort(MessagePort_base* port, v8::Local<v8::Value>& retVal);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        ThrowTypeError("not a constructor");
    }

    static result_t load(v8::Local<v8::Value> v, obj_ptr<worker_threads_base>& retVal)
    { return CALL_E_TYPEMISMATCH; }

public:
    static void s_static_get_isMainThread(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_get_threadId(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_get_parentPort(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_get_workerData(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_receiveMessageOnPort(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

#include "ifs/Worker.h"
#include "ifs/MessagePort.h"
#include "ifs/MessageChannel.h"

namespace fibjs {
inline ClassInfo& worker_threads_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "receiveMessageOnPort", s_static_receiveMessageOnPort, true, ClassData::ASYNC_SYNC }
    };

    static ClassData::ClassObject s_object[] = {
        { "Worker", Worker_base::class_info },
        { "MessagePort", MessagePort_base::class_info },
        { "MessageChannel", MessageChannel_base::class_info }
    };

    static ClassData::ClassProperty s_property[] = {
        { "isMainThread", s_static_get_isMainThread, block_set, true },
        { "threadId", s_static_get_threadId, block_set, true },
        { "parentPort", s_static_get_parentPort, block_set, true },
        { "workerData", s_static_get_workerData, block_set, true }
    };

    static ClassData s_cd = {
        "worker_threads", true, s__new, NULL,
        ARRAYSIZE(s_method), s_method, ARRAYSIZE(s_object), s_object, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
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

inline void worker_threads_base::s_static_get_threadId(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = get_threadId(vr);

    METHOD_RETURN();
}

inline void worker_threads_base::s_static_get_parentPort(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<MessagePort_base> vr;

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

inline void worker_threads_base::s_static_receiveMessageOnPort(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Value> vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(obj_ptr<MessagePort_base>, 0);

    hr = receiveMessageOnPort(v0.get(), vr);

    METHOD_RETURN();
}
}
