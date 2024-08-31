/*
 * vm.cpp
 *
 *  Created on: Oct 15, 2023
 *      Author: lion
 */

#include "object.h"
#include "ifs/vm.h"

namespace fibjs {

DECLARE_MODULE(vm);

void vm_get_global(v8::Local<v8::Object> obj, v8::Local<v8::Object>& retVal)
{
    Isolate* isolate = Isolate::current(obj);
    v8::Local<v8::Context> context = isolate->context();

    v8::MaybeLocal<v8::Value> maybe_value = obj->GetPrivate(context,
        v8::Private::ForApi(isolate->m_isolate, isolate->NewString("_global")));
    if (!maybe_value.IsEmpty()) {
        v8::Local<v8::Value> value;
        if (maybe_value.ToLocal(&value) && value->IsObject())
            retVal = value.As<v8::Object>();
    }
}

result_t vm_base::createContext(v8::Local<v8::Object> contextObject, v8::Local<v8::Object> opts, v8::Local<v8::Object>& retVal)
{
    vm_get_global(contextObject, retVal);
    if (!retVal.IsEmpty()) {
        retVal = contextObject;
        return 0;
    }

    Isolate* isolate = Isolate::current(contextObject);
    obj_ptr<SandBox_base> sbox;

    retVal = contextObject;
    return SandBox_base::_new(v8::Object::New(isolate->m_isolate), contextObject, sbox);
}

result_t vm_base::isContext(v8::Local<v8::Object> contextObject, bool& retVal)
{
    v8::Local<v8::Object> _global;
    vm_get_global(contextObject, _global);

    retVal = !_global.IsEmpty();

    return 0;
}

result_t vm_base::runInContext(exlib::string code, v8::Local<v8::Object> contextifiedObject,
    v8::Local<v8::Object> opts, v8::Local<v8::Value>& retVal)
{
    obj_ptr<Script_base> script;
    result_t hr = Script_base::_new(code, opts, script);
    if (hr < 0)
        return hr;

    return script->runInContext(contextifiedObject, opts, retVal);
}

result_t vm_base::runInContext(exlib::string code, v8::Local<v8::Object> contextifiedObject,
    exlib::string filename, v8::Local<v8::Value>& retVal)
{
    Isolate* isolate = Isolate::current(contextifiedObject);
    v8::Local<v8::Object> opts = v8::Object::New(isolate->m_isolate);
    opts->Set(isolate->context(), isolate->NewString("filename"), isolate->NewString(filename)).FromJust();
    return runInContext(code, contextifiedObject, opts, retVal);
}

result_t vm_base::runInNewContext(exlib::string code, v8::Local<v8::Object> contextObject,
    v8::Local<v8::Object> opts, v8::Local<v8::Value>& retVal)
{
    obj_ptr<Script_base> script;
    result_t hr = Script_base::_new(code, opts, script);
    if (hr < 0)
        return hr;

    return script->runInNewContext(contextObject, opts, retVal);
}

result_t vm_base::runInNewContext(exlib::string code, v8::Local<v8::Object> contextObject,
    exlib::string filename, v8::Local<v8::Value>& retVal)
{
    Isolate* isolate = Isolate::current(contextObject);
    v8::Local<v8::Object> opts = v8::Object::New(isolate->m_isolate);
    opts->Set(isolate->context(), isolate->NewString("filename"), isolate->NewString(filename)).FromJust();
    return runInNewContext(code, contextObject, opts, retVal);
}

result_t vm_base::runInThisContext(exlib::string code, v8::Local<v8::Object> opts,
    v8::Local<v8::Value>& retVal)
{
    obj_ptr<Script_base> script;
    result_t hr = Script_base::_new(code, opts, script);
    if (hr < 0)
        return hr;

    return script->runInThisContext(opts, retVal);
}

result_t vm_base::runInThisContext(exlib::string code, exlib::string filename,
    v8::Local<v8::Value>& retVal)
{
    Isolate* isolate = Isolate::current();
    v8::Local<v8::Object> opts = v8::Object::New(isolate->m_isolate);
    opts->Set(isolate->context(), isolate->NewString("filename"), isolate->NewString(filename)).FromJust();
    return runInThisContext(code, opts, retVal);
}

}
