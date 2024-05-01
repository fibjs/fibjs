/*
 * RTCDataChannel.cpp
 *
 *  Created on: Apr 27, 2024
 */

#include "object.h"
#include "RTCDataChannel.h"
#include "Buffer.h"

namespace fibjs {

RTCDataChannel::RTCDataChannel(const std::shared_ptr<rtc::DataChannel>& dataChannel)
    : m_dataChannel(dataChannel)
{
    m_dataChannel->onOpen([this]() {
        this->_emit("open");
    });

    m_dataChannel->onClosed([this]() {
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
    close();
}

result_t RTCDataChannel::send(Buffer_base* data)
{
    Buffer* buffer = Buffer::Cast(data);
    m_dataChannel->send((const rtc::byte*)buffer->data(), buffer->length());

    return 0;
}

result_t RTCDataChannel::send(exlib::string data)
{
    m_dataChannel->send(std::string(data));
    return 0;
}

result_t RTCDataChannel::close()
{
    m_dataChannel->resetCallbacks();
    m_dataChannel->close();
    return 0;
}

result_t RTCDataChannel::get_id(int32_t& retVal)
{
    retVal = m_dataChannel->id().value_or(-1);
    return 0;
}

result_t RTCDataChannel::get_label(exlib::string& retVal)
{
    retVal = m_dataChannel->label();
    return 0;
}

result_t RTCDataChannel::get_protocol(exlib::string& retVal)
{
    retVal = m_dataChannel->protocol();
    return 0;
}

result_t RTCDataChannel::get_bufferedAmount(double& retVal)
{
    retVal = m_dataChannel->bufferedAmount();
    return 0;
}

}
