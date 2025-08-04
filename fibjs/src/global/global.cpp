#include "object.h"
#include "ifs/global.h"
#include "ifs/coroutine.h"
#include "ifs/process.h"
#include "ifs/base64.h"
#include "ifs/http.h"
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

result_t global_base::run(exlib::string fname)
{
    return Isolate::current()->m_topSandbox->run(fname);
}

result_t global_base::require(exlib::string id, v8::Local<v8::Value>& retVal)
{
    return Isolate::current()->m_topSandbox->require(id, s_root, retVal);
}

result_t global_base::fetch(exlib::string url, v8::Local<v8::Object> opts,
    obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac)
{
    return http_base::request(url, opts, retVal, ac);
}

}
