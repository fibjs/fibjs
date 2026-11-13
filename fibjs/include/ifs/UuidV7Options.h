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

class UuidV7Options : public obj_base {
public:
    LOAD_OPTIONS(UuidV7Options, (msecs));

public:
    std::optional<int64_t> msecs;
};
}
