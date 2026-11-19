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

class WASISocketOption : public obj_base {
public:
    LOAD_OPTIONS(WASISocketOption, (address)(port));

public:
    std::optional<exlib::string> address = "127.0.0.1";
    int32_t port{};
};
}
