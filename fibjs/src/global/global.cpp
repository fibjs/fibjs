#include "object.h"
#include "ifs/global.h"
#include "ifs/coroutine.h"
#include "ifs/process.h"
#include "SandBox.h"
#include <vector>

namespace fibjs {

result_t global_base::get_Master(obj_ptr<Worker_base>& retVal)
{
    return CALL_RETURN_NULL;
}

result_t global_base::get_global(v8::Local<v8::Object>& retVal)
{
    Isolate* isolate = Isolate::current();
    retVal = isolate->context()->Global();
    return 0;
}

result_t global_base::GC()
{
    Isolate::current()->m_isolate->LowMemoryNotification();
    return 0;
}

result_t global_base::run(exlib::string fname, v8::Local<v8::Array> argv)
{
    return Isolate::current()->m_topSandbox->run(fname, argv);
}

result_t global_base::get_argv(v8::Local<v8::Array>& retVal)
{
    return process_base::get_argv(retVal);
}

result_t global_base::get___filename(exlib::string& retVal)
{
    return 0;
}

result_t global_base::get___dirname(exlib::string& retVal)
{
    return 0;
}

result_t global_base::require(exlib::string id, v8::Local<v8::Value>& retVal)
{
    return Isolate::current()->m_topSandbox->require(id, "", retVal);
}

result_t global_base::repl(v8::Local<v8::Array> cmds)
{
    return Isolate::current()->m_topSandbox->repl(cmds);
}

result_t global_base::repl(Stream_base* out, v8::Local<v8::Array> cmds)
{
    return Isolate::current()->m_topSandbox->repl(cmds, out);
}
}
