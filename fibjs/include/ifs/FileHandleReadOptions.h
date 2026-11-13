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

class FileHandleReadOptions : public obj_base {
public:
    LOAD_OPTIONS(FileHandleReadOptions, (buffer)(offset)(length)(position));

public:
    std::optional<obj_ptr<Buffer_base>> buffer;
    std::optional<int32_t> offset = 0;
    std::optional<int32_t> length;
    std::optional<int32_t> position = -1;
};
}
