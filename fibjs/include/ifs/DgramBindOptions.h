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

class DgramBindOptions : public obj_base {
public:
    LOAD_OPTIONS(DgramBindOptions, (port)(address));

public:
    std::optional<int32_t> port;
    std::optional<exlib::string> address;
};
}
