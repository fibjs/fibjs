#include "utils.h"
#include "object.h"
#include "ifs/console.h"
#include "ifs/util.h"
#include "ifs/Buffer.h"
#include "ifs/Stream.h"
#include <uv/include/uv.h>
#include <string.h>
#include <stdio.h>
#include "utf8.h"
#include <csignal>
#include "TextColor.h"

#ifdef FIBJS_ERR_PAYLOAD_TRACE
#include <execinfo.h>
#include <atomic>
#endif

namespace fibjs {

static exlib::string fmtString(result_t hr, const char* str, int32_t len = -1)
{
    exlib::string s;
    if (len < 0)
        len = (int32_t)qstrlen(str);

    s.resize(len + 16);
    s.resize(snprintf(s.data(), len + 17, "[%d] %s", hr, str));

    return s;
}

#define UV_STRERROR_GEN(name, msg) \
    case UV_##name:                \
        return msg;
static const char* uv_error(int err)
{
    switch (err) {
        UV_ERRNO_MAP(UV_STRERROR_GEN)
    }
    return NULL;
}
#undef UV_STRERROR_GEN

#define UV_ERR_NAME_GEN(name, _) \
    case UV_##name:              \
        return #name;
static const char* uv_error_name(int err)
{
    switch (err) {
        UV_ERRNO_MAP(UV_ERR_NAME_GEN)
    }
    return NULL;
}
#undef UV_ERR_NAME_GEN

static struct {
    const char* name;
    const char* message;
} s_error_info[] = {
    { errtype::kError, "" }, // placeholder (offset 0)
    { errtype::kTypeError, "Invalid number of parameters." }, // CALL_E_BADPARAMCOUNT
    { errtype::kTypeError, "Parameter not optional." }, // CALL_E_PARAMNOTOPTIONAL
    { errtype::kTypeError, "The input parameter is not a valid type." }, // CALL_E_BADVARTYPE
    { errtype::kTypeError, "Invalid argument." }, // CALL_E_INVALIDARG
    { errtype::kTypeError, "The argument could not be coerced to the specified type." }, // CALL_E_TYPEMISMATCH
    { errtype::kRangeError, "Value is out of range." }, // CALL_E_OUTRANGE
    { errtype::kTypeError, "Constructor cannot be called as a function." }, // CALL_E_CONSTRUCTOR
    { errtype::kTypeError, "Object is not an instance of declaring class." }, // CALL_E_NOTINSTANCE
    { errtype::kError, "Invalid procedure call." }, // CALL_E_INVALID_CALL
    { errtype::kError, "Re-entrant calls are not allowed." }, // CALL_E_REENTRANT
    { errtype::kError, "Invalid input data." }, // CALL_E_INVALID_DATA
    { errtype::kRangeError, "Index was out of range." }, // CALL_E_BADINDEX
    { errtype::kRangeError, "Memory overflow error." }, // CALL_E_OVERFLOW
    { errtype::kError, "Collection is empty." }, // CALL_E_EMPTY
    { errtype::kError, "Operation now in progress." }, // CALL_E_PENDDING
    { errtype::kError, "Operation not support asynchronous call." }, // CALL_E_NOASYNC
    { errtype::kError, "Operation not support synchronous call." }, // CALL_E_NOSYNC
    { errtype::kError, "Operation is long synchronous call." }, // CALL_E_LONGSYNC
    { errtype::kError, "Operation is GUI call." }, // CALL_E_GUICALL
    { errtype::kError, "Internal error." }, // CALL_E_INTERNAL
    { errtype::kError, "The maximum amount of time for a script to execute was exceeded." }, // CALL_E_TIMEOUT
    { errtype::kError, "Operation was aborted." }, // CALL_E_ABORT
    { errtype::kTypeError, "Invalid return type." }, // CALL_E_RETURN_TYPE
    { errtype::kError, "Exception occurred." }, // CALL_E_EXCEPTION
    { errtype::kError, "JavaScript error." }, // CALL_E_JAVASCRIPT
    { errtype::kError, "Permission denied." }, // CALL_E_PERMIT
    { errtype::kError, "Object closed." }, // CALL_E_CLOSED
};

// Default error class name for an internal result code ("" = generic Error).
static const char* getDefaultErrorTypeName(result_t hr)
{
    if (hr > CALL_E_MIN && hr < CALL_E_MAX) {
        int idx = CALL_E_MAX - hr;
        if (idx >= 0 && idx < (int)(sizeof(s_error_info) / sizeof(s_error_info[0])))
            return s_error_info[idx].name;
    }
    return errtype::kError;
}

exlib::string getResultMessage(result_t hr)
{
    if (hr == CALL_E_EXCEPTION) {
        exlib::string s = Runtime::errMessage();

        if (s.length() > 0)
            return s;
    } else if (hr == CALL_E_PARAMNOTOPTIONAL) {
        exlib::string s = Runtime::errMessage();

        if (s.length() > 0)
            return "property " + s + " is not optional.";
    }

    if (hr > CALL_E_MIN && hr < CALL_E_MAX)
        return fmtString(-hr, s_error_info[CALL_E_MAX - hr].message);

    const char* uv_str = uv_error(hr);
    if (uv_str)
        return fmtString(-hr, uv_str);

    hr = -hr;

#ifdef _WIN32
    char16_t MsgBuf[1024];

    if (FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPWSTR)MsgBuf, 1024, NULL)) {
        exlib::string s = fmtString(hr, UTF8_A(MsgBuf));
        size_t sz = s.length();

        if (sz > 0 && s[sz - 1] == '\n')
            s.resize(sz - 1);
        return s;
    }

    return fmtString(hr, "Unknown error.");
