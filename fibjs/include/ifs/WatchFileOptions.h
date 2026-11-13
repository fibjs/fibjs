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

class WatchFileOptions : public obj_base {
public:
    LOAD_OPTIONS(WatchFileOptions, (persistent)(bigint)(interval));

public:
    std::optional<bool> persistent = true;
    std::optional<bool> bigint = false;
    std::optional<int32_t> interval = 5007;
};
}
