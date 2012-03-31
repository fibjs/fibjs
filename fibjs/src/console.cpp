#include "ifs/console.h"
#include <log4cpp/Category.hh>
#include <sstream>
#include <map>

#ifdef _WIN32
#include <mmsystem.h>
int64_t Ticks()
{
    return timeGetTime();  // Convert to microseconds.
}

#else
#include <sys/time.h>

int64_t Ticks()
{
    struct timeval tv;
    if (gettimeofday(&tv, NULL) < 0)
        return 0;
    return (tv.tv_sec * 1000ll) + tv.tv_usec / 1000;
}

#endif

namespace fibjs
{

std::string Format(const char* fmt, const v8::Arguments& args, int idx = 1)
{
    const char* s = fmt;
    const char* s1;
    char ch;
    int argc = args.Length();
    std::stringstream strBuffer;

    while(1)
    {
        if(idx >= argc)
        {
            strBuffer << s;
            break;
        }

        s1 = s;
        while((ch = *s++) && (ch != '%'));

        strBuffer.write(s1, s - s1 - 1);

        if(ch == '%')
        {
            switch(ch = *s++)
            {
            case 's':
            case 'd':
                strBuffer << *v8::String::Utf8Value(args[idx ++]);
                break;
            case 'j':
            {
                v8::Handle<v8::Context> context = v8::Context::GetCurrent();
                v8::Handle<v8::Object> global = context->Global();

                v8::Handle<v8::Object> JSON = global->Get(v8::String::New("JSON"))->ToObject();
                v8::Handle<v8::Function> JSON_stringify = v8::Handle<v8::Function>::Cast(JSON->Get(v8::String::New("stringify")));

                v8::Handle<v8::Value> myargs[] = {args[idx ++]};

                strBuffer << *v8::String::Utf8Value(JSON_stringify->Call(JSON, 1, myargs));
            }
            break;
            default:
                strBuffer << '%';
            case '%':
                strBuffer << ch;
                break;
            }
        }
        else break;
    }

    while(idx < argc)
        strBuffer << ' ' << *v8::String::Utf8Value(args[idx ++]);

    return strBuffer.str();
}

result_t console_base::log(const char* fmt, const v8::Arguments& args)
{
    log4cpp::Category::getRoot().info(Format(fmt, args));
    return 0;
}

result_t console_base::info(const char* fmt, const v8::Arguments& args)
{
    log4cpp::Category::getRoot().info(Format(fmt, args));
    return 0;
}

result_t console_base::warn(const char* fmt, const v8::Arguments& args)
{
    log4cpp::Category::getRoot().warn(Format(fmt, args));
    return 0;
}

result_t console_base::error(const char* fmt, const v8::Arguments& args)
{
    log4cpp::Category::getRoot().error(Format(fmt, args));
    return 0;
}

static std::map<std::string, int64_t> s_timers;

result_t console_base::time(const char* label)
{
    s_timers[std::string(label)] = Ticks();

    return 0;
}

result_t console_base::timeEnd(const char* label)
{
    int64_t t = Ticks() - s_timers[label];

    s_timers.erase(label);

    printf("%s: %ldms\n", label, t);

    return 0;
}

inline const char* ToCString(const v8::String::Utf8Value& value)
{
    return *value ? *value : "<string conversion failed>";
}

result_t console_base::trace(const char* label)
{
    v8::Handle<v8::StackTrace> stackTrace =
        v8::StackTrace::CurrentStackTrace(10, v8::StackTrace::kOverview);
    int count = stackTrace->GetFrameCount();
    int i;
    std::stringstream strBuffer;

    strBuffer << "console.trace: " << label;

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

    log4cpp::Category::getRoot().debug(strBuffer.str());

    return 0;
}

};
