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

namespace fibjs {

class RTCListenerOptions : public obj_base {
public:
    LOAD_OPTIONS(RTCListenerOptions, (certPem)(keyPem)(keyPass)(maxMessageSize)(maxConnections)(maxConnectionsPerAddress)(authorize));

public:
    std::optional<exlib::string> certPem = "";
    std::optional<exlib::string> keyPem = "";
    std::optional<exlib::string> keyPass = "";
    std::optional<int32_t> maxMessageSize = 262144;
    std::optional<int32_t> maxConnections = 1024;
    std::optional<int32_t> maxConnectionsPerAddress = 16;
    std::optional<v8::Local<v8::Function>> authorize;
};
}
