/*
 * errhelp.cpp
 *
 * Implementation of the factories declared in errhelp.h. The message text and
 * the class are Node's; the wording is what a program reading these errors
 * already sees from Node.
 */

#include "errhelp.h"

// Variant.h cannot be included on its own: it needs v8 and fibjs' Isolate.
// utils.h brings both, and errhelp.h only needs the type for its signatures.
#include "utils.h"

#include <cmath>
#include <cstdio>

namespace fibjs {
namespace errhelp {

exlib::string describe_number(double value)
{
    if (std::isnan(value))
        return "NaN";
    if (std::isinf(value))
        return value < 0 ? "-Infinity" : "Infinity";
    if (std::floor(value) == value && std::fabs(value) < 1e15)
        return std::to_string((long long)value);

    char buf[64];
    snprintf(buf, sizeof(buf), "%g", value);
    return buf;
}

exlib::string describe_received(const Variant& value)
{
    switch (value.type()) {
    case Variant::VT_Undefined:
        return "undefined";
    case Variant::VT_Null:
        return "null";
    case Variant::VT_Boolean:
        return exlib::string("type boolean (")
            + (value.boolVal() ? "true" : "false") + ")";
    case Variant::VT_Integer:
    case Variant::VT_Long:
    case Variant::VT_Number:
        return "type number (" + describe_number(value.dblVal()) + ")";
    case Variant::VT_String:
        return "type string ('" + value.string() + "')";
    case Variant::VT_Object:
    case Variant::VT_JSValue: {
        v8::Local<v8::Value> jsv = (v8::Local<v8::Value>)value;

        if (!jsv.IsEmpty() && jsv->IsArray())
            return "an instance of Array";
        return "an instance of Object";
    }
    default:
        return "an instance of Object";
    }
}

result_t invalid_arg_type(const char* name, const char* expected,
    const Variant& received)
{
    result_t::builder b = result_t::make(CALL_E_INVALID_DATA)
                              .with_type_name(errtype::kTypeError)
                              .with_code("ERR_INVALID_ARG_TYPE")
                              .with_format("The \"%s\" argument must be %s. Received %s",
                                  name, expected, describe_received(received).c_str());
    // The name is the point of P2: a caller must be able to tell WHICH
    // argument was rejected without parsing the message.
    return result_t(std::move(b).arg("name", name));
}

result_t invalid_arg_type(const char* name, const char* expected)
{
    return invalid_arg_type(name, expected, Variant());
}

result_t invalid_arg_value(const char* name, const char* must_be,
    const Variant& received)
{
    result_t::builder b = result_t::make(CALL_E_INVALID_DATA)
                              .with_type_name(errtype::kTypeError)
                              .with_code("ERR_INVALID_ARG_VALUE")
                              .with_format("The argument '%s' %s. Received %s",
                                  name, must_be, describe_received(received).c_str());
    return result_t(std::move(b).arg("name", name));
}

result_t invalid_arg_value(const char* name, const char* must_be)
{
    return invalid_arg_value(name, must_be, Variant());
}

result_t out_of_range(const char* name, const char* range,
    const Variant& received)
{
    result_t::builder b = result_t::make(CALL_E_OVERFLOW)
                              .with_type_name(errtype::kRangeError)
                              .with_code("ERR_OUT_OF_RANGE")
                              .with_format("The value of \"%s\" is out of range. It must be %s. Received %s",
                                  name, range, describe_received(received).c_str());
    return result_t(std::move(b).arg("name", name));
}

result_t out_of_range_text(const char* msg)
{
    return result_t::make(CALL_E_OVERFLOW)
        .with_type_name(errtype::kRangeError)
        .with_code("ERR_OUT_OF_RANGE")
        .with_message(msg);
}

result_t closed(const char* code, const char* message)
{
    return result_t::make(CALL_E_INVALID_CALL)
        .with_type_name(errtype::kError)
        .with_code(code)
        .with_message(message);
}

} // namespace errhelp
} // namespace fibjs
