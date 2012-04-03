#include "ifs/console.h"
#include <log4cpp/Category.hh>
#include <sstream>

#ifdef _WIN32
#include <windows.h>

static LARGE_INTEGER systemFrequency;

inline int64_t Ticks()
{
    LARGE_INTEGER t;

    if(systemFrequency.QuadPart == 0)
        QueryPerformanceFrequency(&systemFrequency);

    QueryPerformanceCounter(&t);

    return t.QuadPart * 1000000 / systemFrequency.QuadPart;
}

#else
#include <sys/time.h>

inline int64_t Ticks()
{
    struct timeval tv;
    if (gettimeofday(&tv, NULL) < 0)
        return 0;
    return (tv.tv_sec * 1000000ll) + tv.tv_usec;
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
    long t = (long)(Ticks() - s_timers[label]);

    s_timers.erase(label);

    log4cpp::Category::getRoot().info("%s: %Gms", label, t / 1000.0);

    return 0;
}

inline const char* ToCString(const v8::String::Utf8Value& value)
{
    return *value ? *value : "<string conversion failed>";
}

result_t console_base::trace(const char* label)
{
    std::stringstream strBuffer;

    strBuffer << "console.trace: " << label;
    strBuffer << traceInfo();

    log4cpp::Category::getRoot().warn(strBuffer.str());

    return 0;
}

#ifdef assert
#undef assert
#endif

result_t console_base::assert(bool value, const char* msg)
{
    if(!value)
    {
        std::stringstream strBuffer;

        strBuffer << "assert: " << msg;
        strBuffer << traceInfo();

        log4cpp::Category::getRoot().warn(strBuffer.str());
    }

    return 0;
}

};
