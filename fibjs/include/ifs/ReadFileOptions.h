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

class ReadFileOptions : public obj_base {
public:
    LOAD_OPTIONS(ReadFileOptions, (encoding)(flag));

public:
    std::optional<exlib::string> encoding = "";
    std::optional<exlib::string> flag = "r";
};
}
