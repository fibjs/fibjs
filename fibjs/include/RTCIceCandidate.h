/*
 * RTCIceCandidate.h
 *
 *  Created on: May 2, 2024
 *      Author: lion
 */

#pragma once

#define RTC_STATIC

#include "ifs/RTCIceCandidate.h"
#include "datachannel/include/rtc/rtc.hpp"

namespace fibjs {

class RTCIceCandidate : public RTCIceCandidate_base {
public:
    RTCIceCandidate(rtc::Candidate& candidate)
        : m_candidate(candidate)
    {
    }

    RTCIceCandidate(exlib::string& candidate, exlib::string& sdpMid)
        : m_candidate(candidate, sdpMid)
    {
    }

public:
    // RTCIceCandidate_base
    virtual result_t get_candidate(exlib::string& retVal);
    virtual result_t get_sdpMid(exlib::string& retVal);
    virtual result_t get_priority(int32_t& retVal);
    virtual result_t get_transport(exlib::string& retVal);
    virtual result_t get_address(exlib::string& retVal);
    virtual result_t get_port(int32_t& retVal);
    virtual result_t get_type(exlib::string& retVal);

public:
    rtc::Candidate m_candidate;
};

} /* namespace fibjs */