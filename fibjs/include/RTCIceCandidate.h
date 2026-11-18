/*
 * RTCIceCandidate.h
 *
 *  Created on: May 2, 2024
 *      Author: lion
 */

#pragma once

#ifndef RTC_STATIC
#define RTC_STATIC
#endif

#include "ifs/RTCIceCandidate.h"
#include "datachannel/include/rtc/rtc.hpp"

namespace fibjs {

class RTCIceCandidate : public RTCIceCandidate_base {
public:
    // An empty candidate is the standard's end-of-candidates marker: it has
    // no address and `addIceCandidate` only resolves it, so the wrapper keeps
    // the flag instead of asking the library to parse an empty string.
    RTCIceCandidate()
        : m_empty(true)
    {
    }

    RTCIceCandidate(exlib::string& candidate)
        : m_candidate(candidate)
    {
    }

    RTCIceCandidate(exlib::string& candidate, exlib::string& sdpMid)
        : m_candidate(candidate, sdpMid)
    {
    }

public:
    bool is_empty() const
    {
        return m_empty;
    }

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
    bool m_empty = false;
};

} /* namespace fibjs */