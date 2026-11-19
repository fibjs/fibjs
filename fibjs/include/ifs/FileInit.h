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

class FileInit : public obj_base {
public:
    LOAD_OPTIONS(FileInit, (data)(type)(endings)(lastModified)(name));

public:
    std::variant<obj_ptr<Buffer_base>, exlib::string> data{};
    std::optional<exlib::string> type = "";
    std::optional<exlib::string> endings = "transparent";
    std::optional<int64_t> lastModified;
    std::optional<exlib::string> name = "";
};
}
