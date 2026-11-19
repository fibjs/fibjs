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

class RTCIceCandidateInit : public obj_base {
public:
    LOAD_OPTIONS(RTCIceCandidateInit, (candidate)(sdpMid));

public:
    std::optional<exlib::string> candidate = "";
    std::optional<exlib::string> sdpMid;
};
}
