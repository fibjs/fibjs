/*
 * assert.g
 *
 *  Created on: Dec 8, 2024
 *      Author: lion
 */

#include "utils.h"

#pragma once

namespace fibjs {

v8::Local<v8::Value> AssertionError(exlib::string operator_, v8::Local<v8::Value> actual, v8::Local<v8::Value> expected, exlib::string message, v8::Local<v8::Value> property = v8::Local<v8::Value>());

// The message renders any value (assertion messages are output text); an
// undefined message means "no message". This lenient rendering lives here
// instead of an IDL String conversion.
inline result_t _test(bool value, exlib::string operator_, v8::Local<v8::Value> actual, v8::Local<v8::Value> expected, v8::Local<v8::Value> msg, v8::Local<v8::Value> property = v8::Local<v8::Value>())
{
    if (!value) {
        exlib::string message;

        if (!msg.IsEmpty() && !msg->IsUndefined())
            GetArgumentValue(Isolate::current(), msg, message);

        ThrowError(AssertionError(operator_, actual, expected, message, property));
        return CALL_E_JAVASCRIPT;
    }

    return 0;
}

}
