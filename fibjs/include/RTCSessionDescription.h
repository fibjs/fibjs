/*
 * RTCSessionDescription.h
 *
 *  Created on: May 2, 2024
 *      Author: lion
 */

#pragma once

#define RTC_STATIC

#include "ifs/RTCSessionDescription.h"
#include "datachannel/include/rtc/rtc.hpp"

namespace fibjs {

class RTCSessionDescription : public RTCSessionDescription_base {
public:
    RTCSessionDescription(exlib::string& sdp, exlib::string& type)
        : m_desc(sdp, type)
    {
    }

public:
    // RTCSessionDescription_base
    virtual result_t get_type(exlib::string& retVal)
    {
        switch (m_desc.type()) {
        case rtc::Description::Type::Unspec:
            retVal = "unspec";
            break;
        case rtc::Description::Type::Offer:
            retVal = "offer";
            break;
        case rtc::Description::Type::Answer:
            retVal = "answer";
            break;
        case rtc::Description::Type::Pranswer:
            retVal = "pranswer";
            break;
        case rtc::Description::Type::Rollback:
            retVal = "rollback";
            break;
        default:
            retVal = "unknown";
            break;
        }

        return 0;
    }

    virtual result_t get_sdp(exlib::string& retVal)
    {
        retVal = m_desc.generateSdp();
        return 0;
    }

public:
    rtc::Description m_desc;
};

} /* namespace fibjs */