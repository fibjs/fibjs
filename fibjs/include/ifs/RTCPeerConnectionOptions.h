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
#include <boost/preprocessor.hpp>
#include "ifs/RTCIceServer.h"

namespace fibjs {

class RTCPeerConnectionOptions : public obj_base {
public:
    LOAD_OPTIONS(RTCPeerConnectionOptions, (certificateType)(iceTransportPolicy)(iceServers)(maxMessageSize)(enableIceUdpMux)(disableFingerprintVerification)(bindAddress)(port)(iceUfrag)(icePwd)(certPem)(keyPem)(keyPass));

public:
    std::optional<exlib::string> certificateType;
    std::optional<exlib::string> iceTransportPolicy;
    std::optional<std::vector<obj_ptr<RTCIceServer>>> iceServers;
    std::optional<int32_t> maxMessageSize;
    std::optional<bool> enableIceUdpMux;
    std::optional<bool> disableFingerprintVerification;
    std::optional<exlib::string> bindAddress;
    std::optional<int32_t> port;
    std::optional<exlib::string> iceUfrag;
    std::optional<exlib::string> icePwd;
    std::optional<exlib::string> certPem;
    std::optional<exlib::string> keyPem;
    std::optional<exlib::string> keyPass;
};
}
