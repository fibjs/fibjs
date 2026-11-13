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
#include "ifs/KeyEncodingOptions.h"

namespace fibjs {

class GenerateKeyPairOptions : public obj_base {
public:
    LOAD_OPTIONS(GenerateKeyPairOptions, (modulusLength)(publicExponent)(hashAlgorithm)(mgf1Algorithm)(saltLength)(divisorLength)(namedCurve)(paramEncoding)(publicKeyEncoding)(privateKeyEncoding));

public:
    std::optional<int32_t> modulusLength;
    std::optional<int32_t> publicExponent = 65537;
    std::optional<exlib::string> hashAlgorithm;
    std::optional<exlib::string> mgf1Algorithm;
    std::optional<int32_t> saltLength = -1;
    std::optional<int32_t> divisorLength = -1;
    std::optional<exlib::string> namedCurve;
    std::optional<exlib::string> paramEncoding;
    std::optional<obj_ptr<KeyEncodingOptions>> publicKeyEncoding;
    std::optional<obj_ptr<KeyEncodingOptions>> privateKeyEncoding;
};
}
