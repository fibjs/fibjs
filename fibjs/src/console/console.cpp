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
#include "ifs/util.h"
#include <map>
#include "console.h"
#include <stdlib.h>

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
#include <dlfcn.h>
#include <sys/time.h>

#include "utils.h" // for ARRAYSIZE()

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

extern std_logger* s_std;

void _log(int32_t type, const char *fmt, const v8::FunctionCallbackInfo<v8::Value> &args)
{
    int32_t level;

    console_base::get_loglevel(level);

    if (type <= level)
    {
        qstring str;

        util_base::format(fmt, args, str);
        asyncLog(type, str);
    }
}

result_t console_base::log(const char *fmt, const v8::FunctionCallbackInfo<v8::Value> &args)
{
    _log(_INFO, fmt, args);
    return 0;
}

result_t console_base::log(const v8::FunctionCallbackInfo<v8::Value> &args)
{
    return log(NULL, args);
}

result_t console_base::debug(const char *fmt, const v8::FunctionCallbackInfo<v8::Value> &args)
{
    _log(_DEBUG, fmt, args);
    return 0;
}

result_t console_base::debug(const v8::FunctionCallbackInfo<v8::Value> &args)
{
    return debug(NULL, args);
}

result_t console_base::info(const char *fmt, const v8::FunctionCallbackInfo<v8::Value> &args)
{
    _log(_INFO, fmt, args);
    return 0;
}

result_t console_base::info(const v8::FunctionCallbackInfo<v8::Value> &args)
{
    return info(NULL, args);
}

result_t console_base::notice(const char *fmt, const v8::FunctionCallbackInfo<v8::Value> &args)
{
    _log(_NOTICE, fmt, args);
    return 0;
}

result_t console_base::notice(const v8::FunctionCallbackInfo<v8::Value> &args)
{
    return notice(NULL, args);
}

result_t console_base::warn(const char *fmt, const v8::FunctionCallbackInfo<v8::Value> &args)
{
    _log(_WARN, fmt, args);
    return 0;
}

result_t console_base::warn(const v8::FunctionCallbackInfo<v8::Value> &args)
{
    return warn(NULL, args);
}

result_t console_base::error(const char *fmt, const v8::FunctionCallbackInfo<v8::Value> &args)
{
    _log(_ERROR, fmt, args);
    return 0;
}

result_t console_base::error(const v8::FunctionCallbackInfo<v8::Value> &args)
{
    return error(NULL, args);
}

result_t console_base::crit(const char *fmt, const v8::FunctionCallbackInfo<v8::Value> &args)
{
    _log(_CRIT, fmt, args);
    return 0;
}

result_t console_base::crit(const v8::FunctionCallbackInfo<v8::Value> &args)
{
    return crit(NULL, args);
}

result_t console_base::alert(const char *fmt, const v8::FunctionCallbackInfo<v8::Value> &args)
{
    _log(_ALERT, fmt, args);
    return 0;
}

result_t console_base::alert(const v8::FunctionCallbackInfo<v8::Value> &args)
{
    return alert(NULL, args);
}

qstring json_format(v8::Local<v8::Value> obj);

result_t console_base::dir(v8::Local<v8::Value> obj)
{
    qstring strBuffer = json_format(obj);
    asyncLog(_INFO, strBuffer);
    return 0;
}

static std::map<qstring, int64_t> s_timers;

result_t console_base::time(const char *label)
{
    s_timers[qstring(label)] = Ticks();

    return 0;
}

result_t console_base::timeEnd(const char *label)
{
    long t = (long) (Ticks() - s_timers[label]);

    s_timers.erase(label);

    qstring strBuffer;
    char numStr[64];

    sprintf(numStr, "%.10g", t / 1000.0);

    strBuffer.append(label);
    strBuffer.append(": ", 2);
    strBuffer.append(numStr);
    strBuffer.append("ms", 2);

    asyncLog(_INFO, strBuffer);
    return 0;
}

inline const char *ToCString(const v8::String::Utf8Value &value)
{
    return *value ? *value : "<string conversion failed>";
}

result_t console_base::trace(const char *label)
{
    qstring strBuffer;

    strBuffer.append("console.trace: ", 15);
    strBuffer.append(label);
    strBuffer.append(traceInfo(10));

    asyncLog(_WARN, strBuffer);
    return 0;
}

result_t console_base::_assert(v8::Local<v8::Value> value, const char *msg)
{
    return assert_base::ok(value, msg);
}

result_t console_base::print(const char *fmt, const v8::FunctionCallbackInfo<v8::Value> &args)
{
    _log(_PRINT, fmt, args);
    return 0;
}

result_t console_base::print(const v8::FunctionCallbackInfo<v8::Value> &args)
{
    return print(NULL, args);
}

char *read_line()
{
    char *text = (char *)malloc(1024);

    if (fgets(text, 1024, stdin) != NULL)
    {
        int32_t textLen = (int32_t)qstrlen(text);
        if (textLen > 0 && text[textLen - 1] == '\n')
            text[textLen - 1] = '\0';     // getting rid of newline character
        return text;
    }

    free(text);
    return NULL;
}

#ifndef _WIN32
extern "C"
{
    void __real_free(void*);
    char *readline(const char *prompt);
    void add_history(char *line);
}
#endif

result_t console_base::readLine(const char *msg, qstring &retVal,
                                AsyncEvent *ac)
{
    if (!ac)
    {
        flushLog(true);
        return CHECK_ERROR(CALL_E_LONGSYNC);
    }

#ifndef _WIN32
    static bool _init = false;
    static char *(*_readline)(const char *);
    static void (*_add_history)(char *);

#ifdef DEBUG
#ifdef __clang__
    static void (*_free)(void*);

    if (_free == 0)
        _free = (void (*)(void*))dlsym(RTLD_NEXT, "free");
#else
#define _free __real_free
#endif
#else
#define _free free
#endif

    if (!_init)
    {
        _init = true;

#ifdef __clang__
        void *handle = dlopen("libreadline.dylib", RTLD_LAZY);
#else
        const char *readline_dylib_names[] =
        {
            "libreadline.so.6",
            "libreadline.so.5",
            "libreadline.so"
        };
        const size_t readline_dylib_names_size = ARRAYSIZE(readline_dylib_names);
        void *handle = 0;

        for (size_t i = 0; i < readline_dylib_names_size; i++)
        {
            handle = dlopen(readline_dylib_names[i], RTLD_LAZY);
            if (handle) break;
        }
#endif

        if (handle)
        {
            _readline = (char *(*)(const char *))dlsym(handle, "readline");
            _add_history = (void (*)(char *))dlsym(handle, "add_history");
        } else
        {
            _readline = readline;
            _add_history = add_history;
        }
    }

    if (isatty(fileno(stdin)))
    {
        qstring strmsg = msg;
        char *line;
        int32_t lfpos = strmsg.find_last_of(0x0a);

        if ( lfpos >= 0 )
        {
            puts (strmsg.substr(0, lfpos).c_str());
            line = _readline( strmsg.substr(lfpos + 1).c_str() );
        }
        else
            line = _readline( msg );

        if (!line)
            return CHECK_ERROR(LastError());

        if (*line)
        {
            _add_history(line);
            retVal = line;
        }
        _free(line);
    } else
#endif
    {
        s_std->out(msg);
        char *line = read_line();

        if (!line)
            return CHECK_ERROR(LastError());

        retVal = line;
        free(line);
    }

    return 0;
}

}
;
