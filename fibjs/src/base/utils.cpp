#include "utils.h"
#include "object.h"
#include "ifs/console.h"
#include <uv/include/uv.h>
#include <string.h>
#include <stdio.h>
#include "utf8.h"
#include <csignal>

namespace fibjs {

static exlib::string fmtString(result_t hr, const char* str, int32_t len = -1)
{
    exlib::string s;
    if (len < 0)
        len = (int32_t)qstrlen(str);

    s.resize(len + 16);
    s.resize(snprintf(s.c_buffer(), len + 17, "[%d] %s", hr, str));

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

exlib::string getResultMessage(result_t hr)
{
    static const char* s_errors[] = {
        "",
        // CALL_E_BADPARAMCOUNT
        "Invalid number of parameters.",
        // CALL_E_PARAMNOTOPTIONAL
        "Parameter not optional.",
        // CALL_E_BADVARTYPE
        "The input parameter is not a valid type.",
        // CALL_E_INVALIDARG
        "Invalid argument.",
        // CALL_E_TYPEMISMATCH
        "The argument could not be coerced to the specified type.",
        // CALL_E_OUTRANGE
        "Value is out of range.",

        // CALL_E_CONSTRUCTOR
        "Constructor cannot be called as a function.",
        // CALL_E_NOTINSTANCE
        "Object is not an instance of declaring class.",
        // CALL_E_INVALID_CALL
        "Invalid procedure call.",
        // CALL_E_REENTRANT_CALL
        "Re-entrant calls are not allowed.",
        // CALL_E_INVALID_DATA
        "Invalid input data.",
        // CALL_E_BADINDEX
        "Index was out of range.",
        // CALL_E_OVERFLOW
        "Memory overflow error.",
        // CALL_E_EMPTY
        "Collection is empty.",
        // CALL_E_PENDDING
        "Operation now in progress.",
        // CALL_E_NOASYNC
        "Operation not support asynchronous call.",
        // CALL_E_NOSYNC
        "Operation not support synchronous call.",
        // CALL_E_LONGSYNC
        "Operation is long synchronous call.",
        // CALL_E_GUICALL
        "Operation is GUI call.",
        // CALL_E_INTERNAL
        "Internal error.",
        // CALL_E_TIMEOUT
        "The maximum amount of time for a script to execute was exceeded.",
        // CALL_E_RETURN_TYPE
        "Invalid return type.",
        // CALL_E_EXCEPTION
        "Exception occurred.",
        // CALL_E_JAVASCRIPT
        "JavaScript error.",
        // CALL_E_PERMIT
        "Permission denied.",
        // CALL_E_CLOSED
        "Object closed."
    };

    if (hr == CALL_E_EXCEPTION) {
        exlib::string s = Runtime::errMessage();

        if (s.length() > 0)
            return s;
    }

    if (hr > CALL_E_MIN && hr < CALL_E_MAX)
        return fmtString(-hr, s_errors[CALL_E_MAX - hr]);

    const char* uv_str = uv_error(hr);
    if (uv_str)
        return fmtString(-hr, uv_str);

    hr = -hr;

#ifdef _WIN32
    exlib::wchar MsgBuf[1024];

    if (FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), MsgBuf, 1024, NULL)) {
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

v8::Local<v8::Value> ThrowResult(result_t hr)
{
    Isolate* isolate = Isolate::current();
    v8::Local<v8::Value> v = v8::Exception::Error(
        isolate->NewString(getResultMessage(hr)));
    v8::Local<v8::Object> e = v8::Local<v8::Object>::Cast(v);
    v8::Local<v8::Context> context = isolate->context();

    e->Set(context, isolate->NewString("number"), v8::Int32::New(isolate->m_isolate, -hr)).Check();

    const char* _name = uv_error_name(hr);
    if (_name)
        e->Set(context, isolate->NewString("code"), isolate->NewString(_name)).Check();

    return isolate->m_isolate->ThrowException(e);
}

exlib::string GetException(TryCatch& try_catch, result_t hr, bool repl)
{
    if (hr < 0 && hr != CALL_E_JAVASCRIPT)
        return getResultMessage(hr);

    Isolate* isolate = Isolate::current();
    v8::HandleScope handle_scope(isolate->m_isolate);
    v8::Local<v8::Context> context = isolate->context();
    if (try_catch.HasCaught()) {
        v8::Local<v8::Value> err = try_catch.Exception();
        v8::Local<v8::Object> err_obj = err.As<v8::Object>();
        v8::Local<v8::Message> message = try_catch.Message();

        if (message.IsEmpty())
            return isolate->toString(err_obj);

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

        if (err_obj->IsNativeError()) {
            v8::Local<v8::Value> stack_trace_string = try_catch.StackTrace(context).FromMaybe(v8::Local<v8::Value>());
            if (!stack_trace_string.IsEmpty())
                strError.append(isolate->toString(stack_trace_string));
        } else {
            JSValue message;
            JSValue name;

            if (repl) {
                strError.append("Thrown: ");
            }

            if (!repl && err->IsObject()) {
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

        return strError;
    }

    return "";
}

result_t throwSyntaxError(TryCatch& try_catch)
{
    Isolate* isolate = Isolate::current();

    v8::Local<v8::Message> message = try_catch.Message();
    if (message.IsEmpty())
        ThrowError(isolate->toString(try_catch.Exception()));
    else {
        return Runtime::setError(GetException(try_catch, 0));
    }

    return CALL_E_JAVASCRIPT;
}

exlib::string ReportException(TryCatch& try_catch, result_t hr, bool repl)
{
    exlib::string msg;

    if (try_catch.HasCaught() || hr < 0) {
        msg = GetException(try_catch, hr, repl);
        errorLog(msg);
    }

    return msg;
}

result_t CheckConfig(v8::Local<v8::Object> opts, const char** keys)
{
    v8::Local<v8::Context> context = opts->GetCreationContextChecked();
    JSArray ks = opts->GetPropertyNames(context);
    int32_t len = ks->Length();
    int32_t i;
    Isolate* isolate = Isolate::current();

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
