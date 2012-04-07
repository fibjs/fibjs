#include "utils.h"
#include "acPool.h"
#include <string.h>
#include <sstream>
#include <log4cpp/Category.hh>

namespace fibjs
{

v8::Handle<v8::Value> ThrowResult(result_t hr)
{
    static const char* s_errors[] =
    {
        "",
        "Invalid number of parameters.",
        "Parameter not optional.",
        "Constructor cannot be called as a function.",
        "Object is not an instance of declaring class.",
        "The input parameter is not a valid type.",
        "An argument is invalid.",
        "The argument could not be coerced to the specified type.",
        "Value is out of range.",
        "Index was out of range."
    };

    if(hr < 0 && hr > CALL_E_MAX)
        return ThrowError(s_errors[-hr]);

#ifdef _WIN32
    LPWSTR pMsgBuf = NULL;
    if (FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                       NULL, CALL_E_MAX - hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPWSTR) &pMsgBuf, 0, NULL ) && pMsgBuf)
    {
        v8::ThrowException(v8::Exception::Error(v8::String::New((const uint16_t*)pMsgBuf)));
        LocalFree(pMsgBuf);
    }
    else
        ThrowError("Unknown error.");

    return v8::Undefined();
//    return GetLastError();
#else
    return ThrowError(strerror(CALL_E_MAX - hr));
#endif
}

inline const char* ToCString(const v8::String::Utf8Value& value)
{
    return *value ? *value : "<string conversion failed>";
}

void ReportException(v8::TryCatch* try_catch, bool rt)
{
    v8::HandleScope handle_scope;

    v8::String::Utf8Value exception(try_catch->Exception());

    v8::Handle<v8::Message> message = try_catch->Message();
    if (message.IsEmpty())
    	asyncLog(log4cpp::Priority::ERROR, ToCString(exception));
    else
    {
        if(rt)
        {
            v8::String::Utf8Value stack_trace(try_catch->StackTrace());
            if (stack_trace.length() > 0)
            {
            	asyncLog(log4cpp::Priority::ERROR, ToCString(stack_trace));
                return;
            }
        }

        std::stringstream strError;

        v8::String::Utf8Value filename(message->GetScriptResourceName());
        strError << ToCString(exception) << "\n    at ";
        strError << ToCString(filename);
        int lineNumber = message->GetLineNumber();
        strError << ':' << lineNumber << ':' << (message->GetStartColumn() + 1);

        asyncLog(log4cpp::Priority::ERROR, strError.str());
    }
}

std::string traceInfo()
{
	v8::HandleScope handle_scope;

	v8::Handle<v8::StackTrace> stackTrace =
        v8::StackTrace::CurrentStackTrace(10, v8::StackTrace::kOverview);
    int count = stackTrace->GetFrameCount();
    int i;
    std::stringstream strBuffer;

    for(i = 0; i < count; i ++)
    {
        v8::Local<v8::StackFrame> f = stackTrace->GetFrame(i);

        v8::String::Utf8Value funname(f->GetFunctionName());
        v8::String::Utf8Value filename(f->GetScriptName());

        strBuffer << "\n    at ";

        if(**funname)
            strBuffer << *funname << " (";

        strBuffer << *filename << ':' << f->GetLineNumber() << ':' << f->GetColumn();

        if(**funname)
            strBuffer << ')';
    }

    return strBuffer.str();
}

std::string toJSON(v8::Handle<v8::Value> v)
{
	std::string str;
	v8::HandleScope handle_scope;

    v8::Handle<v8::Context> context = v8::Context::GetCurrent();
    v8::Handle<v8::Object> global = context->Global();

    v8::Handle<v8::Object> JSON = global->Get(v8::String::New("JSON"))->ToObject();
    v8::Handle<v8::Function> JSON_stringify = v8::Handle<v8::Function>::Cast(JSON->Get(v8::String::New("stringify")));

    v8::Handle<v8::Value> myargs[] = {v};

    str = *v8::String::Utf8Value(JSON_stringify->Call(JSON, 1, myargs));

	return str;
}

}
