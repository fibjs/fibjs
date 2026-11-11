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

#include <initializer_list>
#include <type_traits>
#include <variant>

// Variant is used by reference and by value in the signatures below. It is
// forward-declared here on purpose: Variant.h needs v8 and Isolate, so pulling
// it in would make every file that wants an argument error depend on both.
namespace fibjs {
class Variant;
class Buffer_base;
class date_t;

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

// Rewrite a failed ARG / OPT_ARG conversion into a described argument error.
//
// This one is different from the rest of the header, and the difference is the
// whole reason it exists. The argument macros run INSIDE `METHOD_OVER`, whose
// retry protocol decides "this variant does not apply" from the value code:
// `is_overload_mismatch()` is true for -20006..-20001. So the value code has to
// STAY in that band -- returning an out-of-band code here would stop the
// runtime from trying the next overload, and `fs.readFileSync(p, {encoding})`
// would stop falling back to the string form.
//
// The description is therefore split from the value code:
//   value code  CALL_E_TYPEMISMATCH (in band, carries the retry signal)
//   payload     the real code (ERR_INVALID_ARG_TYPE / ERR_INVALID_ARG_VALUE /
//               ERR_OUT_OF_RANGE), the argument name, and the message
// `effective_error()` reports `number` from the payload's result_code, so JS
// still sees 20005 -- the same value it sees today. Only `code`, `args.name`
// and the message become richer, which is what P2 set out to do.
//
// `from_conversion` returns `hr` unchanged when the failure is not one of the
// three conversion outcomes, so an unrelated error inside a getter is not
// relabelled as an argument problem.
result_t from_conversion(result_t hr, const char* name,
    const class Variant& received, bool strict);
result_t from_conversion(result_t hr, const char* name, bool strict);

// The arity failure METHOD_OVER reports when too few arguments were passed.
//
// Same split as from_conversion: the value code stays CALL_E_PARAMNOTOPTIONAL
// (in the retry band, so another overload can still claim the call) while the
// payload carries the description and, when the IDL named it, the parameter.
// `name` may be nullptr for the hand-written wrappers that do not know it.
result_t missing_required_arg(const char* name, int32_t argc, int32_t required);

// ---------------------------------------------------------------------------
// Value-level descriptions
//
// These are what the GetArgumentValue overloads call when a conversion fails.
// A conversion function knows the type it wanted (`exlib::string`,
// `obj_ptr<Buffer_base>`, ...) but not the argument's name, so the split is:
//
//    describe_*   the overload says WHAT it expected, from its own type
//    from_value   deploy caller adds the name it knows, and the code
//
// The value code stays whatever `hr` already carries (in the retry band), for
// the reason spelled out above from_conversion: returning an out-of-band code
// from inside METHOD_OVER would stop the runtime trying the next overload.
// `number` is unaffected too: without with_result, effective_error() reports
// the value code, exactly as before this header existed.

// The expected-type phrase for a failing conversion, or nullptr when the type
// has nothing more useful to say than "wrong type".
exlib::string describe_expected_string();
exlib::string describe_expected_buffer();
exlib::string describe_expected_object(const char* class_name);
exlib::string describe_expected_number();
exlib::string describe_expected_boolean();
exlib::string describe_expected_function();
exlib::string describe_expected_date();
exlib::string describe_expected_array(const char* element);

// Wrap a failed conversion with a description. `expected` may be empty, in
// which case the message falls back to the generic "has the wrong type".
// `name` may be nullptr for a call site that has no argument name.
result_t from_value(result_t hr, const char* name,
    const class Variant& received, const char* expected);
result_t from_value(result_t hr, const char* name, const char* expected);

namespace detail {
// Detects obj_ptr<T> / v8::Local<T>: both are holders whose failure to convert
// means "not the kind of object this parameter wants".
template <typename T>
struct is_object_holder : std::false_type {
};
template <typename T>
struct is_object_holder<obj_ptr<T>> : std::true_type {
};

} // namespace detail
// NOTE: v8::Local<T> cannot be matched here -- errhelp.h does not include
// v8.h, and a partial specialisation needs the template to be declared. The
// specialisation that covers it therefore lives in utils.h, which does include
// v8.h and is where every ARG resides.

// The expected-type phrase for a declared parameter type, or nullptr when the
// type has nothing useful to add.
//
// The argument macro is the only site that knows BOTH things a good message
// needs: what the parameter is called (from the IDL) and what it was declared
// as (the macro's first argument). GetArgumentValue knows the second and not
// the first -- it receives `args[n]` alone -- which is why the description is
// produced here instead of inside the conversion overloads. That also leaves
// the 111 hand-written GetArgumentValue callers untouched, and avoids
// describing one failure twice when an overload delegates to another
// (int32_t goes through int64_t).
//
// nullptr is the honest answer for:
//   * a union (std::variant): its members differ, so naming one would be a
//     lie, and the function that takes the union reports the real problem
//     (a URL that is not a file URL, say);
//   * Variant: it accepts anything.
template <typename T>
struct expected_of {
    static const char* get()
    {
        if constexpr (std::is_same_v<T, exlib::string>)
            return "must be of type string";
        else if constexpr (std::is_same_v<T, bool>)
            return "must be of type boolean";
        else if constexpr (std::is_same_v<T, date_t>)
            return "must be an instance of Date";
        else if constexpr (std::is_arithmetic_v<T>)
            // Integral and floating point alike: the class (TypeError vs
            // RangeError) already tells a range problem from a type problem.
            return "must be of type number";
        else if constexpr (std::is_same_v<T, obj_ptr<Buffer_base>>)
            return "must be an instance of Buffer or Uint8Array";
        else if constexpr (detail::is_object_holder<T>::value)
            return "must be of type object";
        else
            return nullptr;
    }
};

// A union names its members, because "must be one of type string or object" is
// the sentence Node prints and the one a caller needs: it says the value may be
// EITHER form, which is the whole point of the parameter being a union. The
// phrase is built once per union type (a function-local static), so the four
// alternatives of a typical union cost one string each.
template <typename... Ts>
struct expected_of<std::variant<Ts...>> {
    static const char* get()
    {
        static const exlib::string phrase = build();
        return phrase.c_str();
    }

private:
    static exlib::string member_phrase(const char* p)
    {
        // "must be of type string" -> "string"
        if (!p)
            return exlib::string();
        exlib::string s(p);
        static const char* kPrefix = "must be ";
        if (s.length() > qstrlen(kPrefix)
            && s.substr(0, qstrlen(kPrefix)) == kPrefix)
            s = s.substr(qstrlen(kPrefix));

        static const char* kOfType = "of type ";
        size_t at = s.find(kOfType);
        if (at != exlib::string::npos)
            return s.substr(at + qstrlen(kOfType));

        static const char* kInstance = "an instance of ";
        at = s.find(kInstance);
        if (at != exlib::string::npos)
            return s.substr(at + qstrlen(kInstance));

        return s;
    }

    static exlib::string build()
    {
        exlib::string out = "must be one of type ";
        bool first = true;
        (void)std::initializer_list<int>{
            (append_member(out, first, expected_of<Ts>::get()), 0)...};
        return out;
    }

    static void append_member(exlib::string& out, bool& first, const char* p)
    {
        exlib::string m = member_phrase(p);
        if (m.empty())
            return;
        if (!first)
            out.append(" or ");
        out.append(m);
        first = false;
    }
};

} // namespace errhelp
} // namespace fibjs

#endif // fibjs_errhelp_h
