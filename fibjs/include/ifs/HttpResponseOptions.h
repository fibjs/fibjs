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
#include "ifs/Headers.h"

namespace fibjs {

class HttpResponseOptions : public obj_base {
public:
    LOAD_OPTIONS(HttpResponseOptions, (status)(statusText)(headers));

public:
    std::optional<int32_t> status;
    std::optional<exlib::string> statusText;
    std::optional<std::variant<obj_ptr<Headers_base>, obj_ptr<NObject>>> headers;
};
}