#else
    return fmtString(hr, strerror(hr));
#endif
}

// Build a DOMException instance (falls back to an Error with a name property
// when the runtime does not expose the DOMException global).
static v8::Local<v8::Value> MakeNamedGlobalError(Isolate* isolate, const char* name, exlib::string msg)
{
    v8::Local<v8::Context> context = isolate->context();
    v8::Local<v8::Value> ctorVal;

    if (!context->Global()->Get(context, isolate->NewString(name)).ToLocal(&ctorVal)
        || !ctorVal->IsFunction())
        return v8::Local<v8::Value>();

    v8::Local<v8::Value> args[] = { isolate->NewString(msg) };
    v8::Local<v8::Value> error;
    if (ctorVal.As<v8::Function>()->NewInstance(context, 1, args).ToLocal(&error))
        return error;

    return v8::Local<v8::Value>();
}

v8::Local<v8::Value> MakeDOMException(Isolate* isolate, const char* name, exlib::string msg)
{
    v8::Local<v8::Context> context = isolate->context();
    v8::Local<v8::Value> ctor;

    if (context->Global()->Get(context, isolate->NewString("DOMException")).ToLocal(&ctor)
        && ctor->IsFunction()) {
        v8::Local<v8::Value> args[] = { isolate->NewString(msg), isolate->NewString(name) };
        v8::Local<v8::Value> e;
        if (ctor.As<v8::Function>()->NewInstance(context, 2, args).ToLocal(&e))
            return e;
    }

    v8::Local<v8::Value> e = v8::Exception::Error(isolate->NewString(msg));
    e.As<v8::Object>()->Set(context, isolate->NewString("name"), isolate->NewString(name)).IsJust();
    return e;
}

template <typename F>
static v8::Local<v8::Value> MakeNamedErrorOrFallback(Isolate* isolate, const char* name, exlib::string msg, F fallback)
{
    v8::Local<v8::Value> error = MakeNamedGlobalError(isolate, name, msg);
    if (!error.IsEmpty())
        return error;
    return fallback(isolate->NewString(msg));
}

// Build the JS error object for a class name. The name is the only type
// channel: "" means the generic Error, unknown names degrade to it as well.
static v8::Local<v8::Value> MakeException(Isolate* isolate, const exlib::string& type_name, exlib::string msg)
{
    v8::Local<v8::String> v8msg = isolate->NewString(msg);

    if (type_name.empty() || type_name == errtype::kError)
        return v8::Exception::Error(v8msg);

    if (type_name == errtype::kAbortError) {
        exlib::string error_msg = msg.empty() ? "The operation was aborted." : msg;
        v8::Local<v8::Value> error = MakeNamedGlobalError(isolate, "AbortError", error_msg);
        if (!error.IsEmpty())
            return error;
        return MakeDOMException(isolate, "AbortError", error_msg);
    }

    if (type_name == errtype::kTimeoutError) {
        exlib::string error_msg = msg.empty() ? "The operation timed out." : msg;
        v8::Local<v8::Value> error = MakeNamedGlobalError(isolate, "TimeoutError", error_msg);
        if (!error.IsEmpty())
            return error;
        return MakeDOMException(isolate, "TimeoutError", error_msg);
    }

    if (type_name == errtype::kTypeError)
        return MakeNamedErrorOrFallback(isolate, errtype::kTypeError, msg,
            [&](v8::Local<v8::String> v8msg) {
                return v8::Exception::TypeError(v8msg);
            });

    if (type_name == errtype::kRangeError)
        return MakeNamedErrorOrFallback(isolate, errtype::kRangeError, msg,
            [&](v8::Local<v8::String> v8msg) {
                return v8::Exception::RangeError(v8msg);
            });

    if (type_name == errtype::kSyntaxError)
        return MakeNamedErrorOrFallback(isolate, errtype::kSyntaxError, msg,
            [&](v8::Local<v8::String> v8msg) {
                return v8::Exception::SyntaxError(v8msg);
            });

    if (type_name == errtype::kReferenceError)
        return MakeNamedErrorOrFallback(isolate, errtype::kReferenceError, msg,
            [&](v8::Local<v8::String> v8msg) {
                return v8::Exception::ReferenceError(v8msg);
            });

    if (type_name == "SystemError") {
        // Node's SystemError: a plain Error whose name is "SystemError".
        v8::Local<v8::Value> e = v8::Exception::Error(v8msg);

        e.As<v8::Object>()
            ->Set(isolate->context(), isolate->NewString("name"), isolate->NewString("SystemError"))
            .IsJust();
        return e;
    }

    if (type_name == errtype::kURIError || type_name == errtype::kEvalError) {
        v8::Local<v8::Value> error = MakeNamedGlobalError(isolate, type_name.c_str(), msg);
        if (!error.IsEmpty())
            return error;
        return v8::Exception::Error(v8msg);
    }

    return v8::Exception::Error(v8msg);
}

