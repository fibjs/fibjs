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

class FileHandleWriteFileOptions : public obj_base {
public:
    LOAD_OPTIONS(FileHandleWriteFileOptions, (encoding));

public:
    std::optional<exlib::string> encoding = "utf8";
};
}
