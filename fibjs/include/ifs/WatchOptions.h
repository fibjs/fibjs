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

class WatchOptions : public obj_base {
public:
    LOAD_OPTIONS(WatchOptions, (persistent)(recursive)(encoding));

public:
    std::optional<bool> persistent = true;
    std::optional<bool> recursive = false;
    std::optional<exlib::string> encoding = "utf8";
};
}
