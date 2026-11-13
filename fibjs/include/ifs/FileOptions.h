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

class FileOptions : public obj_base {
public:
    LOAD_OPTIONS(FileOptions, (type)(endings)(lastModified)(data)(name));

public:
    std::optional<exlib::string> type = "";
    std::optional<exlib::string> endings = "transparent";
    std::optional<int64_t> lastModified;
    std::optional<std::variant<obj_ptr<Buffer_base>, exlib::string>> data;
    std::optional<exlib::string> name = "";
};
}
