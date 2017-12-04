#include "utils.h"
#include "object.h"
#include "ifs/console.h"
#include <string.h>
#include <stdio.h>
#include "utf8.h"

namespace fibjs {

static exlib::string fmtString(result_t hr, const char* str, int32_t len = -1)
{
    exlib::string s;
    if (len < 0)
        len = (int32_t)qstrlen(str);

    s.resize(len + 16);
    s.resize(sprintf(&s[0], "[%d] %s", hr, str));

    return s;
}

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
        "Operation not support synchronous call.",
        // CALL_E_NOASYNC
        "Operation now in progress.",
        // CALL_E_NOSYNC
        "Operation not support asynchronous call.",
        // CALL_E_LONGSYNC
        "Operation is long synchronous call.",
        // CALL_E_GUICALL
        "Operation is GUI call.",
        // CALL_E_INTERNAL
        "Internal error.",
        // CALL_E_RETURN_TYPE
        "Invalid return type.",
        // CALL_E_EXCEPTION
        "Exception occurred.",
        // CALL_E_JAVASCRIPT
        "Javascript error.",
        // CALL_E_PERMIT
        "Permission denied"
    };

    if (hr == CALL_E_EXCEPTION) {
        exlib::string s = Runtime::errMessage();

        if (s.length() > 0)
            return s;
    }

    if (hr > CALL_E_MIN && hr < CALL_E_MAX)
        return fmtString(hr, s_errors[CALL_E_MAX - hr]);

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
    v8::Local<v8::Value> e = v8::Exception::Error(
        isolate->NewString(getResultMessage(hr)));
    e->ToObject()->Set(isolate->NewString("number"), v8::Int32::New(isolate->m_isolate, -hr));

    return isolate->m_isolate->ThrowException(e);
}

exlib::string GetException(TryCatch& try_catch, result_t hr, bool repl)
{
    Isolate* isolate = Isolate::current();
    v8::HandleScope handle_scope(isolate->m_isolate);
    v8::Local<v8::Context> context = isolate->m_isolate->GetCurrentContext();
    if (try_catch.HasCaught()) {
        v8::Local<v8::Value> err = try_catch.Exception();
        v8::Local<v8::Object> err_obj = err.As<v8::Object>();
        v8::String::Utf8Value exception(err_obj);
        v8::Local<v8::Message> message = try_catch.Message();

        if (message.IsEmpty())
            return ToCString(exception);

        exlib::string strError;
        v8::String::Utf8Value filename(message->GetScriptResourceName());
        strError.append(ToCString(filename));
        int32_t lineNumber = message->GetLineNumber();
        if (lineNumber > 0) {
            char numStr[32];
            strError.append(1, ':');
            sprintf(numStr, "%d", lineNumber);
            strError.append(numStr);
            strError.append(1, ':');
            sprintf(numStr, "%d", message->GetStartColumn() + 1);
            strError.append(numStr);
        }
        strError.append("\n");

        v8::Local<v8::String> sourceline;
        if (message->GetSourceLine(context).ToLocal(&sourceline)) {
            // Print line of source code.
            v8::String::Utf8Value sourcelinevalue(isolate->m_isolate, sourceline);
            const char* sourceline_string = ToCString(sourcelinevalue);
            strError.append(sourceline_string);
            strError.append("\n");
            // Print wavy underline (GetUnderline is deprecated).
            int start = message->GetStartColumn(context).FromJust();
            for (int i = 0; i < start; i++) {
                if (sourceline_string[i] == '\0') {
                    break;
                }
                strError.append((sourceline_string[i] == '\t') ? "\t" : " ");
            }
            int end = message->GetEndColumn(context).FromJust();
            for (int i = start; i < end; i++) {
                strError.append("^");
            }
            strError.append("\n");
        }

        if (err_obj->IsNativeError()) {
            v8::Local<v8::Value> stack_trace_string;
            try_catch.StackTrace(context).ToLocal(&stack_trace_string);
            v8::String::Utf8Value stack_trace(isolate->m_isolate,
                stack_trace_string.As<v8::String>());
            strError.append(ToCString(stack_trace));
        } else {
            v8::Local<v8::Value> message;
            v8::Local<v8::Value> name;

            if (repl) {
                strError.append("Thrown: ");
            }

            if (!repl && err->IsObject()) {
                message = err_obj->Get(isolate->NewString("message"));
                name = err_obj->Get(isolate->NewString("name"));
            }

            if (message.IsEmpty() || message->IsUndefined() || name.IsEmpty() || name->IsUndefined()) {
                // Not an error object. Just print as-is.
                v8::String::Utf8Value message(err);
                strError.append(*message ? *message : "<toString() threw exception>");
                strError.append("\n");
            } else {
                v8::String::Utf8Value message_string(isolate->m_isolate, message);
                v8::String::Utf8Value name_string(isolate->m_isolate, name);
                strError.append(ToCString(name_string));
                strError.append(": ");
                strError.append(ToCString(message_string));
            }
        }

        return strError;
    } else if (hr < 0)
        return getResultMessage(hr);

    return "";
}

result_t throwSyntaxError(TryCatch& try_catch)
{
    v8::String::Utf8Value exception(try_catch.Exception());

    v8::Local<v8::Message> message = try_catch.Message();
    if (message.IsEmpty())
        ThrowError(ToCString(exception));
    else {
        return Runtime::setError(GetException(try_catch, 0));
    }

    return CALL_E_JAVASCRIPT;
}

void ReportException(TryCatch& try_catch, result_t hr, bool repl)
{
    if (try_catch.HasCaught() || hr < 0)
        errorLog(GetException(try_catch, hr, repl));
}

result_t CheckConfig(v8::Local<v8::Object> opts, const char** keys)
{
    v8::Local<v8::Array> ks = opts->GetPropertyNames();
    int32_t len = ks->Length();
    int32_t i;

    for (i = 0; i < len; i++) {
        v8::String::Utf8Value k(ks->Get(i));
        const char** p = keys;

        while (p[0]) {
            if (!qstrcmp(*k, p[0]))
                break;
            p++;
        }
        if (!p[0])
            return CHECK_ERROR(Runtime::setError(exlib::string("unknown option \'") + *k + "\'."));
    }

    return 0;
}
}
