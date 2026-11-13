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
#include "ifs/KeyObject.h"
#include "ifs/Buffer.h"

namespace fibjs {

class KeyOptions : public obj_base {
public:
    LOAD_OPTIONS(KeyOptions, (key)(format)(type)(passphrase)(encoding)(namedCurve)(toX25519));

public:
    std::optional<std::variant<obj_ptr<KeyObject_base>, obj_ptr<Buffer_base>, exlib::string, obj_ptr<NObject>>> key;
    std::optional<exlib::string> format;
    std::optional<exlib::string> type;
    std::optional<std::variant<exlib::string, obj_ptr<Buffer_base>>> passphrase;
    std::optional<exlib::string> encoding = "utf8";
    std::optional<exlib::string> namedCurve;
    std::optional<bool> toX25519 = false;
};
}
