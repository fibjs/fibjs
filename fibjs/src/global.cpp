#include "ifs/global.h"

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
    obj_ptr<console_base> s_console;

    if(s_console == NULL)
        s_console = new console_base();

    retVal = s_console;

    return 0;
}

result_t global_base::get_os(obj_ptr<os_base>& retVal)
{
    obj_ptr<os_base> s_os;

    if(s_os == NULL)
        s_os = new os_base();

    retVal = s_os;

    return 0;
}

result_t global_base::get_fs(obj_ptr<fs_base>& retVal)
{
    obj_ptr<fs_base> s_fs;

    if(s_fs == NULL)
        s_fs = new fs_base();

    retVal = s_fs;

    return 0;
}

result_t global_base::print(const char* fmt, const v8::Arguments& args)
{
    return console_base::log(fmt, args);
}

}
