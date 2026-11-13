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

namespace fibjs {

class DiffieHellmanOptions : public obj_base {
public:
    LOAD_OPTIONS(DiffieHellmanOptions, (privateKey)(publicKey));

public:
    std::optional<obj_ptr<KeyObject_base>> privateKey;
    std::optional<obj_ptr<KeyObject_base>> publicKey;
};
}
