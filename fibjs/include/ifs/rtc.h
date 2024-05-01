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
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }
};
}

#include "ifs/RTCPeerConnection.h"
#include "ifs/RTCSessionDescription.h"
#include "ifs/RTCIceCandidate.h"

namespace fibjs {
inline ClassInfo& rtc_base::class_info()
{
    static ClassData::ClassObject s_object[] = {
        { "RTCPeerConnection", RTCPeerConnection_base::class_info },
        { "RTCSessionDescription", RTCSessionDescription_base::class_info },
        { "RTCIceCandidate", RTCIceCandidate_base::class_info }
    };

    static ClassData s_cd = {
        "rtc", true, s__new, NULL,
        0, NULL, ARRAYSIZE(s_object), s_object, 0, NULL, 0, NULL, NULL, NULL,
        &object_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}
}
