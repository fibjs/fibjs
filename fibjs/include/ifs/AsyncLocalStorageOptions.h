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

class AsyncLocalStorageOptions : public obj_base {
public:
    LOAD_OPTIONS(AsyncLocalStorageOptions, (defaultValue)(name));

public:
    std::optional<v8::Local<v8::Value>> defaultValue;
    std::optional<v8::Local<v8::Value>> name;
};
}