// Optional per-call error context: modules (e.g. fs) set it right before returning an
// error so that BuildError can attach Node.js compatible errno/syscall/path fields
// and parameter summaries. The whole payload is kept - including args, message and
// result code - so no structured information is lost on the way to the JS boundary.
static thread_local ErrorPayload s_err_payload;

void setErrorPayload(const ErrorPayload& data)
{
    s_err_payload = data;
#ifdef FIBJS_ERR_PAYLOAD_TRACE
    s_err_payload.trace_ensure();
#endif
}

ErrorPayload takeErrorPayload()
{
    ErrorPayload data = std::move(s_err_payload);

    s_err_payload = ErrorPayload();
    return data;
}

void clearErrorPayload()
{
    s_err_payload = ErrorPayload();
}

#ifdef FIBJS_ERR_PAYLOAD_TRACE
static std::atomic<uint64_t> s_trace_next_id { 0 };
static std::atomic<uint64_t> s_trace_built { 0 };
static std::atomic<uint64_t> s_trace_lost { 0 };

static void error_payload_trace_summary()
{
    fprintf(stderr, "[errpayload] summary built=%llu lost=%llu delivered=%llu\n",
        (unsigned long long)s_trace_built.load(), (unsigned long long)s_trace_lost.load(),
        (unsigned long long)(s_trace_built.load() - s_trace_lost.load()));
}

uint64_t error_payload_trace_next_id()
{
    static bool s_init = false;

    if (!s_init) {
        s_init = true;

        // fibjs exits through _exit() on the normal path, so the atexit
        // summary is only a best effort; the banner proves the instrument is
        // live and every LOST line carries the running counters.
        fprintf(stderr, "[errpayload] trace active\n");
        atexit(error_payload_trace_summary);
    }

    s_trace_built.fetch_add(1);
    return s_trace_next_id.fetch_add(1) + 1;
}

void error_payload_trace_record(ErrorPayloadTrace& trace)
{
    trace.depth = backtrace(trace.frames, (int)(sizeof(trace.frames) / sizeof(trace.frames[0])));
}

void error_payload_trace_lost(const ErrorPayload& payload)
{
    exlib::string args;

    for (const ErrorArg& arg : payload.args) {
        if (!args.empty())
            args.append(", ");
        args.append(arg.name);
        args.append("=");
        args.append(arg.value);
    }

    uint64_t lost = s_trace_lost.fetch_add(1) + 1;

    fprintf(stderr, "[errpayload] LOST #%llu code=\"%s\" errno=%d syscall=\"%s\" path=\"%s\" hostname=\"%s\" message=\"%s\" args={%s} built=%llu lost=%llu\n",
        (unsigned long long)payload.m_trace->id, payload.code.c_str(),
        payload.has_errno ? payload.errno_value : 0, payload.syscall.c_str(),
        payload.path.c_str(), payload.hostname.c_str(), payload.message.c_str(),
        args.c_str(), (unsigned long long)s_trace_built.load(), (unsigned long long)lost);

    if (payload.m_trace && payload.m_trace->depth > 0) {
        char** symbols = backtrace_symbols(payload.m_trace->frames, payload.m_trace->depth);

        if (symbols) {
            for (int i = 0; i < payload.m_trace->depth; i++)
                fprintf(stderr, "[errpayload]   #%d %s\n", i, symbols[i]);
            free(symbols);
        }
    }
}
#endif

// ---------------------------------------------------------------------------
// ErrorPayload builder: formatted messages and bounded parameter summaries.
// ---------------------------------------------------------------------------

// Buffer objects always render as `<Buffer len=N>`, both when the caller passes
// the object itself and when it only knows the length.
static exlib::string error_buffer_summary(int64_t length)
{
    char buf[64];
    snprintf(buf, sizeof(buf), "<Buffer len=%lld>", (long long)length);
    return exlib::string(buf);
}

static void truncate_error_summary(exlib::string& value, int32_t max_length)
{
    if ((int32_t)value.length() > max_length) {
        value.resize(max_length - 3);
        value.append("...");
    }
}

