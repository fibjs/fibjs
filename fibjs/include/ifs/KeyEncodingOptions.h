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

class KeyEncodingOptions : public obj_base {
public:
    LOAD_OPTIONS(KeyEncodingOptions, (format)(type)(cipher)(passphrase));

public:
    std::optional<exlib::string> format;
    std::optional<exlib::string> type;
    std::optional<exlib::string> cipher;
    std::optional<std::variant<exlib::string, obj_ptr<Buffer_base>>> passphrase;
};
}
