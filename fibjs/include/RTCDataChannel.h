/*
 * RTCDataChannel.h
 *
 *  Created on: Apr 27, 2024
 *      Author: lion
 */

#pragma once

#ifndef RTC_STATIC
#define RTC_STATIC
#endif

#include "ifs/RTCDataChannel.h"
#include "datachannel/include/rtc/rtc.hpp"

#include <atomic>

namespace fibjs {

class RTCDataChannel : public RTCDataChannel_base {
public:
    RTCDataChannel(Isolate* isolate, const std::shared_ptr<rtc::DataChannel>& dataChannel, bool negotiated);
    ~RTCDataChannel();

public:
    virtual result_t stop()
    {
        return close();
    }

    // RTCDataChannel_base
    virtual result_t send(Union_send_data data);
    virtual result_t close();
    virtual result_t get_id(int32_t& retVal);
    virtual result_t get_label(exlib::string& retVal);
    virtual result_t get_protocol(exlib::string& retVal);
    virtual result_t get_bufferedAmount(double& retVal);
    virtual result_t get_bufferedAmountLowThreshold(double& retVal);
    virtual result_t set_bufferedAmountLowThreshold(double newVal);
    virtual result_t get_readyState(exlib::string& retVal);
    virtual result_t get_ordered(bool& retVal);
    virtual result_t get_maxPacketLifeTime(int32_t& retVal);
    virtual result_t get_maxRetransmits(int32_t& retVal);
    virtual result_t get_negotiated(bool& retVal);

public:
    virtual result_t onEventChange(exlib::string type, exlib::string ev, v8::Local<v8::Function> func)
    {
        if (!m_inited) {
            m_self = new ValueHolder(wrap());
            m_inited = true;
        }

        return result_t();
    }

private:
    std::shared_ptr<rtc::DataChannel> m_dataChannel;

    // The library exposes the channel state as two booleans and does not keep
    // the negotiated flag, so both are tracked here: the local side knows the
    // flag from the creation options, and an in-band channel is not negotiated.
    bool m_negotiated = false;
    std::atomic<bool> m_closed { false };
    uint32_t m_bufferedAmountLowThreshold = 0;

    bool m_inited = false;
    obj_ptr<ValueHolder> m_self;
};

} /* namespace fibjs */