void ErrorPayload::append_arg(const exlib::string& name, const exlib::string& value)
{
    ErrorArg arg;

    arg.name = name;
    truncate_error_summary(arg.name, kErrorArgNameMaxLength);

    arg.value = value;
    truncate_error_summary(arg.value, kErrorArgValueMaxLength);

    // Modules enhance a payload hop by hop (take - add - store), so the same
    // summary can be offered repeatedly by a retry loop. A repeated pair adds
    // no information and would let one payload grow without bound.
    for (const ErrorArg& existing : args) {
        if (existing.name == arg.name && existing.value == arg.value) {
#ifdef FIBJS_ERR_PAYLOAD_TRACE
            void* frames[8];
            int depth = backtrace(frames, 8);
            char** symbols = backtrace_symbols(frames, depth);

            fprintf(stderr, "[errpayload] DUP arg %s=%s (args now %d)\n", arg.name.c_str(), arg.value.c_str(), (int)args.size());
            if (symbols) {
                for (int i = 0; i < depth; i++)
                    fprintf(stderr, "[errpayload]   #%d %s\n", i, symbols[i]);
                free(symbols);
            }
#endif
            return;
        }
    }

    args.push_back(arg);
}

ErrorPayload& ErrorPayload::format(const char* fmt, ...)
{
    if (fmt) {
        va_list va;
        char buf[1024];

        va_start(va, fmt);
        vsnprintf(buf, sizeof(buf), fmt, va);
        va_end(va);

        message.assign(buf);
    }

    return *this;
}

ErrorPayload& ErrorPayload::arg(const char* name, const exlib::string& value)
{
    append_arg(name ? name : "", value);
    return *this;
}

ErrorPayload& ErrorPayload::arg(const char* name, int32_t value)
{
    char buf[32];
    snprintf(buf, sizeof(buf), "%d", value);
    return arg(name, exlib::string(buf));
}

ErrorPayload& ErrorPayload::arg(const char* name, uint32_t value)
{
    char buf[32];
    snprintf(buf, sizeof(buf), "%u", value);
    return arg(name, exlib::string(buf));
}

ErrorPayload& ErrorPayload::arg(const char* name, int64_t value)
{
    char buf[32];
    snprintf(buf, sizeof(buf), "%lld", (long long)value);
    return arg(name, exlib::string(buf));
}

ErrorPayload& ErrorPayload::arg(const char* name, uint64_t value)
{
    char buf[32];
    snprintf(buf, sizeof(buf), "%llu", (unsigned long long)value);
    return arg(name, exlib::string(buf));
}

ErrorPayload& ErrorPayload::arg(const char* name, bool value)
{
    return arg(name, exlib::string(value ? "true" : "false"));
}

ErrorPayload& ErrorPayload::arg(const char* name, double value)
{
    char buf[64];
    snprintf(buf, sizeof(buf), "%g", value);
    return arg(name, exlib::string(buf));
}

ErrorPayload& ErrorPayload::arg_redacted(const char* name)
{
    append_arg(name ? name : "", "<redacted>");
    return *this;
}

ErrorPayload& ErrorPayload::arg_object(const char* name, const char* class_name)
{
    exlib::string summary = "<Object ";

    summary.append(class_name && *class_name ? class_name : "Object");
    summary.append(">");

    return arg(name, summary);
}

ErrorPayload& ErrorPayload::arg_buffer(const char* name, int64_t length)
{
    return arg(name, error_buffer_summary(length));
}

exlib::string summarize_for_error(object_base* obj)
{
    if (!obj)
        return "<Object>";

    // fibjs-specific families render with their own prefix: Buffer objects
    // report their length, stream objects announce the class family.
    Buffer_base* buffer = Buffer_base::getInstance(obj);
    if (buffer) {
        int32_t length = 0;

        if (buffer->get_length(length) != 0)
            return "<Buffer>";

        return error_buffer_summary(length);
    }

    const char* name = obj->Classinfo().name();
    if (!name || !*name)
        name = "Object";

    bool is_stream = Stream_base::getInstance(obj) != NULL;

    exlib::string summary = is_stream ? "<Stream " : "<Object ";
    summary.append(name);
    summary.append(">");
    return summary;
}

exlib::string summarize_for_error(const Variant& value)
{
    char buf[64];

    switch (value.type()) {
    case Variant::VT_Undefined:
        return "undefined";
    case Variant::VT_Null:
        return "null";
    case Variant::VT_Boolean:
        return value.boolVal() ? "true" : "false";
    case Variant::VT_Integer:
        snprintf(buf, sizeof(buf), "%d", value.intVal());
        return exlib::string(buf);
    case Variant::VT_Long:
        snprintf(buf, sizeof(buf), "%lld", (long long)value.longVal());
        return exlib::string(buf);
    case Variant::VT_Number:
        snprintf(buf, sizeof(buf), "%g", value.dblVal());
        return exlib::string(buf);
    case Variant::VT_String:
        return value.string();
    case Variant::VT_Date:
        return "<Date>";
    case Variant::VT_ArrayBuffer:
        snprintf(buf, sizeof(buf), "<ArrayBuffer len=%d>", (int32_t)value.arrayBufferLength());
        return exlib::string(buf);
    case Variant::VT_Object: {
        object_base* obj = value.object();
        return obj ? summarize_for_error(obj) : exlib::string("<Object>");
    }
    case Variant::VT_JSON:
        return "<JSON>";
    case Variant::VT_MSGPACK:
        return "<MessagePack>";
    default:
        return "<Value>";
    }
}

