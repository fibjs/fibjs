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
#include <vector>

#include "qstring.h"
#include "obj_ptr.h"

#ifdef _WIN32
#include <windows.h>
#endif

namespace fibjs {

class ErrorPayload;
class Variant;
class object_base;

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
    int32_t m_code = 0;
    obj_ptr<ErrorPayload> m_payload;   // private; const view via payload()

public:
    result_t() = default;
    result_t(int32_t c)
        : m_code(c)
    {
    }

    // Raw code read -- whitelisted call sites only (the raw value channel):
    // the JS boundary (BuildError/FillError), EventInfo, uv/errno interop,
    // ODBC, debug prints and logs; enforced by temp/audit_code_whitelist.py.
    // Everything else decodes via the predicates; there is no implicit
    // conversion to int32_t (P4-final removed it).
    int32_t code() const
    {
        return m_code;
    }

    // The description that rides with the value (read-only; the write path is
    // the builder below -- the value type has no writer, so a moved or copied
    // value can never be modified).
    bool has_payload() const
    {
        return (const ErrorPayload*)m_payload != nullptr;
    }

    const ErrorPayload* payload() const
    {
        return m_payload.get();
    }

    // ----- creation: the builder is the only writable shape -----
    class builder;
    static builder make(int32_t code);
    // The system-error factories: the primary form takes the raw code while the
    // caller still holds it as int32_t (single-shot creation -- no intermediate
    // value is wrapped and reopened); a uv status and an errno are the same
    // domain (the code IS -errno), so one name covers both. The result_t form
    // re-codes a value carried across a layer boundary (the transports).
    static builder from_system(int32_t errno_code);
    static builder from_system(result_t hr);
    static builder inherit(result_t&& base);
    static builder inherit_as(result_t&& base, result_t code);

    // The landing (defined below the builder): converts a builder into this
    // immutable value and fires the interim mirrors. A constructor (rather
    // than a conversion operator) so that direct-initialization forms --
    // e.g. the CHECK_ERROR(<builder>) macro's result_t(hr) -- resolve too.
    result_t(builder&& b);

    // Semantic checks (P2 replaces the hard coded comparisons with these).
    bool failed() const
    {
        return m_code < 0;
    }

    bool ok() const
    {
        return m_code >= 0;
    }

    // A failure that is not <c>: the "propagate anything but this code" idiom
    // (`failed() && !is(c)`), e.g. GetConfigValue's strict mode reports
    // CALL_E_PARAMNOTOPTIONAL for an absent option.
    bool failed_other_than(int32_t c) const
    {
        return m_code < 0 && m_code != c;
    }

    bool is(int32_t c) const
    {
        return m_code == c;
    }

    // A real system error (errno domain): the internal CALL_E_* codes live
    // below CALL_E_MAX, and success (0) must not classify as one either.
    bool is_system_error() const
    {
        return m_code < 0 && m_code > CALL_E_MAX;
    }

    bool is_internal_error() const
    {
        return m_code <= CALL_E_MAX;
    }

    int32_t errno_value() const
    {
        return is_system_error() ? -m_code : 0;
    }

    bool is_pending() const
    {
        return m_code == CALL_E_PENDDING;
    }

    bool is_nosync() const
    {
        return m_code == CALL_E_NOSYNC;
    }

    bool is_exception() const
    {
        return m_code == CALL_E_EXCEPTION;
    }

    bool is_javascript() const
    {
        return m_code == CALL_E_JAVASCRIPT;
    }

    bool is_return_null() const
    {
        return m_code == CALL_RETURN_NULL;
    }

    bool is_return_undefined() const
    {
        return m_code == CALL_RETURN_UNDEFINED;
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
        return m_code == 0;
    }

    // Strict option parsing (`GetConfigValue(..., true)`) reports this when the
    // option was not provided at all.
    bool is_param_missing() const
    {
        return m_code == CALL_E_PARAMNOTOPTIONAL;
    }

    // The "no such file" family: on POSIX both macros are -ENOENT, on Windows
    // they are distinct codes, so the family is their union. A check for one
    // of the two codes must stay `is(CALL_E_...)`: using this predicate there
    // would accept the other code as well on Windows.
    bool is_not_found() const
    {
        return m_code == CALL_E_FILE_NOT_FOUND || m_code == CALL_E_PATH_NOT_FOUND;
    }

