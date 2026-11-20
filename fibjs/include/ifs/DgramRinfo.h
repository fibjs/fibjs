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

class DgramRinfo : public obj_base {
public:
    LOAD_OPTIONS(DgramRinfo, (address)(family)(port)(size));

public:
    exlib::string address{};
    exlib::string family{};
    int32_t port{};
    int32_t size{};
};
}