// Plain description of a result code, without the "[N] " prefix that
// getResultMessage() adds. Node.js embeds this text in its system errors as
// "<CODE>: <text>, <syscall> '<path>'".
static exlib::string error_text(result_t hr)
{
    if (hr > CALL_E_MIN && hr < CALL_E_MAX)
        return s_error_info[CALL_E_MAX - hr].message;

    const char* uv_str = uv_error(hr);
    if (uv_str)
        return uv_str;

    if (hr >= 0)
        return exlib::string();

    return strerror((int)-hr);
}

// Node.js omits the path for the fd based syscalls (there is no path to
// report); fibjs keeps err.path for diagnostics but renders the message the
// same way Node does.
static bool is_fd_syscall(const exlib::string& syscall)
{
    return syscall == "read" || syscall == "write" || syscall == "close"
        || syscall == "lseek" || syscall == "fsync" || syscall == "fdatasync"
        || syscall == "ftruncate";
}

static v8::Local<v8::Value> BuildError(Isolate* isolate, result_t hr, exlib::string type_name, exlib::string msg)
{
    ErrorPayload payload = takeErrorPayload();

    error_payload_delivered(payload);

    // Precedence: payload name (crosses threads) > caller name > hr default.
    if (!payload.error_type_name.empty())
        type_name = payload.error_type_name;
    if (type_name.empty())
        type_name = getDefaultErrorTypeName(hr);

    v8::Local<v8::Value> v = MakeException(isolate, type_name, msg);
    v8::Local<v8::Object> e = v.As<v8::Object>();
    v8::Local<v8::Context> context = isolate->context();

    e->Set(context, isolate->NewString("number"), v8::Int32::New(isolate->m_isolate, -hr)).IsJust();

    const char* _name = uv_error_name(hr);
    if (!_name) {
        int uv_err = uv_translate_sys_error(-hr);
        if (uv_err != UV_UNKNOWN)
            _name = uv_error_name(uv_err);
    }

    exlib::string final_code;
    if (!payload.code.empty())
        final_code = payload.code;
    else if (_name)
        final_code = _name;

    if (!final_code.empty())
        e->Set(context, isolate->NewString("code"), isolate->NewString(final_code)).IsJust();

    // Node.js compatible error fields, filled in by the module that raised the error
    if (!payload.empty()) {
        // Node.js reports errno as a negative value for system errors
        if (payload.has_errno)
            e->Set(context, isolate->NewString("errno"), v8::Int32::New(isolate->m_isolate, payload.errno_value)).IsJust();
        else if (hr < 0 && hr > CALL_E_MAX)
            e->Set(context, isolate->NewString("errno"), v8::Int32::New(isolate->m_isolate, hr)).IsJust();

        if (!payload.syscall.empty())
            e->Set(context, isolate->NewString("syscall"), isolate->NewString(payload.syscall)).IsJust();

        if (!payload.path.empty())
            e->Set(context, isolate->NewString("path"), isolate->NewString(payload.path)).IsJust();

        // Node.js exposes the second path of a two-path operation as `dest`.
        if (!payload.path2.empty())
            e->Set(context, isolate->NewString("dest"), isolate->NewString(payload.path2)).IsJust();

        if (!payload.hostname.empty())
            e->Set(context, isolate->NewString("hostname"), isolate->NewString(payload.hostname)).IsJust();

        // A module that wrote the full text (fs cp / rm style Node errors) wins
        // over the text derived from code + syscall below.
        if (!payload.message.empty())
            msg = payload.message;

        // Node.js renders a system error as "<CODE>: <text>, <syscall> '<path>'"
        // for filesystem calls, "<syscall> <CODE> <address>" for sockets (the
        // address is the host or the unix path), "<CODE>: <text>, <syscall>" when
        // there is no path - which is what a Node program reads from err.message.
        if (!payload.syscall.empty() && !final_code.empty() && payload.message.empty()) {
            if (!payload.hostname.empty())
                msg = payload.syscall + " " + final_code + " " + payload.hostname;
            else if (!payload.path.empty() && payload.syscall == "connect")
                msg = payload.syscall + " " + final_code + " " + payload.path;
            else if (!payload.path.empty() && !is_fd_syscall(payload.syscall)) {
                msg = final_code + ": " + error_text(hr) + ", " + payload.syscall + " '" + payload.path + "'";

                if (!payload.path2.empty())
                    msg += " -> '" + payload.path2 + "'";
            }
            else
                msg = final_code + ": " + error_text(hr) + ", " + payload.syscall;
        }

        // MakeException() set the message it was given; when the payload refined
        // it (system text or a module provided text) the object takes the final
        // one.
        if (!msg.empty())
            e->Set(context, isolate->NewString("message"), isolate->NewString(msg)).IsJust();

        // Node.js renders the stack head from the final error text; this object's
        // stack was captured when it was created, before the payload refined the
        // message (and possibly the name), so refresh that first line.
        {
            v8::Local<v8::Value> namev;
            exlib::string js_name = "Error";

            if (e->Get(context, isolate->NewString("name")).ToLocal(&namev) && namev->IsString()) {
                v8::String::Utf8Value utf8(isolate->m_isolate, namev);
                if (*utf8)
                    js_name = *utf8;
            }

            exlib::string head = js_name;
            if (final_code.length() >= 4 && final_code.substr(0, 4) == "ERR_")
                head += " [" + final_code + "]";
            head += ": " + msg;

            v8::Local<v8::Value> stackv;
            if (e->Get(context, isolate->NewString("stack")).ToLocal(&stackv) && stackv->IsString()) {
                v8::String::Utf8Value utf8(isolate->m_isolate, stackv);
                exlib::string stack = *utf8 ? *utf8 : "";
                size_t nl = stack.find('\n');

                if (stack.substr(0, nl) != head) {
                    if (nl != exlib::string::npos)
                        stack = head + stack.substr(nl);
                    else
                        stack = head;

                    e->Set(context, isolate->NewString("stack"), isolate->NewString(stack)).IsJust();
                }
            }
        }

        if (!payload.args.empty()) {
            v8::Local<v8::Object> args = v8::Object::New(isolate->m_isolate);

            for (const ErrorArg& arg : payload.args)
                args->Set(context, isolate->NewString(arg.name), isolate->NewString(arg.value)).IsJust();

            // Diagnostic view only, keep it read-only so user code cannot turn
            // a structured error into a shared mutable side channel.
            args->SetIntegrityLevel(context, v8::IntegrityLevel::kFrozen).IsJust();

            e->Set(context, isolate->NewString("args"), args).IsJust();
        }
    }

    return e;
}

