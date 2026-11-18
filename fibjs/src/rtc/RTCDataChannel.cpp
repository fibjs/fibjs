/*
 * RTCDataChannel.cpp
 *
 *  Created on: Apr 27, 2024
 */

#include "object.h"
#include "RTCDataChannel.h"
#include "Buffer.h"

namespace fibjs {

RTCDataChannel::RTCDataChannel(Isolate* isolate, const std::shared_ptr<rtc::DataChannel>& dataChannel, bool negotiated)
    : m_dataChannel(dataChannel)
    , m_negotiated(negotiated)
{
    holder(isolate);

    // Register the open callback for incoming channels too: the library calls
    // it right after the onDataChannel callback returned, so the `datachannel`
    // event is queued before the `open` event and a handler attached there
    // still sees it, like in a browser.
    m_dataChannel->onOpen([this]() {
        this->_emit("open");
    });

    m_dataChannel->onClosed([this]() {
        m_closed = true;
        this->m_self.Release();
        this->_emit("close");
    });

    m_dataChannel->onError([this](const exlib::string& error) {
        obj_ptr<NObject> ev = new NObject();

        ev->add("error", error);
        this->_emit("error", ev);
    });

    m_dataChannel->onMessage([this](rtc::message_variant data) {
        obj_ptr<NObject> ev = new NObject();

        if (std::holds_alternative<std::string>(data))
            ev->add("data", std::get<std::string>(std::move(data)));
        else {
            auto bin = std::get<rtc::binary>(std::move(data));
            obj_ptr<Buffer> buffer = new Buffer(bin.data(), bin.size());
            ev->add("data", buffer);
        }

        this->_emit("message", ev);
    });

    m_dataChannel->onBufferedAmountLow([this]() {
        this->_emit("bufferedamountlow");
    });
}

RTCDataChannel::~RTCDataChannel()
{
    // Teardown is silent: the object is going away, so no JS wrapper can
    // observe a close event, and the callbacks must not outlive `this`.
    m_closed = true;
    m_dataChannel->resetCallbacks();
    m_dataChannel->close();
}

result_t RTCDataChannel::send(Union_send_data data)
{
    try {
        if (std::holds_alternative<exlib::string>(data))
            m_dataChannel->send(std::string(std::get<exlib::string>(data)));
        else {
            Buffer* buffer = Buffer::Cast(std::get<obj_ptr<Buffer_base>>(data));
            m_dataChannel->send((const rtc::byte*)buffer->data(), buffer->length());
        }
    } catch (std::exception& e) {
        return result_t::make(CALL_E_INVALID_DATA).with_type_name(errtype::kError).with_code("ERR_RTC_ERROR").with_message(e.what());
    }

    return result_t();
}

result_t RTCDataChannel::close()
{
    if (m_closed.exchange(true))
        return result_t();

    // libdatachannel runs the closed callback synchronously on the first
    // close, so the event is delivered through the same path as a close by
    // the peer. Closing an already closed channel is a no-op.
    m_dataChannel->close();
    m_self.Release();

    return result_t();
}

result_t RTCDataChannel::get_id(int32_t& retVal)
{
    // 65535 is the sentinel of an id that has not been negotiated yet.
    auto id = m_dataChannel->id();
    retVal = id.has_value() ? (int32_t)*id : 65535;
    return result_t();
}

result_t RTCDataChannel::get_label(exlib::string& retVal)
{
    retVal = m_dataChannel->label();
    return result_t();
}

result_t RTCDataChannel::get_protocol(exlib::string& retVal)
{
    retVal = m_dataChannel->protocol();
    return result_t();
}

result_t RTCDataChannel::get_bufferedAmount(double& retVal)
{
    retVal = m_dataChannel->bufferedAmount();
    return result_t();
}

result_t RTCDataChannel::get_bufferedAmountLowThreshold(double& retVal)
{
    retVal = m_bufferedAmountLowThreshold;
    return result_t();
}

result_t RTCDataChannel::set_bufferedAmountLowThreshold(double newVal)
{
    if (newVal < 0)
        return result_t::make(CALL_E_INVALID_DATA).with_type_name(errtype::kTypeError).with_code("ERR_INVALID_ARG_VALUE").with_format("The value of 'bufferedAmountLowThreshold' is out of range. It must be >= 0. Received %g", newVal);

    m_bufferedAmountLowThreshold = (uint32_t)newVal;
    m_dataChannel->setBufferedAmountLowThreshold(m_bufferedAmountLowThreshold);

    return result_t();
}

result_t RTCDataChannel::get_readyState(exlib::string& retVal)
{
    // The library closes a channel in one step, so the standard `closing`
    // state is never observed.
    if (m_closed || m_dataChannel->isClosed())
        retVal = "closed";
    else if (m_dataChannel->isOpen())
        retVal = "open";
    else
        retVal = "connecting";

    return result_t();
}

result_t RTCDataChannel::get_ordered(bool& retVal)
{
    retVal = !m_dataChannel->reliability().unordered;
    return result_t();
}

result_t RTCDataChannel::get_maxPacketLifeTime(int32_t& retVal)
{
    auto reliability = m_dataChannel->reliability();
    if (!reliability.maxPacketLifeTime.has_value())
        return result_t::return_null();

    retVal = (int32_t)reliability.maxPacketLifeTime->count();
    return result_t();
}

result_t RTCDataChannel::get_maxRetransmits(int32_t& retVal)
{
    auto reliability = m_dataChannel->reliability();
    if (!reliability.maxRetransmits.has_value())
        return result_t::return_null();

    retVal = (int32_t)reliability.maxRetransmits.value();
    return result_t();
}

result_t RTCDataChannel::get_negotiated(bool& retVal)
{
    retVal = m_negotiated;
    return result_t();
}

}