    // The value could not be converted to the type the API asked for.
    bool is_type_mismatch() const
    {
        return m_code == CALL_E_TYPEMISMATCH;
    }

    // The input parameter is not a valid type: the inner collection / variant
    // conversions report this and the callers remap it to a type error.
    bool is_bad_var_type() const
    {
        return m_code == CALL_E_BADVARTYPE;
    }

    // A failure about how the arguments were provided: the overload arity, a
    // missing strict option, or a value that does not convert. The argument
    // bridge retries these leniently (see CHECK_ARGUMENT).
    bool is_arg_error() const
    {
        return m_code == CALL_E_BADPARAMCOUNT || m_code == CALL_E_PARAMNOTOPTIONAL || m_code == CALL_E_TYPEMISMATCH;
    }

    // The async bridge's control codes: the call has to be made in another form
    // (sync / async / long sync / gui) or is still in progress. They are not
    // failures to report.
    bool is_bridge_code() const
    {
        return m_code == CALL_E_NOSYNC || m_code == CALL_E_NOASYNC || m_code == CALL_E_LONGSYNC
            || m_code == CALL_E_GUICALL || m_code == CALL_E_PENDDING;
    }

    // The argument-validation band (CALL_E_MIN_ARG, CALL_E_MAX): what a variant
    // of one METHOD_OVER set reports when the call's arguments do not fit it.
    // The overload retry loop reads any of these as "this variant did not
    // apply, try the next one".
    bool is_overload_mismatch() const
    {
        return m_code > CALL_E_MIN_ARG && m_code < CALL_E_MAX;
    }

    // The internal code band that carries a canned message in s_error_info
    // (utils.cpp renders these with fmtString).
    bool has_error_info() const
    {
        return m_code > CALL_E_MIN && m_code < CALL_E_MAX;
    }
};

// The 4-byte P1 layout grew to 16B in P5: code + null payload pointer on the
// success path (two stores, no atomic, no allocation); the description is
// allocated only on failures.
static_assert(sizeof(result_t) == 16, "result_t is { int32_t, obj_ptr<ErrorPayload> }");

// ---------------------------------------------------------------------------
// The description that rides with a failing result_t. Refcounted (obj_base):
// copies share, drops release. Construction and modification belong to
// result_t / its builder (batch 2: the write face is private, the free
// payload APIs are gone).
// ---------------------------------------------------------------------------
struct ErrorArg {
    exlib::string name;
    exlib::string value;
};

// Bounds for arg summaries. Longer names/values are truncated with a trailing
// "..." marker so a single payload cannot grow without limit.
const int32_t kErrorArgNameMaxLength = 64;
const int32_t kErrorArgValueMaxLength = 128;

struct ErrorPayload : public obj_base {
    exlib::string error_type_name;
    exlib::string syscall;
    exlib::string path;
    // Second path of a two-path operation (rename / symlink / link / copyfile):
    // Node reports "<syscall> '<path>' -> '<path2>'".
    exlib::string path2;
    exlib::string hostname;
    exlib::string code;
    exlib::string message;
    // Diagnostic: the C++ function that snapshotted a JS exception into this
    // description (decision 4: the sentinel snapshot carries type / message /
    // code / func). Never rendered on its own, like args a read-only detail.
    exlib::string func;
    // The carried code for the producers whose code belongs to the overload
    // retry band (CALL_E_MIN_ARG, CALL_E_MAX): it must not become the value's
    // code or the METHOD_OVER machinery would read it as "this variant did not
    // apply" and try the next variant. The boundary resolves it (resolveException).
    result_t result_code = 0;
    bool has_errno = false;
    int32_t errno_value = 0;
    std::vector<ErrorArg> args;

    // Creation and every mutator belong to result_t / its builder: a
    // description is built by a builder chain, attached to a value, and never
    // modified afterwards (so copies may share it freely). Outside readers get
    // the const view only (result_t::payload()).
    friend class result_t;
    friend class result_t::builder;

private:
    ErrorPayload() = default;

