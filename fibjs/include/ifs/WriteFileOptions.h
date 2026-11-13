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

class WriteFileOptions : public obj_base {
public:
    LOAD_OPTIONS(WriteFileOptions, (encoding)(flag)(mode));

public:
    std::optional<exlib::string> encoding = "utf8";
    std::optional<exlib::string> flag = "w";
    std::optional<int32_t> mode = 0666;
};
}