static void resolveException(result_t& hr, exlib::string& type_name, exlib::string& msg)
{
    if (hr == CALL_E_EXCEPTION) {
        hr = Runtime::errCode();
        type_name = Runtime::errTypeName();
        msg = Runtime::errMessage();
    }
}
v8::Local<v8::Value> FillError(result_t hr, exlib::string msg)
{
    exlib::string type_name;
    exlib::string rt_msg;
    resolveException(hr, type_name, rt_msg);
    return BuildError(Isolate::current(), hr, type_name, msg);
}

v8::Local<v8::Value> FillError(result_t hr)
{
    exlib::string type_name;
    exlib::string msg;
    resolveException(hr, type_name, msg);
    if (msg.empty())
        msg = getResultMessage(hr);
    return BuildError(Isolate::current(), hr, type_name, msg);
}

v8::Local<v8::Value> FillError(result_t hr, v8::Local<v8::StackTrace> stack)
{
    exlib::string type_name;
    exlib::string msg;
    resolveException(hr, type_name, msg);
    if (msg.empty())
        msg = getResultMessage(hr);

    Isolate* isolate = Isolate::current();
    v8::Local<v8::Value> v = BuildError(isolate, hr, type_name, msg);

    // Set custom stack trace if provided
    if (!stack.IsEmpty()) {
        v8::Local<v8::Object> e = v.As<v8::Object>();
        v8::Local<v8::Context> context = isolate->context();

        // Build stack trace string from frames
        exlib::string stack_str = msg + "\n";
        int frame_count = stack->GetFrameCount();

        for (int i = 0; i < frame_count; i++) {
            v8::Local<v8::StackFrame> frame = stack->GetFrame(isolate->m_isolate, i);

            v8::String::Utf8Value script_name(isolate->m_isolate, frame->GetScriptName());
            v8::String::Utf8Value function_name(isolate->m_isolate, frame->GetFunctionName());

            stack_str += "    at ";
            if (*function_name && strlen(*function_name) > 0) {
                stack_str += *function_name;
                stack_str += " (";
            }
            if (*script_name) {
                stack_str += *script_name;
            }
            stack_str += ":";
            stack_str += std::to_string(frame->GetLineNumber());
            stack_str += ":";
            stack_str += std::to_string(frame->GetColumn());
            if (*function_name && strlen(*function_name) > 0) {
                stack_str += ")";
            }
            stack_str += "\n";
        }

        e->Set(context, isolate->NewString("stack"), isolate->NewString(stack_str)).IsJust();
    }

    return v;
}

