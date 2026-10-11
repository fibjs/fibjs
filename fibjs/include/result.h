/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#pragma once

// The error channel of every fibjs operation, below utils.h: Isolate.h and
// the fiber plumbing name result_t too (call_pipe, Unref, sync lambdas), so
// the type cannot live in utils.h, which includes Isolate.h.
//
// result_t is a class (not a bare int) so the common checks become named
// predicates instead of hard coded comparisons and, in a later step of the
// migration, the structured error payload can ride with the value. Until then
// it still converts to and from int32_t implicitly, so existing call sites
// keep compiling unchanged.

#include <errno.h>
#include <stdint.h>

#ifdef _WIN32
#include <windows.h>
#endif

namespace fibjs {

#define CALL_RETURN_NULL 100000
#define CALL_RETURN_UNDEFINED 100001

#define CALL_E_MAX -20000
// Invalid number of parameters.
#define CALL_E_BADPARAMCOUNT (CALL_E_MAX - 1)
// Parameter not optional.
#define CALL_E_PARAMNOTOPTIONAL (CALL_E_MAX - 2)
// The input parameter is not a valid type.
#define CALL_E_BADVARTYPE (CALL_E_MAX - 3)
// Invalid argument.
#define CALL_E_INVALIDARG (CALL_E_MAX - 4)
// The argument could not be coerced to the specified type.
#define CALL_E_TYPEMISMATCH (CALL_E_MAX - 5)
// Value is out of range.
#define CALL_E_OUTRANGE (CALL_E_MAX - 6)

#define CALL_E_MIN_ARG (CALL_E_OUTRANGE - 1)

// Constructor cannot be called as a function.
#define CALL_E_CONSTRUCTOR (CALL_E_MAX - 7)
// Object is not an instance of declaring class.
#define CALL_E_NOTINSTANCE (CALL_E_MAX - 8)
// Invalid procedure call.
#define CALL_E_INVALID_CALL (CALL_E_MAX - 9)
//  Re-entrant calls are not allowed.
#define CALL_E_REENTRANT (CALL_E_MAX - 10)
// Invalid input data
#define CALL_E_INVALID_DATA (CALL_E_MAX - 11)
// Index was out of range.
#define CALL_E_BADINDEX (CALL_E_MAX - 12)
// Memory overflow error.
#define CALL_E_OVERFLOW (CALL_E_MAX - 13)
// Collection is empty.
#define CALL_E_EMPTY (CALL_E_MAX - 14)
// Operation now in progress.
#define CALL_E_PENDDING (CALL_E_MAX - 15)
// Operation not support asynchronous call.
#define CALL_E_NOASYNC (CALL_E_MAX - 16)
// Operation not support synchronous call.
#define CALL_E_NOSYNC (CALL_E_MAX - 17)
// Operation is long synchronous call.
#define CALL_E_LONGSYNC (CALL_E_MAX - 18)
// Operation is GUI call.
#define CALL_E_GUICALL (CALL_E_MAX - 19)
// Internal error.
#define CALL_E_INTERNAL (CALL_E_MAX - 20)
// The maximum amount of time for a script to execute was exceeded.
#define CALL_E_TIMEOUT (CALL_E_MAX - 21)
// Operation was aborted.
#define CALL_E_ABORT (CALL_E_MAX - 22)
// Invalid return type.
#define CALL_E_RETURN_TYPE (CALL_E_MAX - 23)
// Exception occurred.
#define CALL_E_EXCEPTION (CALL_E_MAX - 24)
// JavaScript error.
#define CALL_E_JAVASCRIPT (CALL_E_MAX - 25)
// Permission denied.
#define CALL_E_PERMIT (CALL_E_MAX - 26)
// Object closed.
#define CALL_E_CLOSED (CALL_E_MAX - 27)
// Resource is busy (e.g. an active cursor on the connection).
#define CALL_E_BUSY (CALL_E_MAX - 28)

#define CALL_E_MIN (CALL_E_MAX - 100)

#ifndef _WIN32
#define CALL_E_FILE_NOT_FOUND (-ENOENT)
#define CALL_E_PATH_NOT_FOUND (-ENOENT)
#define CALL_E_BAD_FILE (-EBADF)
#define CALL_E_NETNAME_DELETED (-ENOENT)
#define CALL_E_CLOSED_SOCKET (-EPERM)
#else
#define CALL_E_FILE_NOT_FOUND (-ERROR_FILE_NOT_FOUND)
#define CALL_E_PATH_NOT_FOUND (-ERROR_PATH_NOT_FOUND)
#define CALL_E_BAD_FILE (-ERROR_OPERATION_ABORTED)
#define CALL_E_NETNAME_DELETED (-ERROR_NETNAME_DELETED)
#define CALL_E_CLOSED_SOCKET CALL_E_BAD_FILE
#endif

// Return code of every fibjs operation. It is a class (not a bare int) so the
// common checks become named predicates instead of hard coded comparisons and,
// in a later step of the migration, the structured error payload can ride with
// the value. Until then it still converts to and from int32_t implicitly, so
// existing call sites keep compiling unchanged.
class result_t {
public:
    int32_t code = 0;

