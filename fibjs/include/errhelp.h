/*
 * errhelp.h
 *
 * Node-compatible argument and state errors, in one place.
 *
 * Before this header the four factories lived as private statics in
 * fibjs/src/fs/fs.cpp, so only fs could produce an error Node programs can
 * branch on (`TypeError ERR_INVALID_ARG_VALUE`, `RangeError ERR_OUT_OF_RANGE`).
 * Every other module spelled the same condition by hand, which is why 244
 * sites ended up sharing one code string and most of them carry no argument
 * name at all.
 *
 * Two rules this header exists to enforce:
 *
 *   1. The class (`name`) and the value code (`number`) must agree. Declaring
 *      errtype::kRangeError while passing with_result(invalid_arg()) makes JS
 *      read `name: 'RangeError', number: 20004` -- two channels saying
 *      different things. out_of_range() below pairs them correctly.
 *
 *   2. No new with_result() call. The retired CALL_E_EXCEPTION needed it to
 *      smuggle a code past the overload retry band; every code here is out of
 *      that band already, so the payload carries only the description.
 */

#ifndef fibjs_errhelp_h
#define fibjs_errhelp_h

#include "result.h"

// Variant is used by reference and by value in the signatures below. It is
// forward-declared here on purpose: Variant.h needs v8 and Isolate, so pulling
// it in would make every file that wants an argument error depend on both.
namespace fibjs {
class Variant;

namespace errhelp {

// Node's "Received ..." text for a rejected value: `undefined`, `null`,
// `type number (12)`, `type string ('x')`, `an instance of Array`.
exlib::string describe_received(const Variant& value);

// Node prints NaN / Infinity by name and integers without a fractional part.
exlib::string describe_number(double value);

// The "must be <expected>" clause, e.g. "must be of type string".
result_t invalid_arg_type(const char* name, const char* expected,
    const Variant& received);
// No received value: renders as `undefined`, which is what a missing argument
// is. Kept as an overload so the Variant can stay incomplete here.
result_t invalid_arg_type(const char* name, const char* expected);

// The "is invalid" clause, e.g. "The argument 'flags' is invalid. Received 'zz'".
result_t invalid_arg_value(const char* name, const char* must_be,
    const Variant& received);
result_t invalid_arg_value(const char* name, const char* must_be);

// The "is out of range" clause; the class is RangeError, matching the code.
result_t out_of_range(const char* name, const char* range, const Variant& received);

// A raw-message variant for callers that already rendered the received text
// (fs streams do, from a size they only know as a number).
result_t out_of_range_text(const char* msg);

// The handle is in a state that does not allow the call. `code` is the
// domain's own string (EBADF, ERR_STREAM_DESTROYED, ERR_INVALID_STATE, ...);
// the class stays Error, which is what Node reports for all of them.
result_t closed(const char* code, const char* message);

} // namespace errhelp
} // namespace fibjs

#endif // fibjs_errhelp_h
