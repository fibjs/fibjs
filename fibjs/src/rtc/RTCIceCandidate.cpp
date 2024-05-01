/*
 * RTCIceCandidate.cpp
 *
 *  Created on: Apr 27, 2024
 */

#include "object.h"
#include "RTCIceCandidate.h"

namespace fibjs {

result_t RTCIceCandidate::get_candidate(exlib::string& retVal)
{
    retVal = m_candidate.candidate();
    return 0;
}

result_t RTCIceCandidate::get_sdpMid(exlib::string& retVal)
{
    retVal = m_candidate.mid();
    return 0;
}

result_t RTCIceCandidate::get_priority(int32_t& retVal)
{
    retVal = m_candidate.priority();
    return 0;
}

result_t RTCIceCandidate::get_transport(exlib::string& retVal)
{
    if (!m_candidate.isResolved())
        return CALL_RETURN_UNDEFINED;

    switch (m_candidate.transportType()) {
    case rtc::Candidate::TransportType::Udp:
        retVal = "udp";
        break;
    case rtc::Candidate::TransportType::TcpActive:
        retVal = "tcp-active";
        break;
    case rtc::Candidate::TransportType::TcpPassive:
        retVal = "tcp-passive";
        break;
    case rtc::Candidate::TransportType::TcpSo:
        retVal = "tcp-so";
        break;
    case rtc::Candidate::TransportType::TcpUnknown:
        retVal = "tcp-unknown";
        break;
    default:
        retVal = "unknown";
        break;
    }

    return 0;
}

result_t RTCIceCandidate::get_address(exlib::string& retVal)
{
    if (!m_candidate.isResolved())
        return CALL_RETURN_UNDEFINED;

    retVal = m_candidate.address().value();
    return 0;
}

result_t RTCIceCandidate::get_port(int32_t& retVal)
{
    if (!m_candidate.isResolved())
        return CALL_RETURN_UNDEFINED;

    retVal = m_candidate.port().value();
    return 0;
}

result_t RTCIceCandidate::get_type(exlib::string& retVal)
{
    switch (m_candidate.type()) {
    case rtc::Candidate::Type::Host:
        retVal = "host";
        break;
    case rtc::Candidate::Type::ServerReflexive:
        retVal = "srflx";
        break;
    case rtc::Candidate::Type::PeerReflexive:
        retVal = "prflx";
        break;
    case rtc::Candidate::Type::Relayed:
        retVal = "relay";
        break;
    default:
        retVal = "unknown";
        break;
    }

    return 0;
}

} /* namespace fibjs */
