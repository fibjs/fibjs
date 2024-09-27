/*
 * Script.cpp
 *
 *  Created on: Oct 15, 2023
 *      Author: lion
 */

#include "object.h"
#include "ifs/vm.h"
#include "Script.h"
#include "SandBox.h"
#include "Buffer.h"
#include "Timer.h"

namespace fibjs {

void vm_get_global(v8::Local<v8::Object> obj, v8::Local<v8::Object>& retVal);

result_t Script_base::_new(exlib::string code, v8::Local<v8::Object> opts,
    obj_ptr<Script_base>& retVal, v8::Local<v8::Object> This)
{
    obj_ptr<Script> s = new Script();
    s->wrap(This);

    result_t hr = s->init(code, opts);
    if (hr == 0)
        retVal = s;

    return hr;
}

result_t Script::init(exlib::string code, v8::Local<v8::Object> opts)
{
    Isolate* isolate = holder();
    result_t hr;

    exlib::string filename("<anonymous>");
    hr = GetConfigValue(isolate, opts, "filename", filename, true);
    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
        return hr;

    int32_t lineOffset = 0;
    hr = GetConfigValue(isolate, opts, "lineOffset", lineOffset, true);
    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
        return hr;

    int32_t columnOffset = 0;
    hr = GetConfigValue(isolate, opts, "columnOffset", columnOffset, true);
    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
        return hr;

    obj_ptr<Buffer_base> cachedData;
    hr = GetConfigValue(isolate, opts, "cachedData", cachedData, true);
    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
        return hr;

    v8::ScriptCompiler::CompileOptions compile_options = v8::ScriptCompiler::kNoCompileOptions;
    v8::ScriptCompiler::CachedData* cached_data = nullptr;
    if (cachedData != nullptr) {
        obj_ptr<Buffer> cachedData_ = cachedData.As<Buffer>();
        cached_data = new v8::ScriptCompiler::CachedData(cachedData_->data(), cachedData_->length());
        compile_options = v8::ScriptCompiler::kConsumeCodeCache;
    }

    v8::ScriptOrigin origin(isolate->NewString(filename), lineOffset, columnOffset);
    v8::ScriptCompiler::Source source(isolate->NewString(code), origin, cached_data);

    v8::MaybeLocal<v8::UnboundScript> maybe_ub_script
        = v8::ScriptCompiler::CompileUnboundScript(isolate->m_isolate, &source, compile_options);

    v8::Local<v8::UnboundScript> ub_script;
    if (maybe_ub_script.IsEmpty() || !maybe_ub_script.ToLocal(&ub_script))
        return CALL_E_JAVASCRIPT;

    m_script.Reset(isolate->m_isolate, ub_script);

    return 0;
}

result_t Script::runInContext(v8::Local<v8::Object> contextifiedObject, v8::Local<v8::Object> opts,
    v8::Local<v8::Value>& retVal)
{
    v8::Local<v8::Object> global;

    vm_get_global(contextifiedObject, global);
    if (global.IsEmpty())
        return CALL_E_BADVARTYPE;

    v8::Local<v8::Context> _context = global->GetCreationContextChecked();
    _context->Enter();
    result_t hr = runInThisContext(opts, retVal);
    _context->Exit();

    return hr;
}

result_t Script::runInNewContext(v8::Local<v8::Object> contextObject, v8::Local<v8::Object> opts,
    v8::Local<v8::Value>& retVal)
{
    Isolate* isolate = holder();
    v8::Local<v8::Object> global;

    result_t hr = vm_base::createContext(contextObject, v8::Object::New(isolate->m_isolate), global);
    if (hr < 0)
        return hr;

    vm_get_global(contextObject, global);
    v8::Local<v8::Context> _context = global->GetCreationContextChecked();
    _context->Enter();
    hr = runInThisContext(opts, retVal);
    _context->Exit();

    return hr;
}

result_t Script::runInThisContext(v8::Local<v8::Object> opts, v8::Local<v8::Value>& retVal)
{
    Isolate* isolate = holder();

    int32_t timeout = 0;
    result_t hr = GetConfigValue(isolate, opts, "timeout", timeout, true);
    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
        return hr;

    v8::Local<v8::UnboundScript> ub_script = v8::Local<v8::UnboundScript>::New(isolate->m_isolate, m_script);
    v8::Local<v8::Script> script = ub_script->BindToCurrentContext();
    v8::MaybeLocal<v8::Value> result;

    if (timeout > 0) {
        TimeoutScope ts(timeout);
        result = script->Run(isolate->context());
        hr = ts.result(result.IsEmpty() ? CALL_E_JAVASCRIPT : 0);
        if (hr < 0)
            return hr;
    } else
        result = script->Run(isolate->context());

    if (result.IsEmpty() || !result.ToLocal(&retVal))
        return CALL_E_JAVASCRIPT;

    return 0;
}

result_t Script::createCachedData(obj_ptr<Buffer_base>& retVal)
{
    Isolate* isolate = holder();
    v8::Local<v8::UnboundScript> ub_script = v8::Local<v8::UnboundScript>::New(isolate->m_isolate, m_script);

    std::unique_ptr<v8::ScriptCompiler::CachedData> new_cached_data;
    new_cached_data.reset(v8::ScriptCompiler::CreateCodeCache(ub_script));
    retVal = new Buffer(new_cached_data->data, new_cached_data->length);

    return 0;
}

}
