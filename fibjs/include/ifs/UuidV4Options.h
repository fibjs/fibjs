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

class UuidV4Options : public obj_base {
public:
    LOAD_OPTIONS(UuidV4Options, (random));

public:
    std::optional<std::variant<obj_ptr<Buffer_base>, v8::Local<v8::Array>>> random;
};
}