v8::Local<v8::Value> ThrowResult(result_t hr)
{
    Isolate* isolate = Isolate::current();
    return isolate->m_isolate->ThrowException(FillError(hr));
}

exlib::string GetException(v8::Local<v8::Value> err, bool repl, bool trace)
{
    if (err.IsEmpty() || !err->IsObject())
        return "Unknown error.";

    Isolate* isolate = Isolate::current();
    v8::HandleScope handle_scope(isolate->m_isolate);
    v8::Local<v8::Context> context = isolate->context();

    v8::Local<v8::Message> message = v8::Exception::CreateMessage(isolate->m_isolate, err);
    // try_catch.Message();

    if (message.IsEmpty())
        return isolate->toString(err);

    exlib::string strError;
    v8::Local<v8::Value> res = message->GetScriptResourceName();
    if (!res->IsUndefined()) {
        strError.append(isolate->toString(res));
        int32_t lineNumber = message->GetLineNumber(context).FromMaybe(0);
        if (lineNumber > 0) {
            char numStr[32];
            strError.append(1, ':');
            snprintf(numStr, sizeof(numStr), "%d", lineNumber);
            strError.append(numStr);
            strError.append(1, ':');
            snprintf(numStr, sizeof(numStr), "%d", message->GetStartColumn() + 1);
            strError.append(numStr);
        }
        strError.append("\n");

        v8::Local<v8::String> sourceline = message->GetSourceLine(context).FromMaybe(v8::Local<v8::String>());
        if (!sourceline.IsEmpty()) {
            // Print line of source code.
            v8::String::Utf8Value sourcelinevalue(isolate->m_isolate, sourceline);
            const char* sourceline_string = ToCString(sourcelinevalue);
            strError.append(sourceline_string);
            strError.append("\n");
            // Print wavy underline (GetUnderline is deprecated).
            int start = message->GetStartColumn(context).FromMaybe(0);
            for (int i = 0; i < start; i++) {
                if (sourceline_string[i] == '\0') {
                    break;
                }
                strError.append((sourceline_string[i] == '\t') ? "\t" : " ");
            }
            int end = message->GetEndColumn(context).FromMaybe(start);
            for (int i = start; i < end; i++) {
                strError.append("^");
            }
            strError.append("\n");
        }
    }

    if (err->IsObject()) {
        v8::Local<v8::Object> err_obj = err.As<v8::Object>();

        if (trace && err_obj->IsNativeError()) {
            v8::Local<v8::Value> stack_trace_string = v8::TryCatch::StackTrace(context, err).FromMaybe(v8::Local<v8::Value>());
            if (!stack_trace_string.IsEmpty())
                strError.append(isolate->toString(stack_trace_string));
        } else {
            JSValue message;
            JSValue name;

            if (repl) {
                strError.append("Thrown: ");
            }

            if (!repl) {
                message = err_obj->Get(context, isolate->NewString("message"));
                name = err_obj->Get(context, isolate->NewString("name"));
            }

            if (message.IsEmpty() || message->IsUndefined() || name.IsEmpty() || name->IsUndefined()) {
                // Not an error object. Just print as-is.
                strError.append(isolate->toString(err));
                strError.append("\n");
            } else {
                strError.append(isolate->toString(name));
                strError.append(": ");
                strError.append(isolate->toString(message));
            }
        }

        strError.append(" " + COLOR_RESET);

        v8::Local<v8::Array> keys;
        if (err_obj->GetPropertyNames(context).ToLocal(&keys) && !keys.IsEmpty()) {
            int32_t len = keys->Length();
            v8::Local<v8::Object> o = v8::Object::New(isolate->m_isolate);

            for (int32_t i = 0; i < len; i++) {
                v8::Local<v8::Value> key;
                if (!keys->Get(context, i).ToLocal(&key))
                    continue;

                v8::Local<v8::Value> val;
                if (!err_obj->Get(context, key).ToLocal(&val))
                    continue;

                o->Set(context, key, val).IsJust();
            }

            exlib::string str;
            util_base::inspect(o, v8::Local<v8::Object>(), str);
            strError.append(str);
        }
    }

    return strError;
}

exlib::string GetException(TryCatch& try_catch, result_t hr, bool repl, bool trace)
{
    if (hr < 0 && hr != CALL_E_JAVASCRIPT)
        return getResultMessage(hr);

    Isolate* isolate = Isolate::current();
    v8::HandleScope handle_scope(isolate->m_isolate);
    if (try_catch.HasCaught())
        return GetException(try_catch.Exception(), repl, trace);

    return "Unknown error.";
}

result_t throwSyntaxError(TryCatch& try_catch)
{
    Isolate* isolate = Isolate::current();

    v8::Local<v8::Message> message = try_catch.Message();
    if (message.IsEmpty())
        ThrowError(isolate->toString(try_catch.Exception()));
    else {
        return Runtime::setError(GetException(try_catch, 0, false, true));
    }

    return CALL_E_JAVASCRIPT;
}

