#include "object.h"
#include "ifs/global.h"
#include "ifs/coroutine.h"
#include "ifs/process.h"
#include "ifs/base64.h"
#include "ifs/http.h"
#include "WebResponse.h"
#include "SandBox.h"
#include "Runtime.h"
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
    Isolate* isolate = Isolate::current();

    // Check if we are evaluating an ES module
    // global.require should only work in REPL/script context, not in ES modules
    if (isolate->m_module_evaluating > 0)
        return Runtime::setError("require is not defined in ES module scope, use import instead");

    return isolate->m_topSandbox->require(id, s_root, retVal);
}

result_t global_base::fetch(exlib::string url, v8::Local<v8::Object> opts,
    obj_ptr<WebResponse_base>& retVal, AsyncEvent* ac)
{
    return http_base::fetch(url, opts, retVal, ac);
}

result_t global_base::fetch(HttpRequest_base* request, v8::Local<v8::Object> opts,
    obj_ptr<WebResponse_base>& retVal, AsyncEvent* ac)
{
    return http_base::fetch(request, opts, retVal, ac);
}

result_t global_base::queueMicrotask(v8::Local<v8::Function> callback)
{
    Isolate* isolate = Isolate::current();
    isolate->m_isolate->EnqueueMicrotask(callback);
    return 0;
}

}
