/*
 * rtc.cpp
 *
 *  Created on: Oct 26, 2024
 */

#include "object.h"
#include "ifs/rtc.h"

#include "datachannel/include/rtc/rtc.hpp"

#ifndef JUICE_STATIC
#define JUICE_STATIC
#endif
#include "juice/include/juice/juice.h"

namespace fibjs {
DECLARE_MODULE(rtc);

result_t rtc_base::startServer(RTCServerOptions* options)
{
    juice_server_config_t server_config = {};
    std::vector<juice_server_credentials> server_credentials;
    std::vector<std::pair<std::string, std::string>> credentials_;

    if (options && options->credentials.has_value()) {
        for (auto& item : options->credentials.value())
            credentials_.push_back(std::make_pair(item.first, item.second));

        for (auto& [username, password] : credentials_) {
            juice_server_credentials cred = {};
            cred.username = username.c_str();
            cred.password = password.c_str();
            server_credentials.push_back(cred);
        }

        server_config.credentials = server_credentials.data();
        server_config.credentials_count = server_credentials.size();
    }

    if (options) {
        server_config.max_allocations = options->maxAllocations.value_or(0);
        server_config.max_peers = options->maxPeers.value_or(0);
        server_config.bind_address = options->bindAddress.has_value() ? options->bindAddress.value().c_str() : nullptr;
        server_config.port = options->port.value_or(3478);
        server_config.relay_port_range_begin = options->relayPortRangeBegin.value_or(0);
        server_config.relay_port_range_end = options->relayPortRangeEnd.value_or(0);
    }

    juice_server_t* server = juice_server_create(&server_config);
    if (!server)
        return result_t::make(CALL_E_INTERNAL).with_type_name(errtype::kError).with_code("ERR_INTERNAL").with_message("Failed to create the server");

    return result_t();
}

result_t rtc_base::setSctpSettings(RTCSctpSettings* options)
{
    // rtc::SctpSettings keeps every field optional and applies the optimized
    // library default to the ones that stay disengaged, so only the fields the
    // caller provided are copied here.
    rtc::SctpSettings s = {};

    if (options) {
        if (options->recvBufferSize.has_value())
            s.recvBufferSize = options->recvBufferSize.value();
        if (options->sendBufferSize.has_value())
            s.sendBufferSize = options->sendBufferSize.value();
        if (options->maxChunksOnQueue.has_value())
            s.maxChunksOnQueue = options->maxChunksOnQueue.value();
        if (options->initialCongestionWindow.has_value())
            s.initialCongestionWindow = options->initialCongestionWindow.value();
        if (options->maxBurst.has_value())
            s.maxBurst = options->maxBurst.value();
        if (options->congestionControlModule.has_value())
            s.congestionControlModule = options->congestionControlModule.value();
        if (options->delayedSackTimeMs.has_value())
            s.delayedSackTime = std::chrono::milliseconds(options->delayedSackTimeMs.value());
        if (options->minRetransmitTimeoutMs.has_value())
            s.minRetransmitTimeout = std::chrono::milliseconds(options->minRetransmitTimeoutMs.value());
        if (options->maxRetransmitTimeoutMs.has_value())
            s.maxRetransmitTimeout = std::chrono::milliseconds(options->maxRetransmitTimeoutMs.value());
        if (options->initialRetransmitTimeoutMs.has_value())
            s.initialRetransmitTimeout = std::chrono::milliseconds(options->initialRetransmitTimeoutMs.value());
        if (options->maxRetransmitAttempts.has_value())
            s.maxRetransmitAttempts = options->maxRetransmitAttempts.value();
        if (options->heartbeatIntervalMs.has_value())
            s.heartbeatInterval = std::chrono::milliseconds(options->heartbeatIntervalMs.value());
    }

    rtc::SetSctpSettings(s);

    return result_t();
}

static rtc::LogLevel s_level = rtc::LogLevel::None;
result_t rtc_base::get_loglevel(exlib::string& retVal)
{
    switch (s_level) {
    case rtc::LogLevel::Verbose:
        retVal = "verbose";
        break;
    case rtc::LogLevel::Debug:
        retVal = "debug";
        break;
    case rtc::LogLevel::Info:
        retVal = "info";
        break;
    case rtc::LogLevel::Warning:
        retVal = "warning";
        break;
    case rtc::LogLevel::Error:
        retVal = "error";
        break;
    case rtc::LogLevel::Fatal:
        retVal = "fatal";
        break;
    case rtc::LogLevel::None:
        retVal = "none";
        break;
    default:
        break;
    }

    return result_t();
}

result_t rtc_base::set_loglevel(exlib::string newVal)
{
    rtc::LogLevel level;
    if (newVal == "verbose")
        level = rtc::LogLevel::Verbose;
    else if (newVal == "debug")
        level = rtc::LogLevel::Debug;
    else if (newVal == "info")
        level = rtc::LogLevel::Info;
    else if (newVal == "warning")
        level = rtc::LogLevel::Warning;
    else if (newVal == "error")
        level = rtc::LogLevel::Error;
    else if (newVal == "fatal")
        level = rtc::LogLevel::Fatal;
    else if (newVal == "none")
        level = rtc::LogLevel::None;
    else
        return result_t::make(CALL_E_INVALID_DATA).with_type_name(errtype::kTypeError).with_code("ERR_INVALID_ARG_VALUE").with_format("Invalid log level: '%s'.", newVal.c_str());

    rtc::InitLogger(level);
    s_level = level;

    return result_t();
}

}
