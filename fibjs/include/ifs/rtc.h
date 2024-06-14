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

class RTCPeerConnection_base;
class RTCSessionDescription_base;
class RTCIceCandidate_base;

class rtc_base : public object_base {
    DECLARE_CLASS(rtc_base);

public:
    // rtc_base
    static result_t bind(exlib::string bind_address, int32_t local_port, v8::Local<v8::Function> cb);
    static result_t bind(int32_t local_port, v8::Local<v8::Function> cb);
    static result_t unbind();

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_static_bind(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_unbind(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

#include "ifs/RTCPeerConnection.h"
#include "ifs/RTCSessionDescription.h"
#include "ifs/RTCIceCandidate.h"

namespace fibjs {
inline ClassInfo& rtc_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "bind", s_static_bind, true, ClassData::ASYNC_SYNC },
        { "unbind", s_static_unbind, true, ClassData::ASYNC_SYNC }
    };

    static ClassData::ClassObject s_object[] = {
        { "RTCPeerConnection", RTCPeerConnection_base::class_info },
        { "RTCSessionDescription", RTCSessionDescription_base::class_info },
        { "RTCIceCandidate", RTCIceCandidate_base::class_info }
    };

    static ClassData s_cd = {
        "rtc", true, s__new, NULL,
        ARRAYSIZE(s_method), s_method, ARRAYSIZE(s_object), s_object, 0, NULL, 0, NULL, NULL, NULL,
        &object_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void rtc_base::s_static_bind(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(3, 3);

    ARG(exlib::string, 0);
    ARG(int32_t, 1);
    ARG(v8::Local<v8::Function>, 2);

    hr = bind(v0, v1, v2);

    METHOD_OVER(2, 2);

    ARG(int32_t, 0);
    ARG(v8::Local<v8::Function>, 1);

    hr = bind(v0, v1);

    METHOD_VOID();
}

inline void rtc_base::s_static_unbind(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = unbind();

    METHOD_VOID();
}
}
