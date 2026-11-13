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

class SignCreateOptions : public obj_base {
public:
    LOAD_OPTIONS(SignCreateOptions, (dsaEncoding)(padding)(saltLength));

public:
    std::optional<exlib::string> dsaEncoding = "der";
    std::optional<int32_t> padding;
    std::optional<int32_t> saltLength;
};
}
