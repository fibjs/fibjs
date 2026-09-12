#include "utils.h"
#include "object.h"
#include "ifs/console.h"
#include "ifs/util.h"
#include <uv/include/uv.h>
#include <string.h>
#include <stdio.h>
#include "utf8.h"
#include <csignal>
#include "TextColor.h"

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
    ErrorType type;
    const char* message;
} s_error_info[] = {
    { kError, "" }, // placeholder (offset 0)
    { kTypeError, "Invalid number of parameters." }, // CALL_E_BADPARAMCOUNT
    { kTypeError, "Parameter not optional." }, // CALL_E_PARAMNOTOPTIONAL
    { kTypeError, "The input parameter is not a valid type." }, // CALL_E_BADVARTYPE
    { kTypeError, "Invalid argument." }, // CALL_E_INVALIDARG
    { kTypeError, "The argument could not be coerced to the specified type." }, // CALL_E_TYPEMISMATCH
    { kRangeError, "Value is out of range." }, // CALL_E_OUTRANGE
    { kTypeError, "Constructor cannot be called as a function." }, // CALL_E_CONSTRUCTOR
    { kTypeError, "Object is not an instance of declaring class." }, // CALL_E_NOTINSTANCE
    { kError, "Invalid procedure call." }, // CALL_E_INVALID_CALL
    { kError, "Re-entrant calls are not allowed." }, // CALL_E_REENTRANT
    { kError, "Invalid input data." }, // CALL_E_INVALID_DATA
    { kRangeError, "Index was out of range." }, // CALL_E_BADINDEX
    { kRangeError, "Memory overflow error." }, // CALL_E_OVERFLOW
    { kError, "Collection is empty." }, // CALL_E_EMPTY
    { kError, "Operation now in progress." }, // CALL_E_PENDDING
    { kError, "Operation not support asynchronous call." }, // CALL_E_NOASYNC
    { kError, "Operation not support synchronous call." }, // CALL_E_NOSYNC
    { kError, "Operation is long synchronous call." }, // CALL_E_LONGSYNC
    { kError, "Operation is GUI call." }, // CALL_E_GUICALL
    { kError, "Internal error." }, // CALL_E_INTERNAL
    { kError, "The maximum amount of time for a script to execute was exceeded." }, // CALL_E_TIMEOUT
    { kError, "Operation was aborted." }, // CALL_E_ABORT
    { kTypeError, "Invalid return type." }, // CALL_E_RETURN_TYPE
    { kError, "Exception occurred." }, // CALL_E_EXCEPTION
    { kError, "JavaScript error." }, // CALL_E_JAVASCRIPT
    { kError, "Permission denied." }, // CALL_E_PERMIT
    { kError, "Object closed." }, // CALL_E_CLOSED
};

static ErrorType getDefaultErrorType(result_t hr)
{
    if (hr > CALL_E_MIN && hr < CALL_E_MAX) {
        int idx = CALL_E_MAX - hr;
        if (idx >= 0 && idx < (int)(sizeof(s_error_info) / sizeof(s_error_info[0])))
            return s_error_info[idx].type;
    }
    return kError;
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
static v8::Local<v8::Value> MakeDOMException(Isolate* isolate, const char* name, exlib::string msg)
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

static v8::Local<v8::Value> MakeException(Isolate* isolate, ErrorType et, exlib::string msg)
{
    v8::Local<v8::String> v8msg = isolate->NewString(msg);
    switch (et) {
    case kAbortError:
        return MakeDOMException(isolate, "AbortError",
            msg.empty() ? "The operation was aborted." : msg);
    case kTimeoutError:
        return MakeDOMException(isolate, "TimeoutError",
            msg.empty() ? "The operation timed out." : msg);
    case kTypeError:
        return v8::Exception::TypeError(v8msg);
    case kRangeError:
        return v8::Exception::RangeError(v8msg);
    case kSyntaxError:
        return v8::Exception::SyntaxError(v8msg);
    case kReferenceError:
        return v8::Exception::ReferenceError(v8msg);
    case kURIError: {
        auto ctx = isolate->context();
        auto glob = ctx->Global();
        auto ctor = JSValue(glob->Get(ctx, isolate->NewString("URIError"))).As<v8::Object>();
        v8::Local<v8::Value> args[] = { v8msg };
        return ctor->CallAsConstructor(ctx, 1, args).FromMaybe(v8::Local<v8::Value>());
    }
    case kEvalError: {
        auto ctx = isolate->context();
        auto glob = ctx->Global();
        auto ctor = JSValue(glob->Get(ctx, isolate->NewString("EvalError"))).As<v8::Object>();
        v8::Local<v8::Value> args[] = { v8msg };
        return ctor->CallAsConstructor(ctx, 1, args).FromMaybe(v8::Local<v8::Value>());
    }
    default:
        return v8::Exception::Error(v8msg);
    }
}

static v8::Local<v8::Value> BuildError(Isolate* isolate, result_t hr, ErrorType et, exlib::string msg)
{
    v8::Local<v8::Value> v = MakeException(isolate, et, msg);
    v8::Local<v8::Object> e = v.As<v8::Object>();
    v8::Local<v8::Context> context = isolate->context();

    e->Set(context, isolate->NewString("number"), v8::Int32::New(isolate->m_isolate, -hr)).IsJust();

    const char* _name = uv_error_name(hr);
    if (!_name) {
        int uv_err = uv_translate_sys_error(-hr);
        if (uv_err != UV_UNKNOWN)
            _name = uv_error_name(uv_err);
    }

    if (_name)
        e->Set(context, isolate->NewString("code"), isolate->NewString(_name)).IsJust();

    return e;
}

static void resolveException(result_t& hr, ErrorType& et, exlib::string& msg)
{
    if (hr == CALL_E_EXCEPTION) {
        hr = Runtime::errCode();
        et = Runtime::errType();
        msg = Runtime::errMessage();
    }
    if (et == kError)
        et = getDefaultErrorType(hr);
}

v8::Local<v8::Value> FillError(result_t hr, exlib::string msg)
{
    ErrorType et = kError;
    exlib::string rt_msg;
    resolveException(hr, et, rt_msg);
    return BuildError(Isolate::current(), hr, et, msg);
}

v8::Local<v8::Value> FillError(result_t hr)
{
    ErrorType et = kError;
    exlib::string msg;
    resolveException(hr, et, msg);
    if (msg.empty())
        msg = getResultMessage(hr);
    return BuildError(Isolate::current(), hr, et, msg);
}

v8::Local<v8::Value> FillError(result_t hr, v8::Local<v8::StackTrace> stack)
{
    ErrorType et = kError;
    exlib::string msg;
    resolveException(hr, et, msg);
    if (msg.empty())
        msg = getResultMessage(hr);

    Isolate* isolate = Isolate::current();
    v8::Local<v8::Value> v = BuildError(isolate, hr, et, msg);

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

        v8::Local<v8::Array> keys = err_obj->GetPropertyNames(context).ToLocalChecked();
        int32_t len = keys->Length();
        v8::Local<v8::Object> o = v8::Object::New(isolate->m_isolate);

        for (int32_t i = 0; i < len; i++) {
            v8::Local<v8::Value> key = keys->Get(context, i).ToLocalChecked();
            v8::Local<v8::Value> val = err_obj->Get(context, key).ToLocalChecked();

            o->Set(context, key, val).IsJust();
        }

        exlib::string str;
        util_base::inspect(o, v8::Local<v8::Object>(), str);
        strError.append(str);
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
