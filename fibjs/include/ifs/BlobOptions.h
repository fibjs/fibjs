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

class BlobOptions : public obj_base {
public:
    LOAD_OPTIONS(BlobOptions, (type)(endings));

public:
    std::optional<exlib::string> type = "";
    std::optional<exlib::string> endings = "transparent";
};
}
