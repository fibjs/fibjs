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

class RTCDataChannelOptions : public obj_base {
public:
    LOAD_OPTIONS(RTCDataChannelOptions, (ordered)(maxPacketLifeTime)(maxRetransmits)(protocol)(negotiated)(id));

public:
    std::optional<bool> ordered = true;
    std::optional<int32_t> maxPacketLifeTime;
    std::optional<int32_t> maxRetransmits;
    std::optional<exlib::string> protocol = "";
    std::optional<bool> negotiated = false;
    std::optional<int32_t> id;
};
}
