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

class RTCSctpSettings : public obj_base {
public:
    LOAD_OPTIONS(RTCSctpSettings, (recvBufferSize)(sendBufferSize)(maxChunksOnQueue)(initialCongestionWindow)(maxBurst)(congestionControlModule)(delayedSackTimeMs)(minRetransmitTimeoutMs)(maxRetransmitTimeoutMs)(initialRetransmitTimeoutMs)(maxRetransmitAttempts)(heartbeatIntervalMs));

public:
    std::optional<int32_t> recvBufferSize;
    std::optional<int32_t> sendBufferSize;
    std::optional<int32_t> maxChunksOnQueue;
    std::optional<int32_t> initialCongestionWindow;
    std::optional<int32_t> maxBurst;
    std::optional<int32_t> congestionControlModule;
    std::optional<int32_t> delayedSackTimeMs;
    std::optional<int32_t> minRetransmitTimeoutMs;
    std::optional<int32_t> maxRetransmitTimeoutMs;
    std::optional<int32_t> initialRetransmitTimeoutMs;
    std::optional<int32_t> maxRetransmitAttempts;
    std::optional<int32_t> heartbeatIntervalMs;
};
}
