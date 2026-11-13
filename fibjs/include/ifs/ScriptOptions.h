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
#include "ifs/Buffer.h"

namespace fibjs {

class ScriptOptions : public obj_base {
public:
    LOAD_OPTIONS(ScriptOptions, (filename)(lineOffset)(columnOffset)(cachedData)(type));

public:
    std::optional<exlib::string> filename = "<anonymous>";
    std::optional<int32_t> lineOffset = 0;
    std::optional<int32_t> columnOffset = 0;
    std::optional<obj_ptr<Buffer_base>> cachedData;
    std::optional<exlib::string> type = "script";
};
}