    obj_ptr<ErrorPayload> with_type_name(const exlib::string& value)
    {
        error_type_name = value;
        return this;
    }

    obj_ptr<ErrorPayload> with_syscall(const char* value)
    {
        syscall = value ? value : "";
        return this;
    }

    obj_ptr<ErrorPayload> with_path(const exlib::string& value)
    {
        path = value;
        return this;
    }

    obj_ptr<ErrorPayload> with_path2(const exlib::string& value)
    {
        path2 = value;
        return this;
    }

    obj_ptr<ErrorPayload> with_hostname(const exlib::string& value)
    {
        hostname = value;
        return this;
    }

    obj_ptr<ErrorPayload> with_code(const exlib::string& value)
    {
        code = value;
        return this;
    }

    obj_ptr<ErrorPayload> with_message(const exlib::string& value)
    {
        message = value;
        return this;
    }
    obj_ptr<ErrorPayload> with_errno(int32_t value)
    {
        has_errno = true;
        errno_value = value;
        return this;
    }

    // printf-style formatted message.
    obj_ptr<ErrorPayload> format(const char* fmt, ...);

    obj_ptr<ErrorPayload> arg(const char* name, const exlib::string& value);
    obj_ptr<ErrorPayload> arg(const char* name, const char* value)
    {
        return arg(name, value ? exlib::string(value) : exlib::string());
    }
    obj_ptr<ErrorPayload> arg(const char* name, int32_t value);
    obj_ptr<ErrorPayload> arg(const char* name, uint32_t value);
    obj_ptr<ErrorPayload> arg(const char* name, int64_t value);
    obj_ptr<ErrorPayload> arg(const char* name, uint64_t value);
    obj_ptr<ErrorPayload> arg(const char* name, bool value);
    obj_ptr<ErrorPayload> arg(const char* name, double value);
    obj_ptr<ErrorPayload> arg(const char* name, const Variant& value);
    // Object arguments render through summarize_for_error: Buffer objects
    // report their length, Stream objects announce the class family.
    obj_ptr<ErrorPayload> arg(const char* name, object_base* value);

    // Sensitive values never enter the payload, not even truncated.
    obj_ptr<ErrorPayload> arg_redacted(const char* name);
    obj_ptr<ErrorPayload> arg_object(const char* name, const char* class_name);
    obj_ptr<ErrorPayload> arg_buffer(const char* name, int64_t length);

public:
    bool has_args() const
    {
        return !args.empty();
    }

    bool empty() const
    {
        return error_type_name.empty() && syscall.empty() && path.empty() && path2.empty() && hostname.empty()
            && code.empty() && message.empty() && result_code.has_value() && !has_errno && args.empty();
    }

private:
    void append_arg(const exlib::string& name, const exlib::string& value);
};

// ---------------------------------------------------------------------------
// builder: the description under construction -- the only writable shape.
// Factories return builders; every with_*/arg mutates in place and returns the
// same builder; the chain lands into a result_t through the conversion below.
// `result_t` itself has no writer, so "write belongs to creation" is enforced
// by the type system (a moved or copied value can never be modified).
// ---------------------------------------------------------------------------
class result_t::builder {
public:
    builder&& with_message(const exlib::string& value) &&
    {
        ensure()->message = value;
        return std::move(*this);
    }

    builder&& with_type_name(const exlib::string& value) &&
    {
        ensure()->error_type_name = value;
        return std::move(*this);
    }

    builder&& with_code(const exlib::string& value) &&
    {
        ensure()->code = value;
        return std::move(*this);
    }

    builder&& with_syscall(const char* value) &&
    {
        ensure()->with_syscall(value);
        return std::move(*this);
    }

    builder&& with_path(const exlib::string& value) &&
    {
        ensure()->path = value;
        return std::move(*this);
    }

    builder&& with_path2(const exlib::string& value) &&
    {
        ensure()->path2 = value;
        return std::move(*this);
    }

    builder&& with_hostname(const exlib::string& value) &&
    {
        ensure()->hostname = value;
        return std::move(*this);
    }

    // The C++ function that snapshotted a JS exception into the description
    // (call sites pass __func__); a diagnostic field, not rendered.
    builder&& with_func(const char* value) &&
    {
        ensure()->func = value;
        return std::move(*this);
    }

