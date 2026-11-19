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
#include "ifs/WASISocketOption.h"

namespace fibjs {

class WASIHostOptions : public obj_base {
public:
    LOAD_OPTIONS(WASIHostOptions, (version)(args)(env)(preopens)(sockets)(stdin)(stdout)(stderr)(returnOnExit));

public:
    exlib::string version{};
    std::optional<std::vector<v8::Local<v8::Value>>> args;
    std::optional<v8::Local<v8::Object>> env;
    std::optional<v8::Local<v8::Object>> preopens;
    std::optional<std::vector<obj_ptr<WASISocketOption>>> sockets;
    std::optional<int32_t> stdin = 0;
    std::optional<int32_t> stdout = 1;
    std::optional<int32_t> stderr = 2;
    std::optional<bool> returnOnExit = true;
};
}
