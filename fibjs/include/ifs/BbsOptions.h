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

class BbsOptions : public obj_base {
public:
    LOAD_OPTIONS(BbsOptions, (key)(format)(type)(passphrase)(encoding)(namedCurve)(toX25519)(suite)(header)(proof_header));

public:
    std::optional<std::variant<obj_ptr<KeyObject_base>, obj_ptr<Buffer_base>, exlib::string, obj_ptr<NObject>>> key;
    std::optional<exlib::string> format;
    std::optional<exlib::string> type;
    std::optional<std::variant<exlib::string, obj_ptr<Buffer_base>>> passphrase;
    std::optional<exlib::string> encoding = "utf8";
    std::optional<exlib::string> namedCurve;
    std::optional<bool> toX25519 = false;
    std::optional<exlib::string> suite = "Bls12381Sha256";
    std::optional<obj_ptr<Buffer_base>> header;
    std::optional<obj_ptr<Buffer_base>> proof_header;
};
}
