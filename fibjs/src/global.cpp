#include "ifs/global.h"
#include "ifs/Fiber.h"

#ifdef _WIN32
#include <mmsystem.h>
inline int64_t Ticks()
{
    return timeGetTime();  // Convert to microseconds.
}

#else
#include <sys/time.h>

inline int64_t Ticks()
{
    struct timeval tv;
    if (gettimeofday(&tv, NULL) < 0)
        return 0;
    return (tv.tv_sec * 1000ll) + tv.tv_usec / 1000;
}

#endif

namespace fibjs
{

extern v8::Isolate* isolate;

result_t global_base::yield()
{
    v8::Unlocker unlocker(isolate);
    fiber::Fiber::yield();

    return 0;
}

result_t global_base::get_console(obj_ptr<console_base>& retVal)
{
    static obj_ptr<console_base> s_console;

    if(s_console == NULL)
        s_console = new console_base();

    retVal = s_console;

    return 0;
}

result_t global_base::get_os(obj_ptr<os_base>& retVal)
{
    static obj_ptr<os_base> s_os;

    if(s_os == NULL)
        s_os = new os_base();

    retVal = s_os;

    return 0;
}

result_t global_base::get_fs(obj_ptr<fs_base>& retVal)
{
    static obj_ptr<fs_base> s_fs;

    if(s_fs == NULL)
        s_fs = new fs_base();

    retVal = s_fs;

    Ticks();

    return 0;
}

result_t global_base::print(const char* fmt, const v8::Arguments& args)
{
    return console_base::log(fmt, args);
}

result_t global_base::GC()
{
    v8::V8::LowMemoryNotification();
    return 0;
}

}
