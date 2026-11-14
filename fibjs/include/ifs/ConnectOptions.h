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

class ConnectOptions : public obj_base {
public:
    LOAD_OPTIONS(ConnectOptions, (host)(port)(timeout)(family)(hints));

public:
    std::optional<exlib::string> host = "localhost";
    std::optional<int32_t> port = 0;
    std::optional<int32_t> timeout = 0;
    std::optional<int32_t> family = 0;
    std::optional<int32_t> hints = 0;
};
}