exlib::string ReportException(TryCatch& try_catch, result_t hr, bool repl)
{
    exlib::string msg;

    if (try_catch.HasCaught() || hr < 0) {
        msg = GetException(try_catch, hr, repl, true);
        errorLog(msg);
    }

    return msg;
}

result_t CheckConfig(v8::Local<v8::Object> opts, const char** keys)
{
    Isolate* isolate = Isolate::current(opts);
    v8::Local<v8::Context> context = isolate->context();
    JSArray ks = opts->GetPropertyNames(context);
    int32_t len = ks->Length();
    int32_t i;

    for (i = 0; i < len; i++) {
        v8::String::Utf8Value k(isolate->m_isolate, JSValue(ks->Get(context, i)));
        const char** p = keys;

        while (p[0]) {
            if (!qstrcmp(ToCString(k), p[0]))
                break;
            p++;
        }
        if (!p[0])
            return CHECK_ERROR(Runtime::setError(exlib::string("unknown option \'") + *k + "\'."));
    }

    return 0;
}

const char* signo_string(int signo)
{
#define SIGNO_CASE(e) \
    case e:           \
        return #e;

    switch (signo) {
#ifdef SIGHUP
        SIGNO_CASE(SIGHUP);
#endif

#ifdef SIGINT
        SIGNO_CASE(SIGINT);
#endif

#ifdef SIGQUIT
        SIGNO_CASE(SIGQUIT);
#endif

#ifdef SIGILL
        SIGNO_CASE(SIGILL);
#endif

#ifdef SIGTRAP
        SIGNO_CASE(SIGTRAP);
#endif

#ifdef SIGABRT
        SIGNO_CASE(SIGABRT);
#endif

#ifdef SIGIOT
#if SIGABRT != SIGIOT
        SIGNO_CASE(SIGIOT);
#endif
#endif

#ifdef SIGBUS
        SIGNO_CASE(SIGBUS);
#endif

#ifdef SIGFPE
        SIGNO_CASE(SIGFPE);
#endif

#ifdef SIGKILL
        SIGNO_CASE(SIGKILL);
#endif

#ifdef SIGUSR1
        SIGNO_CASE(SIGUSR1);
#endif

#ifdef SIGSEGV
        SIGNO_CASE(SIGSEGV);
#endif

#ifdef SIGUSR2
        SIGNO_CASE(SIGUSR2);
#endif

#ifdef SIGPIPE
        SIGNO_CASE(SIGPIPE);
#endif

#ifdef SIGALRM
        SIGNO_CASE(SIGALRM);
#endif

        SIGNO_CASE(SIGTERM);

#ifdef SIGCHLD
        SIGNO_CASE(SIGCHLD);
#endif

#ifdef SIGSTKFLT
        SIGNO_CASE(SIGSTKFLT);
#endif

#ifdef SIGCONT
        SIGNO_CASE(SIGCONT);
#endif

#ifdef SIGSTOP
        SIGNO_CASE(SIGSTOP);
#endif

#ifdef SIGTSTP
        SIGNO_CASE(SIGTSTP);
#endif

#ifdef SIGBREAK
        SIGNO_CASE(SIGBREAK);
#endif

#ifdef SIGTTIN
        SIGNO_CASE(SIGTTIN);
#endif

#ifdef SIGTTOU
        SIGNO_CASE(SIGTTOU);
#endif

#ifdef SIGURG
        SIGNO_CASE(SIGURG);
#endif

#ifdef SIGXCPU
        SIGNO_CASE(SIGXCPU);
#endif

#ifdef SIGXFSZ
        SIGNO_CASE(SIGXFSZ);
#endif

#ifdef SIGVTALRM
        SIGNO_CASE(SIGVTALRM);
#endif

#ifdef SIGPROF
        SIGNO_CASE(SIGPROF);
#endif

#ifdef SIGWINCH
        SIGNO_CASE(SIGWINCH);
#endif

#ifdef SIGIO
        SIGNO_CASE(SIGIO);
#endif

#ifdef SIGPOLL
#if SIGPOLL != SIGIO
        SIGNO_CASE(SIGPOLL);
#endif
#endif

#ifdef SIGLOST
#if SIGLOST != SIGABRT
        SIGNO_CASE(SIGLOST);
#endif
#endif

#ifdef SIGPWR
#if SIGPWR != SIGLOST
        SIGNO_CASE(SIGPWR);
#endif
#endif

#ifdef SIGINFO
#if !defined(SIGPWR) || SIGINFO != SIGPWR
        SIGNO_CASE(SIGINFO);
#endif
#endif

#ifdef SIGSYS
        SIGNO_CASE(SIGSYS);
#endif

    default:
        return "";
    }
}
}
