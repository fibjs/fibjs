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
#include "../OrderedMap.h"
#include "ifs/MapTypes.h"
#include <boost/preprocessor.hpp>

namespace fibjs {

class RTCServerOptions : public obj_base {
public:
    LOAD_OPTIONS(RTCServerOptions, (credentials)(maxAllocations)(maxPeers)(bindAddress)(port)(relayPortRangeBegin)(relayPortRangeEnd));

public:
    std::optional<StringMap> credentials;
    std::optional<int32_t> maxAllocations = 0;
    std::optional<int32_t> maxPeers = 0;
    std::optional<exlib::string> bindAddress;
    std::optional<int32_t> port = 3478;
    std::optional<int32_t> relayPortRangeBegin = 0;
    std::optional<int32_t> relayPortRangeEnd = 0;
};
}
