/*
 * Script.cpp
 *
 *  Created on: Oct 15, 2023
 *      Author: lion
 */

#include "object.h"
#include "ifs/vm.h"
#include "ifs/ScriptOptions.h"
#include "ifs/ScriptRunOptions.h"
#include "Script.h"
#include "SandBox.h"
#include "Buffer.h"
#include "Timer.h"

namespace fibjs {

void vm_get_global(v8::Local<v8::Object> obj, v8::Local<v8::Object>& retVal);

result_t Script_base::_new(exlib::string code, ScriptOptions* opts,
    obj_ptr<Script_base>& retVal, v8::Local<v8::Object> This)
{
    obj_ptr<Script> s = new Script();
    s->wrap(This);

    result_t hr = s->init(code, opts);
    if (hr.ok())
        retVal = s;

    return hr;
}

result_t Script::init(exlib::string code, ScriptOptions* opts)
{
    Isolate* isolate = holder();

    exlib::string filename("<anonymous>");
    int32_t lineOffset = 0;
    int32_t columnOffset = 0;
    exlib::string type("script");
    obj_ptr<Buffer_base> cachedData;

    if (opts) {
        if (opts->filename.has_value())
            filename = opts->filename.value();
        if (opts->lineOffset.has_value())
            lineOffset = opts->lineOffset.value();
        if (opts->columnOffset.has_value())
            columnOffset = opts->columnOffset.value();
        if (opts->type.has_value())
            type = opts->type.value();
        if (opts->cachedData.has_value())
            cachedData = opts->cachedData.value();
    }

    m_isModule = (type == "module");

    v8::ScriptCompiler::CompileOptions compile_options = v8::ScriptCompiler::kNoCompileOptions;
    v8::ScriptCompiler::CachedData* cached_data = nullptr;
    if (cachedData != nullptr) {
        obj_ptr<Buffer> cachedData_ = cachedData.As<Buffer>();
        cached_data = new v8::ScriptCompiler::CachedData(cachedData_->data(), cachedData_->length());
        compile_options = v8::ScriptCompiler::kConsumeCodeCache;
    }

    v8::ScriptOrigin origin(isolate->NewString(filename), lineOffset, columnOffset,
        false, -1, v8::Local<v8::Value>(), false, false, m_isModule);
    v8::ScriptCompiler::Source source(isolate->NewString(code), origin, cached_data);

    if (m_isModule) {
        v8::MaybeLocal<v8::Module> maybe_module = v8::ScriptCompiler::CompileModule(isolate->m_isolate, &source);
        v8::Local<v8::Module> module;
        if (maybe_module.IsEmpty() || !maybe_module.ToLocal(&module))
            return result_t(CALL_E_JAVASCRIPT);

        m_module.Reset(isolate->m_isolate, module);
    } else {
        v8::MaybeLocal<v8::UnboundScript> maybe_ub_script
            = v8::ScriptCompiler::CompileUnboundScript(isolate->m_isolate, &source, compile_options);

        v8::Local<v8::UnboundScript> ub_script;
        if (maybe_ub_script.IsEmpty() || !maybe_ub_script.ToLocal(&ub_script))
            return result_t(CALL_E_JAVASCRIPT);

        m_script.Reset(isolate->m_isolate, ub_script);
    }

    return result_t();
}

result_t Script::runInContext(v8::Local<v8::Object> contextifiedObject, ScriptRunOptions* opts,
    v8::Local<v8::Value>& retVal)
{
    v8::Local<v8::Object> global;

    vm_get_global(contextifiedObject, global);
    if (global.IsEmpty())
        return result_t::bad_vartype();

    v8::Local<v8::Context> _context;
    if (!global->GetCreationContext().ToLocal(&_context))
        return result_t::bad_vartype();

    _context->Enter();
    result_t hr = runInThisContext(opts, retVal);
    _context->Exit();

    return hr;
}

result_t Script::runInNewContext(v8::Local<v8::Object> contextObject, ScriptRunOptions* opts,
    v8::Local<v8::Value>& retVal)
{
    Isolate* isolate = holder();
    v8::Local<v8::Object> global;

    result_t hr = vm_base::createContext(contextObject, v8::Object::New(isolate->m_isolate), global);
    if (hr.failed())
        return hr;

    vm_get_global(contextObject, global);

    v8::Local<v8::Context> _context;
    if (global.IsEmpty() || !global->GetCreationContext().ToLocal(&_context))
        return result_t::bad_vartype();

    _context->Enter();
    hr = runInThisContext(opts, retVal);
    _context->Exit();

    return hr;
}

result_t Script::runInThisContext(ScriptRunOptions* opts, v8::Local<v8::Value>& retVal)
{
    Isolate* isolate = holder();

    int32_t timeout = opts ? opts->timeout.value_or(0) : 0;
    result_t hr;

    if (m_isModule) {
        // Module cannot be run directly, but compilation already validates syntax
        // Return undefined for module type
        retVal = v8::Undefined(isolate->m_isolate);
        return result_t();
    }

    v8::Local<v8::UnboundScript> ub_script = v8::Local<v8::UnboundScript>::New(isolate->m_isolate, m_script);
    if (ub_script.IsEmpty())
        return result_t::invalid_call();

    v8::Local<v8::Script> script = ub_script->BindToCurrentContext();
    v8::MaybeLocal<v8::Value> result;

    if (timeout > 0) {
        TimeoutScope ts(timeout);
        result = script->Run(isolate->context());
        hr = ts.result(result.IsEmpty() ? result_t(CALL_E_JAVASCRIPT) : result_t());
        if (hr.failed())
            return hr;
    } else
        result = script->Run(isolate->context());

    if (result.IsEmpty() || !result.ToLocal(&retVal))
        return result_t(CALL_E_JAVASCRIPT);

    return result_t();
}

result_t Script::createCachedData(obj_ptr<Buffer_base>& retVal)
{
    Isolate* isolate = holder();

    if (m_isModule) {
        // Module does not support cached data
        return result_t::invalid_call();
    }

    v8::Local<v8::UnboundScript> ub_script = v8::Local<v8::UnboundScript>::New(isolate->m_isolate, m_script);
    if (ub_script.IsEmpty())
        return result_t::invalid_call();

    std::unique_ptr<v8::ScriptCompiler::CachedData> new_cached_data;
    new_cached_data.reset(v8::ScriptCompiler::CreateCodeCache(ub_script));
    retVal = new Buffer(new_cached_data->data, new_cached_data->length);

    return result_t();
}

}
