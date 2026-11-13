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

class X509CheckEmailOptions : public obj_base {
public:
    LOAD_OPTIONS(X509CheckEmailOptions, (subject));

public:
    std::optional<exlib::string> subject = "default";
};
}
