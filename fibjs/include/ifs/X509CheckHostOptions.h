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

class X509CheckHostOptions : public obj_base {
public:
    LOAD_OPTIONS(X509CheckHostOptions, (subject)(wildcards)(partialWildcards)(multiLabelWildcards)(singleLabelSubdomains));

public:
    std::optional<exlib::string> subject = "default";
    std::optional<bool> wildcards = true;
    std::optional<bool> partialWildcards = true;
    std::optional<bool> multiLabelWildcards = false;
    std::optional<bool> singleLabelSubdomains = false;
};
}
