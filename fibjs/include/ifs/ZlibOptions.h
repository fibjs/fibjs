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

class ZlibOptions : public obj_base {
public:
    LOAD_OPTIONS(ZlibOptions, (level)(windowBits)(memLevel)(strategy));

public:
    std::optional<int32_t> level = -1;
    std::optional<int32_t> windowBits = 0;
    std::optional<int32_t> memLevel = 8;
    std::optional<int32_t> strategy = 0;
};
}
