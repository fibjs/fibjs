#include "object.h"
#include "ifs/global.h"
#include "ifs/coroutine.h"
#include "ifs/process.h"
#include "ifs/base64.h"
#include "SandBox.h"
#include <vector>

namespace fibjs {

extern exlib::string s_root;

result_t global_base::get_global(v8::Local<v8::Object>& retVal)
{
    return get_globalThis(retVal);
}

result_t global_base::get_globalThis(v8::Local<v8::Object>& retVal)
{
    Isolate* isolate = Isolate::current();
    retVal = isolate->context()->Global();
    return 0;
}

result_t global_base::gc()
{
    Isolate::current()->m_isolate->LowMemoryNotification();
    return 0;
}

result_t global_base::run(exlib::string fname)
{
    return Isolate::current()->m_topSandbox->run(fname);
}

result_t global_base::require(exlib::string id, v8::Local<v8::Value>& retVal)
{
    return Isolate::current()->m_topSandbox->require(id, s_root, retVal);
}

result_t global_base::btoa(Buffer_base* data, bool url, exlib::string& retVal)
{
    return base64_base::encode(data, url, retVal);
}

result_t global_base::atob(exlib::string data, obj_ptr<Buffer_base>& retVal)
{
    return base64_base::decode(data, retVal);
}

}
