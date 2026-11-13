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
#include "encoding_conv.h"

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

result_t invalid_arg_value_text(const char* name, const char* must_be, const exlib::string& received)
{
    result_t::builder b = result_t::make(CALL_E_INVALID_DATA)
                              .with_type_name(errtype::kTypeError)
                              .with_code("ERR_INVALID_ARG_VALUE")
                              .with_format("The argument '%s' %s. Received '%s'",
                                  name, must_be, received.c_str());
    return result_t(std::move(b).arg("name", name));
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

result_t check_encoding(const exlib::string& encoding)
{
    if (encoding.empty() || encoding == "buffer"
        || encoding_conv::is_buffer_codec(encoding) || encoding_conv::is_encoding(encoding))
        return result_t();

    return invalid_arg_value_text("encoding", "is invalid encoding", encoding);
}

result_t dom_error(const char* dom_name, const char* message, const char* code)
{
    exlib::string type_name = errtype::kDOMExceptionPrefix;
    type_name.append(dom_name);

    // No with_code() unless the caller supplied one: DOMException.code is the
    // legacy WHATWG number, so an absent string code is the honest answer.
    if (code && *code)
        return result_t::make(CALL_E_INVALID_DATA)
            .with_type_name(type_name)
            .with_code(code)
            .with_message(message);

    return result_t::make(CALL_E_INVALID_DATA)
        .with_type_name(type_name)
        .with_message(message);
}

result_t from_conversion(result_t hr, const char* name, const Variant& received,
    bool strict)
{
    // Only the three conversion outcomes are relabelled. Anything else (a
    // getter that threw, a null dereference inside the conversion) keeps its
    // own diagnosis instead of being reported as a bad argument.
    const bool type_bad = hr.is_type_mismatch() || hr.is_bad_var_type();
    const bool range_bad = hr.is(CALL_E_OUTRANGE) || hr.is(CALL_E_OVERFLOW);
    const bool missing = hr.is_param_missing() || hr.is(CALL_E_BADPARAMCOUNT);

    if (!type_bad && !range_bad && !missing)
        return hr;

    // A missing argument is only an argument error in the strict pass; the
    // lenient pass exists to let another overload claim the call.
    if (missing && !strict)
        return hr;

    // The value code stays in the overload retry band on purpose -- see the
    // header. `with_result` carries the real code for `number` and the name
    // for the message; it is not the retired sentinel trick, it is the only
    // way to describe a failure whose value code must stay ambiguous.
    result_t::builder b = result_t::make(CALL_E_TYPEMISMATCH).with_result(hr);

    if (range_bad) {
        b = std::move(b)
                .with_type_name(errtype::kRangeError)
                .with_code("ERR_OUT_OF_RANGE")
                .with_format("The value of \"%s\" is out of range. Received %s",
                    name, describe_received(received).c_str());
    } else if (missing) {
        b = std::move(b)
                .with_type_name(errtype::kTypeError)
                .with_code("ERR_INVALID_ARG_TYPE")
                .with_format("The \"%s\" argument is required. Received %s",
                    name, describe_received(received).c_str());
    } else {
        b = std::move(b)
                .with_type_name(errtype::kTypeError)
                .with_code("ERR_INVALID_ARG_TYPE")
                .with_format("The \"%s\" argument has the wrong type. Received %s",
                    name, describe_received(received).c_str());
    }

    return result_t(std::move(b).arg("name", name));
}

result_t from_conversion(result_t hr, const char* name, bool strict)
{
    return from_conversion(hr, name, Variant(), strict);
}

// ---------------------------------------------------------------------------
// Expected-type phrases
//
// The wording is Node's ("must be of type string", "must be an instance of
// Buffer"), because that is what a program porting from Node already matches
// on, and what a developer reading the message expects.
// ---------------------------------------------------------------------------

exlib::string describe_expected_string()
{
    return "must be of type string";
}

exlib::string describe_expected_buffer()
{
    return "must be an instance of Buffer or Uint8Array";
}

exlib::string describe_expected_object(const char* class_name)
{
    if (class_name && *class_name)
        return exlib::string("must be an instance of ") + class_name;
    return "must be of type object";
}

exlib::string describe_expected_number()
{
    return "must be of type number";
}

exlib::string describe_expected_boolean()
{
    return "must be of type boolean";
}

exlib::string describe_expected_function()
{
    return "must be of type function";
}

exlib::string describe_expected_date()
{
    return "must be an instance of Date";
}

exlib::string describe_expected_array(const char* element)
{
    if (element && *element)
        return exlib::string("must be an instance of Array of ") + element;
    return "must be an instance of Array";
}

// A nested conversion may already have described a more specific argument:
// GetConfigValue names the option field it read, and the outer ARG_NAMED /
// OPT_ARG_NAMED must not rename that failure to the whole options parameter.
// Keeping the inner payload is what makes "The \"retry\" argument must be of
// type number" survive an `options` record load.
static bool has_described_argument(result_t hr)
{
    const ErrorPayload* p = hr.payload();
    if (!p)
        return false;

    for (const auto& a : p->args)
        if (a.name == "name")
            return true;

    return false;
}

result_t from_value(result_t hr, const char* name, const Variant& received,
    const char* expected)
{
    if (has_described_argument(hr))
        return hr;

    // Only the conversion outcomes are described. A getter that threw, or an
    // allocation failure, keeps its own diagnosis.
    const bool type_bad = hr.is_type_mismatch() || hr.is_bad_var_type();
    const bool range_bad = hr.is(CALL_E_OUTRANGE) || hr.is(CALL_E_OVERFLOW);
    const bool missing = hr.is_param_missing() || hr.is(CALL_E_BADPARAMCOUNT);

    if (!type_bad && !range_bad && !missing)
        return hr;

    result_t::builder b = result_t::make(hr.code());

    if (range_bad) {
        b = std::move(b)
                .with_type_name(errtype::kRangeError)
                .with_code("ERR_OUT_OF_RANGE")
                .with_format("The value of \"%s\" is out of range.",
                    name ? name : "argument");
    } else if (missing) {
        b = std::move(b)
                .with_type_name(errtype::kTypeError)
                .with_code("ERR_INVALID_ARG_TYPE")
                .with_format("The \"%s\" argument is required. Received %s",
                    name ? name : "argument",
                    describe_received(received).c_str());
    } else if (expected && *expected) {
        // The useful case: the overload knows what it wanted, so the message
        // names the expected type instead of saying "wrong type".
        b = std::move(b)
                .with_type_name(errtype::kTypeError)
                .with_code("ERR_INVALID_ARG_TYPE")
                .with_format("The \"%s\" argument %s. Received %s",
                    name ? name : "argument", expected,
                    describe_received(received).c_str());
    } else {
        b = std::move(b)
                .with_type_name(errtype::kTypeError)
                .with_code("ERR_INVALID_ARG_TYPE")
                .with_format("The \"%s\" argument has the wrong type. Received %s",
                    name ? name : "argument",
                    describe_received(received).c_str());
    }

    if (name && *name)
        return result_t(std::move(b).arg("name", name));
    return result_t(std::move(b));
}

result_t from_value(result_t hr, const char* name, const char* expected)
{
    return from_value(hr, name, Variant(), expected);
}

result_t from_options_record(result_t hr)
{
    if (!hr.failed() || !hr.is_overload_mismatch() || !hr.payload() || !has_described_argument(hr))
        return hr;

    const ErrorPayload* p = hr.payload();
    result_t::builder b = result_t::make(CALL_E_INVALID_DATA).with_result(hr);

    if (!p->error_type_name.empty())
        b = std::move(b).with_type_name(p->error_type_name);
    if (!p->syscall.empty())
        b = std::move(b).with_syscall(p->syscall.c_str());
    if (!p->path.empty())
        b = std::move(b).with_path(p->path);
    if (!p->path2.empty())
        b = std::move(b).with_path2(p->path2);
    if (!p->hostname.empty())
        b = std::move(b).with_hostname(p->hostname.c_str());
    if (!p->code.empty())
        b = std::move(b).with_code(p->code);
    if (!p->message.empty())
        b = std::move(b).with_message(p->message);
    if (!p->func.empty())
        b = std::move(b).with_func(p->func.c_str());
    if (p->has_errno)
        b = std::move(b).with_errno(p->errno_value);
    for (const auto& a : p->args)
        b = std::move(b).arg(a.name.c_str(), a.value);

    return result_t(std::move(b));
}

result_t missing_required_arg(const char* name, int32_t argc, int32_t required)
{
    // Value code stays ambiguous on purpose: METHOD_OVER must still be able to
    // treat this as "this variant does not apply" and try the next one.
    result_t::builder b = result_t::make(CALL_E_PARAMNOTOPTIONAL)
                              .with_type_name(errtype::kTypeError)
                              .with_code("ERR_INVALID_ARG_TYPE");

    if (name && *name) {
        b = std::move(b).with_format("The \"%s\" argument is required. %d argument(s) were passed, %d required.",
            name, argc, required);
        return result_t(std::move(b).arg("name", name));
    }

    return result_t(std::move(b).with_format("%d argument(s) were passed, %d required.",
        argc, required));
}

} // namespace errhelp
} // namespace fibjs
