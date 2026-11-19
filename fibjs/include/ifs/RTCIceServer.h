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

class RTCIceServer : public obj_base {
public:
    LOAD_OPTIONS(RTCIceServer, (urls)(username)(credential));

public:
    std::variant<exlib::string, std::vector<exlib::string>> urls{};
    std::optional<exlib::string> username;
    std::optional<exlib::string> credential;
};
}
