#include "utils.h"
#include "object.h"
#include <string.h>
#include <stdio.h>
#include <sstream>
#include <log4cpp/Category.hh>
#include "utf8.h"
#include <zmq/zmq.h>

namespace fibjs
{

static std::string fmtString(result_t hr, const char *str, int len = -1)
{
    std::string s;
    if (len < 0)
        len = (int) qstrlen(str);

    s.resize(len + 16);
    s.resize(sprintf(&s[0], "[%d] %s", hr, str));

    return s;
}

static std::string fmtString(result_t hr, std::string &str)
{
    return fmtString(hr, str.c_str(), (int) str.length());
}

std::string getResultMessage(result_t hr)
{
    static const char *s_errors[] =
    {
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
        // CALL_E_NOSYNC
        "Operation not support synchronous call.",
        // CALL_E_NOASYNC
        "Operation not support asynchronous call.",
        // CALL_E_INTERNAL
        "Internal error.",
        // CALL_E_RETURN_TYPE
        "Invalid return type.",
        // CALL_E_EXCEPTION
        "Exception occurred."
    };

    if (hr == CALL_E_EXCEPTION)
    {
        std::string s = Runtime::errMessage();

        if (s.length() > 0)
            return fmtString(-hr, s);
    }

    if (hr > CALL_E_MIN && hr < CALL_E_MAX)
        return fmtString(hr, s_errors[CALL_E_MAX - hr]);

    hr = -hr;

    if (hr > ZMQ_HAUSNUMERO)
        return fmtString(hr, zmq_strerror(hr));

#ifdef _WIN32
    WCHAR MsgBuf[1024];

    if (FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                       NULL, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), MsgBuf, 1024, NULL ))
    {
        std::string s = fmtString(hr, UTF8_A(MsgBuf));
        return s;
    }

    return fmtString(hr, "Unknown error.");
#else
    return fmtString(hr, strerror(hr));
#endif
}

v8::Local<v8::Value> ThrowResult(result_t hr)
{
    v8::Local<v8::Value> e = v8::Exception::Error(
                                  v8::String::NewFromUtf8(isolate, getResultMessage(hr).c_str()));
    e->ToObject()->Set(v8::String::NewFromUtf8(isolate, "number"), v8::Int32::New(isolate, -hr));

    return isolate->ThrowException(e);
}

inline const char *ToCString(const v8::String::Utf8Value &value)
{
    return *value ? *value : "<string conversion failed>";
}

std::string GetException(v8::TryCatch &try_catch, result_t hr)
{
    if (try_catch.HasCaught())
    {
        v8::String::Utf8Value exception(try_catch.Exception());

        v8::Local<v8::Message> message = try_catch.Message();
        if (message.IsEmpty())
            return ToCString(exception);
        else
        {
            v8::Local<v8::Value> trace_value = try_catch.StackTrace();

            if (!IsEmpty(trace_value))
            {
                v8::String::Utf8Value stack_trace(trace_value);
                return ToCString(stack_trace);
            }

            std::stringstream strError;

            v8::String::Utf8Value filename(message->GetScriptResourceName());
            strError << ToCString(exception) << "\n    at ";
            strError << ToCString(filename);
            int lineNumber = message->GetLineNumber();
            strError << ':' << lineNumber << ':'
                     << (message->GetStartColumn() + 1);

            return strError.str();
        }
    }
    else if (hr < 0)
        return getResultMessage(hr);

    return "";
}

void ReportException(v8::TryCatch &try_catch, result_t hr)
{
    if (try_catch.HasCaught() ||  hr < 0)
        asyncLog(log4cpp::Priority::ERROR, GetException(try_catch, hr));
}

std::string traceInfo()
{
    v8::Local<v8::StackTrace> stackTrace = v8::StackTrace::CurrentStackTrace(
            isolate, 10, v8::StackTrace::kOverview);
    int count = stackTrace->GetFrameCount();
    int i;
    std::stringstream strBuffer;

    for (i = 0; i < count; i++)
    {
        v8::Local<v8::StackFrame> f = stackTrace->GetFrame(i);

        v8::String::Utf8Value funname(f->GetFunctionName());
        v8::String::Utf8Value filename(f->GetScriptName());

        strBuffer << "\n    at ";

        if (**funname)
            strBuffer << *funname << " (";

        strBuffer << *filename << ':' << f->GetLineNumber() << ':'
                  << f->GetColumn();

        if (**funname)
            strBuffer << ')';
    }

    return strBuffer.str();
}

}
