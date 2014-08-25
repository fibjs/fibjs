#include "ifs/global.h"
#include "ifs/coroutine.h"
#include "ifs/vm.h"
#include "SandBox.h"
#include "Integer64.h"

namespace fibjs
{

result_t global_base::get_console(obj_ptr<console_base> &retVal)
{
    static obj_ptr<console_base> s_console;

    if (s_console == NULL)
        s_console = new console_base();

    retVal = s_console;

    return 0;
}

result_t global_base::GC()
{
    isolate->LowMemoryNotification();
    return 0;
}

extern obj_ptr<SandBox> s_topSandbox;
result_t global_base::run(const char *fname)
{
    return s_topSandbox->run(fname);
}

result_t global_base::require(const char *id, v8::Local<v8::Value> &retVal)
{
    return s_topSandbox->require(id, retVal);
}

result_t global_base::int64(int64_t hi, int64_t lo, obj_ptr<Integer64_base> &retVal)
{
    retVal = new Integer64(hi, lo);
    return 0;
}

result_t global_base::int64String(const char *num, double base, obj_ptr<Integer64_base> &retVal)
{
    retVal = new Integer64(0);
    (Integer64) retVal->fromString(num, base);
    return 0;
}

result_t global_base::int64Hex(const char *hex, obj_ptr<Integer64_base> &retVal)
{
    retVal = new Integer64(0);
    (Integer64) retVal->fromHex(hex);
    return 0;
}

}
