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
#include "ifs/EventEmitter.h"

namespace fibjs {

class EventEmitter_base;
class Buffer_base;

class RTCDataChannel_base : public EventEmitter_base {
    DECLARE_CLASS(RTCDataChannel_base);

public:
    // RTCDataChannel_base
    virtual result_t send(Buffer_base* data) = 0;
    virtual result_t send(exlib::string data) = 0;
    virtual result_t close() = 0;
    virtual result_t get_id(int32_t& retVal) = 0;
    virtual result_t get_label(exlib::string& retVal) = 0;
    virtual result_t get_protocol(exlib::string& retVal) = 0;
    virtual result_t get_bufferedAmount(double& retVal) = 0;
    virtual result_t get_onopen(v8::Local<v8::Function>& retVal) = 0;
    virtual result_t set_onopen(v8::Local<v8::Function> newVal) = 0;
    virtual result_t get_onmessage(v8::Local<v8::Function>& retVal) = 0;
    virtual result_t set_onmessage(v8::Local<v8::Function> newVal) = 0;
    virtual result_t get_onclose(v8::Local<v8::Function>& retVal) = 0;
    virtual result_t set_onclose(v8::Local<v8::Function> newVal) = 0;
    virtual result_t get_onerror(v8::Local<v8::Function>& retVal) = 0;
    virtual result_t set_onerror(v8::Local<v8::Function> newVal) = 0;
    virtual result_t get_onbufferedamountlow(v8::Local<v8::Function>& retVal) = 0;
    virtual result_t set_onbufferedamountlow(v8::Local<v8::Function> newVal) = 0;

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_send(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_close(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_id(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_label(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_protocol(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_bufferedAmount(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_onopen(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_onopen(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_onmessage(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_onmessage(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_onclose(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_onclose(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_onerror(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_onerror(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_onbufferedamountlow(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_onbufferedamountlow(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

#include "ifs/Buffer.h"

namespace fibjs {
inline ClassInfo& RTCDataChannel_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "send", s_send, false, ClassData::ASYNC_SYNC },
        { "close", s_close, false, ClassData::ASYNC_SYNC }
    };

    static ClassData::ClassProperty s_property[] = {
        { "id", s_get_id, block_set, false },
        { "label", s_get_label, block_set, false },
        { "protocol", s_get_protocol, block_set, false },
        { "bufferedAmount", s_get_bufferedAmount, block_set, false },
        { "onopen", s_get_onopen, s_set_onopen, false },
        { "onmessage", s_get_onmessage, s_set_onmessage, false },
        { "onclose", s_get_onclose, s_set_onclose, false },
        { "onerror", s_get_onerror, s_set_onerror, false },
        { "onbufferedamountlow", s_get_onbufferedamountlow, s_set_onbufferedamountlow, false }
    };

    static ClassData s_cd = {
        "RTCDataChannel", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &EventEmitter_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void RTCDataChannel_base::s_send(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(RTCDataChannel_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(obj_ptr<Buffer_base>, 0);

    hr = pInst->send(v0);

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = pInst->send(v0);

    METHOD_VOID();
}

inline void RTCDataChannel_base::s_close(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(RTCDataChannel_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->close();

    METHOD_VOID();
}

inline void RTCDataChannel_base::s_get_id(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(RTCDataChannel_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_id(vr);

    METHOD_RETURN();
}

inline void RTCDataChannel_base::s_get_label(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(RTCDataChannel_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_label(vr);

    METHOD_RETURN();
}

inline void RTCDataChannel_base::s_get_protocol(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(RTCDataChannel_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_protocol(vr);

    METHOD_RETURN();
}

inline void RTCDataChannel_base::s_get_bufferedAmount(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    double vr;

    METHOD_INSTANCE(RTCDataChannel_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_bufferedAmount(vr);

    METHOD_RETURN();
}

inline void RTCDataChannel_base::s_get_onopen(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_INSTANCE(RTCDataChannel_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_onopen(vr);

    METHOD_RETURN();
}

inline void RTCDataChannel_base::s_set_onopen(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(RTCDataChannel_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Function>, 0);

    hr = pInst->set_onopen(v0);

    METHOD_VOID();
}

inline void RTCDataChannel_base::s_get_onmessage(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_INSTANCE(RTCDataChannel_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_onmessage(vr);

    METHOD_RETURN();
}

inline void RTCDataChannel_base::s_set_onmessage(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(RTCDataChannel_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Function>, 0);

    hr = pInst->set_onmessage(v0);

    METHOD_VOID();
}

inline void RTCDataChannel_base::s_get_onclose(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_INSTANCE(RTCDataChannel_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_onclose(vr);

    METHOD_RETURN();
}

inline void RTCDataChannel_base::s_set_onclose(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(RTCDataChannel_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Function>, 0);

    hr = pInst->set_onclose(v0);

    METHOD_VOID();
}

inline void RTCDataChannel_base::s_get_onerror(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_INSTANCE(RTCDataChannel_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_onerror(vr);

    METHOD_RETURN();
}

inline void RTCDataChannel_base::s_set_onerror(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(RTCDataChannel_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Function>, 0);

    hr = pInst->set_onerror(v0);

    METHOD_VOID();
}

inline void RTCDataChannel_base::s_get_onbufferedamountlow(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_INSTANCE(RTCDataChannel_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_onbufferedamountlow(vr);

    METHOD_RETURN();
}

inline void RTCDataChannel_base::s_set_onbufferedamountlow(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(RTCDataChannel_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Function>, 0);

    hr = pInst->set_onbufferedamountlow(v0);

    METHOD_VOID();
}
}