    result_t() = default;
    result_t(int32_t c)
        : code(c)
    {
    }

    // Compatibility conversion; removed in P4 of the migration. Deprecating it
    // first lists every remaining decode site (warning, build stays green)
    // instead of breaking the tree in one go.
    [[deprecated("decode explicitly: use a result_t predicate, or .code for a raw value")]] operator int32_t() const
    {
        return code;
    }

    // Semantic checks (P2 replaces the hard coded comparisons with these).
    bool failed() const
    {
        return code < 0;
    }

    bool ok() const
    {
        return code >= 0;
    }

    // A failure that is not <c>: the "propagate anything but this code" idiom
    // (`failed() && !is(c)`), e.g. GetConfigValue's strict mode reports
    // CALL_E_PARAMNOTOPTIONAL for an absent option.
    bool failed_other_than(int32_t c) const
    {
        return code < 0 && code != c;
    }

    bool is(int32_t c) const
    {
        return code == c;
    }

    // A real system error (errno domain): the internal CALL_E_* codes live
    // below CALL_E_MAX, and success (0) must not classify as one either.
    bool is_system_error() const
    {
        return code < 0 && code > CALL_E_MAX;
    }

    bool is_internal_error() const
    {
        return code <= CALL_E_MAX;
    }

    int32_t errno_value() const
    {
        return is_system_error() ? -code : 0;
    }

    bool is_pending() const
    {
        return code == CALL_E_PENDDING;
    }

    bool is_nosync() const
    {
        return code == CALL_E_NOSYNC;
    }

    bool is_exception() const
    {
        return code == CALL_E_EXCEPTION;
    }

    bool is_javascript() const
    {
        return code == CALL_E_JAVASCRIPT;
    }

    bool is_return_null() const
    {
        return code == CALL_RETURN_NULL;
    }

    bool is_return_undefined() const
    {
        return code == CALL_RETURN_UNDEFINED;
    }

    // The "nullable read" family (collection / header / body / child getters)
    // reports "there is a value" with 0 and "nothing there" with
    // CALL_RETURN_NULL / CALL_RETURN_UNDEFINED. When this holds, the read set
    // its out-parameter, so an extra `&& value` test next to it is noise.
    // NOTE the polarity at "is a code provided" call sites (e.g. EventInfo):
    // WebSocket close codes are positive (1000-4999), so "a code was given"
    // is `!has_value()`, not `failed()`.
    bool has_value() const
    {
        return code == 0;
    }

    // Strict option parsing (`GetConfigValue(..., true)`) reports this when the
    // option was not provided at all.
    bool is_param_missing() const
    {
        return code == CALL_E_PARAMNOTOPTIONAL;
    }

    // The "no such file" family: on POSIX both macros are -ENOENT, on Windows
    // they are distinct codes, so the family is their union. A check for one
    // of the two codes must stay `is(CALL_E_...)`: using this predicate there
    // would accept the other code as well on Windows.
    bool is_not_found() const
    {
        return code == CALL_E_FILE_NOT_FOUND || code == CALL_E_PATH_NOT_FOUND;
    }

    // The value could not be converted to the type the API asked for.
    bool is_type_mismatch() const
    {
        return code == CALL_E_TYPEMISMATCH;
    }

    // The input parameter is not a valid type: the inner collection / variant
    // conversions report this and the callers remap it to a type error.
    bool is_bad_var_type() const
    {
        return code == CALL_E_BADVARTYPE;
    }

    // A failure about how the arguments were provided: the overload arity, a
    // missing strict option, or a value that does not convert. The argument
    // bridge retries these leniently (see CHECK_ARGUMENT).
    bool is_arg_error() const
    {
        return code == CALL_E_BADPARAMCOUNT || code == CALL_E_PARAMNOTOPTIONAL || code == CALL_E_TYPEMISMATCH;
    }

    // The async bridge's control codes: the call has to be made in another form
    // (sync / async / long sync / gui) or is still in progress. They are not
    // failures to report.
    bool is_bridge_code() const
    {
        return code == CALL_E_NOSYNC || code == CALL_E_NOASYNC || code == CALL_E_LONGSYNC
            || code == CALL_E_GUICALL || code == CALL_E_PENDDING;
    }

    // The argument-validation band (CALL_E_MIN_ARG, CALL_E_MAX): what a variant
    // of one METHOD_OVER set reports when the call's arguments do not fit it.
    // The overload retry loop reads any of these as "this variant did not
    // apply, try the next one".
    bool is_overload_mismatch() const
    {
        return code > CALL_E_MIN_ARG && code < CALL_E_MAX;
    }

    // The internal code band that carries a canned message in s_error_info
    // (utils.cpp renders these with fmtString).
    bool has_error_info() const
    {
        return code > CALL_E_MIN && code < CALL_E_MAX;
    }
};

// P1 keeps the 4-byte layout so the calling convention (and the addon ABI)
// stays identical; P5 (payload carried by the value) raises this deliberately.
static_assert(sizeof(result_t) == sizeof(int32_t), "result_t must stay 4 bytes until P5");

}