    builder&& with_errno(int32_t value) &&
    {
        ErrorPayload* p = ensure();
        p->has_errno = true;
        p->errno_value = value;
        return std::move(*this);
    }

    // A system error: the code of this value IS -errno, so the errno field is
    // the code itself. The call sites that mount a system error rely on the
    // marking without reading the raw code (P4-final gated code()).
    builder&& with_system_errno() &&
    {
        ErrorPayload* p = ensure();
        p->has_errno = true;
        p->errno_value = m_code;
        return std::move(*this);
    }

    // The code rides the payload instead of the value: only for the overload
    // retry band (see ErrorPayload::result_code).
    builder&& with_result(result_t code) &&
    {
        ensure()->result_code = code;
        return std::move(*this);
    }

    // printf-style formatted message; defined in utils.cpp.
    builder&& with_format(const char* fmt, ...);

    builder&& arg(const char* name, const exlib::string& value) &&
    {
        ensure()->arg(name, value);
        return std::move(*this);
    }
    builder&& arg(const char* name, const char* value) &&
    {
        ensure()->arg(name, value);
        return std::move(*this);
    }
    builder&& arg(const char* name, int32_t value) &&
    {
        ensure()->arg(name, value);
        return std::move(*this);
    }
    builder&& arg(const char* name, uint32_t value) &&
    {
        ensure()->arg(name, value);
        return std::move(*this);
    }
    builder&& arg(const char* name, int64_t value) &&
    {
        ensure()->arg(name, value);
        return std::move(*this);
    }
    builder&& arg(const char* name, uint64_t value) &&
    {
        ensure()->arg(name, value);
        return std::move(*this);
    }
    builder&& arg(const char* name, bool value) &&
    {
        ensure()->arg(name, value);
        return std::move(*this);
    }
    builder&& arg(const char* name, double value) &&
    {
        ensure()->arg(name, value);
        return std::move(*this);
    }
    builder&& arg(const char* name, const Variant& value) &&
    {
        ensure()->arg(name, value);
        return std::move(*this);
    }
    builder&& arg(const char* name, object_base* value) &&
    {
        ensure()->arg(name, value);
        return std::move(*this);
    }
    builder&& arg_redacted(const char* name) &&
    {
        ensure()->arg_redacted(name);
        return std::move(*this);
    }
    builder&& arg_object(const char* name, const char* class_name) &&
    {
        ensure()->arg_object(name, class_name);
        return std::move(*this);
    }
    builder&& arg_buffer(const char* name, int64_t length) &&
    {
        ensure()->arg_buffer(name, length);
        return std::move(*this);
    }

private:
    friend class result_t;

    explicit builder(int32_t code)
        : m_code(code)
    {
    }

    ErrorPayload* ensure()
    {
        if (!m_payload)
            m_payload = new ErrorPayload();
        return m_payload;
    }

    int32_t m_code = 0;
    obj_ptr<ErrorPayload> m_payload;
};

inline result_t::builder result_t::make(int32_t code)
{
    return builder(code);
}

inline result_t::builder result_t::from_system(int32_t errno_code)
{
    builder b(errno_code);
    ErrorPayload* p = b.ensure();

    p->has_errno = true;
    p->errno_value = errno_code;
    return b;
}

inline result_t::builder result_t::from_system(result_t hr)
{
    return from_system(hr.code());
}

// Take over a base value's description and keep building on it (the augment
// path: "create a new value on the base of the old one"; zero clone).
inline result_t::builder result_t::inherit(result_t&& base)
{
    builder b(base.m_code);

    b.m_payload = std::move(base.m_payload);
    return b;
}

// Take over a base value's description and land it under a different code: the
// boundary resolving a sentinel to the payload-carried code (resolveException),
// the description kept on the value.
inline result_t::builder result_t::inherit_as(result_t&& base, result_t code)
{
    builder b(code.code());

    b.m_payload = std::move(base.m_payload);
    return b;
}

inline result_t::result_t(builder&& b)
    : m_code(b.m_code)
{
    m_payload = std::move(b.m_payload);
}

}
