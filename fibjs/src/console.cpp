/*
 * console.cpp
 *
 *  Created on: Apr 7, 2012
 *      Author: lion
 */

#include "ifs/console.h"
#include "ifs/assert.h"
#include "ifs/encoding.h"
#include "ifs/process.h"
#include <log4cpp/Category.hh>
#include <sstream>
#include "MyAppender.h"

#ifdef _WIN32
#include <windows.h>

static LARGE_INTEGER systemFrequency;

inline int64_t Ticks()
{
    LARGE_INTEGER t;

    if (systemFrequency.QuadPart == 0)
        QueryPerformanceFrequency(&systemFrequency);

    QueryPerformanceCounter(&t);

    return t.QuadPart * 1000000 / systemFrequency.QuadPart;
}

#else
#include <sys/time.h>
#include <readline/readline.h>
#include <readline/history.h>

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

result_t console_base::get_stderr(obj_ptr<BufferedStream_base> &retVal)
{
    return process_base::get_stderr(retVal);
}

result_t console_base::get_stdin(obj_ptr<BufferedStream_base> &retVal)
{
    return process_base::get_stdin(retVal);
}

result_t console_base::get_stdout(obj_ptr<BufferedStream_base> &retVal)
{
    return process_base::get_stdout(retVal);
}

extern int32_t g_loglevel;

result_t console_base::get_loglevel(int32_t &retVal)
{
    retVal = g_loglevel;
    return 0;
}

result_t console_base::set_loglevel(int32_t newVal)
{
    g_loglevel = newVal;
    return 0;
}

std::string Format(const char *fmt, const v8::FunctionCallbackInfo<v8::Value> &args, int idx = 1)
{
    const char *s = fmt;
    const char *s1;
    char ch;
    int argc = args.Length();
    std::stringstream strBuffer;

    if (argc == 1)
        return fmt;

    while (1)
    {
        if (idx >= argc)
        {
            strBuffer << s;
            break;
        }

        s1 = s;
        while ((ch = *s++) && (ch != '%'))
            ;

        strBuffer.write(s1, s - s1 - 1);

        if (ch == '%')
        {
            switch (ch = *s++)
            {
            case 's':
            case 'd':
            {
                v8::String::Utf8Value s(args[idx++]);
                if (*s)
                    strBuffer << *s;
            }
            break;
            case 'j':
            {
                std::string s;
                encoding_base::jsonEncode(args[idx++], s);
                strBuffer << s;
            }
            break;
            default:
                strBuffer << '%';
            case '%':
                strBuffer << ch;
                break;
            }
        }
        else
            break;
    }

    while (idx < argc)
    {
        v8::String::Utf8Value s(args[idx++]);
        if (*s)
            strBuffer << ' ' << *s;
    }

    return strBuffer.str();
}

result_t console_base::log(const char *fmt, const v8::FunctionCallbackInfo<v8::Value> &args)
{
    asyncLog(log4cpp::Priority::INFO, Format(fmt, args));
    return 0;
}

result_t console_base::info(const char *fmt, const v8::FunctionCallbackInfo<v8::Value> &args)
{
    asyncLog(log4cpp::Priority::INFO, Format(fmt, args));
    return 0;
}

result_t console_base::warn(const char *fmt, const v8::FunctionCallbackInfo<v8::Value> &args)
{
    asyncLog(log4cpp::Priority::WARN, Format(fmt, args));
    return 0;
}

result_t console_base::error(const char *fmt, const v8::FunctionCallbackInfo<v8::Value> &args)
{
    asyncLog(log4cpp::Priority::ERROR, Format(fmt, args));
    return 0;
}

inline void newline(std::stringstream &strBuffer, int padding)
{
    static char s_spc[] =
        "                                                                ";
    int n, n1;

    strBuffer << std::endl;
    if (padding > 0)
    {
        n = padding;
        while (n)
        {
            n1 = n > 64 ? 64 : n;
            strBuffer.write(s_spc, n1);
            n -= n1;
        }
    }
}

result_t console_base::dir(v8::Local<v8::Object> obj)
{
    std::stringstream strBuffer;
    std::string s;
    encoding_base::jsonEncode(obj, s);
    const char *p;
    int padding = 0;
    char ch;
    bool bStrMode = false;
    bool bNewLine = false;
    const int tab_size = 2;

    p = s.c_str();
    while ((ch = *p++) != 0)
    {
        if (bStrMode)
        {
            if (ch == '\\' && *p == '\"')
            {
                strBuffer << ch;
                ch = *p++;
            }
            else if (ch == '\"')
                bStrMode = false;
        }
        else
        {
            switch (ch)
            {
            case '[':
                if (*p == ']')
                {
                    strBuffer << ch;
                    ch = *p ++;
                    break;
                }

                bNewLine = true;
                padding += tab_size;
                break;
            case '{':
                if (*p == '}')
                {
                    strBuffer << ch;
                    ch = *p ++;
                    break;
                }

                bNewLine = true;
                padding += tab_size;
                break;
            case '}':
            case ']':
                padding -= tab_size;
                newline(strBuffer, padding);
                break;
            case ',':
                bNewLine = true;
                break;
            case ':':
                strBuffer << ch;
                ch = ' ';
                break;
            case '\"':
                bStrMode = true;
                break;
            }
        }

        strBuffer << ch;

        if (bNewLine)
        {
            newline(strBuffer, padding);
            bNewLine = false;
        }
    }

    asyncLog(log4cpp::Priority::INFO, strBuffer.str());
    return 0;
}

static std::map<std::string, int64_t> s_timers;

result_t console_base::time(const char *label)
{
    s_timers[std::string(label)] = Ticks();

    return 0;
}

result_t console_base::timeEnd(const char *label)
{
    long t = (long) (Ticks() - s_timers[label]);

    s_timers.erase(label);

    std::stringstream strBuffer;

    strBuffer << label << ": " << (t / 1000.0) << "ms";

    asyncLog(log4cpp::Priority::INFO, strBuffer.str());

    return 0;
}

inline const char *ToCString(const v8::String::Utf8Value &value)
{
    return *value ? *value : "<string conversion failed>";
}

result_t console_base::trace(const char *label)
{
    std::stringstream strBuffer;

    strBuffer << "console.trace: " << label;
    strBuffer << traceInfo();

    asyncLog(log4cpp::Priority::WARN, strBuffer.str());

    return 0;
}

#ifdef assert
#undef assert
#endif

result_t console_base::assert(v8::Local<v8::Value> value, const char *msg)
{
    return assert_base::ok(value, msg);
}

result_t console_base::print(const char *fmt, const v8::FunctionCallbackInfo<v8::Value> &args)
{
    MyAppender ma;

    flushLog();
    ma.out(Format(fmt, args).c_str());

    return 0;
}

result_t console_base::readLine(const char *msg, std::string &retVal,
                                exlib::AsyncEvent *ac)
{
    if (!ac)
    {
        flushLog();
        return CALL_E_NOSYNC;
    }

#ifdef _WIN32
    MyAppender ma;

    ma.out(msg);
    std::getline(std::cin, retVal);
#else
    char *line;

    line = readline(msg);
    if (line != NULL && *line)
    {
        add_history(line);
        retVal = line;
    }
#endif

    return 0;
}

}
;
