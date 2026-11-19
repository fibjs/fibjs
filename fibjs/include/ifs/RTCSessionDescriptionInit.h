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

class RTCSessionDescriptionInit : public obj_base {
public:
    LOAD_OPTIONS(RTCSessionDescriptionInit, (type)(sdp));

public:
    exlib::string type{};
    std::optional<exlib::string> sdp = "";
};
}
