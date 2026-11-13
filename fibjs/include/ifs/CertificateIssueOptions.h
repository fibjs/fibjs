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

class CertificateIssueOptions : public obj_base {
public:
    LOAD_OPTIONS(CertificateIssueOptions, (key)(format)(passphrase)(encoding)(namedCurve)(toX25519)(issuer)(hashAlgorithm)(validFrom)(validTo)(days)(ca)(pathlen)(keyUsage)(type));

public:
    std::optional<std::variant<obj_ptr<KeyObject_base>, obj_ptr<Buffer_base>, exlib::string, obj_ptr<NObject>>> key;
    std::optional<exlib::string> format;
    std::optional<std::variant<exlib::string, obj_ptr<Buffer_base>>> passphrase;
    std::optional<exlib::string> encoding = "utf8";
    std::optional<exlib::string> namedCurve;
    std::optional<bool> toX25519 = false;
    std::optional<obj_ptr<NObject>> issuer;
    std::optional<exlib::string> hashAlgorithm;
    std::optional<date_t> validFrom;
    std::optional<date_t> validTo;
    std::optional<int32_t> days = 100;
    std::optional<bool> ca = false;
    std::optional<int32_t> pathlen = -1;
    std::optional<std::vector<exlib::string>> keyUsage;
    std::optional<std::vector<exlib::string>> type;
};
}
