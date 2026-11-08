/*
 * SandBox_context.cpp
 *
 *  Created on: Jun 7, 2017
 *      Author: lion
 */

#include "object.h"
#include "ifs/url.h"
#include "ifs/module.h"
#include "SandBox.h"
#include "path.h"

namespace fibjs {

static void _resolve(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t argc = args.Length();

    if (argc < 1) {
        ThrowResult(result_t(CALL_E_PARAMNOTOPTIONAL));
        return;
    }

    Isolate* isolate = Isolate::current(args);
    V8_SCOPE(isolate->m_isolate);

    exlib::string id;
    result_t hr = GetArgumentValue(isolate, args[0], id);
    if (hr.failed()) {
        ThrowResult(hr);
        return;
    }

    v8::Local<v8::Context> context = isolate->context();
    v8::Local<v8::Value> data = args.Data();
    v8::Local<v8::Object> _mod;
    if (data.IsEmpty() || !data->ToObject(context).ToLocal(&_mod) || _mod.IsEmpty()) {
        ThrowResult(result_t(CALL_E_JAVASCRIPT));
        return;
    }
    JSValue path = _mod->Get(context, isolate->NewString("_id"));
    obj_ptr<SandBox> sbox = (SandBox*)SandBox_base::getInstance(
        JSValue(_mod->Get(context, isolate->NewString("_sbox"))));

    exlib::string strPath;
    path_base::dirname(isolate->toString(path), strPath);

    exlib::string v;
    hr = sbox->resolve(id, strPath, v);
    if (hr.failed()) {
        if (hr.is_javascript()) {
            args.GetReturnValue().Set(v8::Local<v8::Value>());
            return;
        }
        ThrowResult(hr);
        return;
    }

    args.GetReturnValue().Set(V8_RETURN(sbox->holder()->NewString(v)));
}

static void _require(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t argc = args.Length();

    if (argc < 1) {
        ThrowResult(result_t(CALL_E_PARAMNOTOPTIONAL));
        return;
    }

    if (argc > 1) {
        ThrowResult(result_t(CALL_E_BADPARAMCOUNT));
        return;
    }

    Isolate* isolate = Isolate::current(args);
    V8_SCOPE(isolate->m_isolate);

    exlib::string id;
    result_t hr = GetArgumentValue(isolate, args[0], id);
    if (hr.failed()) {
        ThrowResult(hr);
        return;
    }

    v8::Local<v8::Context> context = isolate->context();
    v8::Local<v8::Value> data = args.Data();
    v8::Local<v8::Object> _mod;
    if (data.IsEmpty() || !data->ToObject(context).ToLocal(&_mod) || _mod.IsEmpty()) {
        ThrowResult(result_t(CALL_E_JAVASCRIPT));
        return;
    }
    JSValue path = _mod->Get(context, isolate->NewString("_id"));
    obj_ptr<SandBox> sbox = (SandBox*)SandBox_base::getInstance(
        JSValue(_mod->Get(context, isolate->NewString("_sbox"))));

    exlib::string strPath;
    path_base::dirname(isolate->toString(path), strPath);

    v8::Local<v8::Value> v;
    hr = sbox->run_module(id, strPath, v, true);
    if (hr.failed()) {
        if (hr.is_javascript()) {
            args.GetReturnValue().Set(v8::Local<v8::Value>());
            return;
        }
        ThrowResult(hr);
        return;
    }

    args.GetReturnValue().Set(V8_RETURN(v));
}

static void _run(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    Isolate* isolate = Isolate::current(args);
    V8_SCOPE(isolate->m_isolate);
    int32_t argc = args.Length();

    if (argc < 1) {
        ThrowResult(result_t(CALL_E_PARAMNOTOPTIONAL));
        return;
    }

    if (argc > 1) {
        ThrowResult(result_t(CALL_E_BADPARAMCOUNT));
        return;
    }

    exlib::string id;
    result_t hr = GetArgumentValue(isolate, args[0], id);
    if (hr.failed()) {
        ThrowResult(hr);
        return;
    }

    v8::Local<v8::Context> context = isolate->context();
    v8::Local<v8::Value> data = args.Data();
    v8::Local<v8::Object> _mod;
    if (data.IsEmpty() || !data->ToObject(context).ToLocal(&_mod) || _mod.IsEmpty()) {
        ThrowResult(result_t(CALL_E_JAVASCRIPT));
        return;
    }
    obj_ptr<SandBox> sbox = (SandBox*)SandBox_base::getInstance(
        JSValue(_mod->Get(context, isolate->NewString("_sbox"))));

    if (SandBox::is_relative(id)) {
        JSValue path = _mod->Get(context, isolate->NewString("_id"));

        exlib::string strPath;

        path_base::dirname(isolate->toString(path), strPath);
        if (strPath.length()) {
            resolvePath(strPath, id);
            id = strPath;
        }
    }

    hr = sbox->run(id, true);
    if (hr.failed()) {
        if (hr.is_javascript()) {
            args.GetReturnValue().Set(v8::Local<v8::Value>());
            return;
        }
        ThrowResult(hr);
        return;
    }
}

static void _createRequire(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t argc = args.Length();

    if (argc < 1) {
        ThrowResult(result_t(CALL_E_PARAMNOTOPTIONAL));
        return;
    }

    if (argc > 1) {
        ThrowResult(result_t(CALL_E_BADPARAMCOUNT));
        return;
    }

    Isolate* isolate = Isolate::current(args);
    v8::Local<v8::Context> context = isolate->context();
    V8_SCOPE(isolate->m_isolate);

    exlib::string id;
    result_t hr = GetArgumentValue(isolate, args[0], id);
    if (hr.failed()) {
        ThrowResult(hr);
        return;
    }

    if (!qstrcmp(id.c_str(), "file:", 5)) {
        exlib::string path;
        hr = url_base::fileURLToPath(id, v8::Local<v8::Object>(), path);
        if (hr.failed()) {
            ThrowResult(hr);
            return;
        }

        id = path;
    }

    v8::Local<v8::Object> _mod = v8::Object::New(isolate->m_isolate);

    _mod->Set(context, isolate->NewString("_sbox"), args.Data()).IsJust();
    _mod->Set(context, isolate->NewString("_id"), isolate->NewString(id)).IsJust();

    v8::Local<v8::Function> fn = isolate->NewFunction("require", _require, _mod);
    fn->Set(context, isolate->NewString("resolve"), isolate->NewFunction("resolve", _resolve, _mod)).IsJust();

    args.GetReturnValue().Set(fn);
}

// No-op stub for Node.js v22.8+ enableCompileCache compatibility
static void _enableCompileCache(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    Isolate* isolate = Isolate::current(args);
    v8::Local<v8::Context> context = isolate->context();
    v8::Local<v8::Object> result = v8::Object::New(isolate->m_isolate);

    result->Set(context, isolate->NewString("status"), v8::Integer::New(isolate->m_isolate, 2)).IsJust();
    result->Set(context, isolate->NewString("message"), isolate->NewString("Compile cache is not supported in fibjs")).IsJust();

    args.GetReturnValue().Set(result);
}

SandBox::Context::Context(SandBox* sb, exlib::string id)
    : m_sb(sb)
    , m_id(id)
{
    Isolate* isolate = m_sb->holder();
    v8::Local<v8::Context> context = isolate->context();

    v8::Local<v8::Object> _mod = v8::Object::New(isolate->m_isolate);

    _mod->Set(context, isolate->NewString("_sbox"), m_sb->wrap(isolate)).IsJust();
    _mod->Set(context, isolate->NewString("_id"), isolate->NewString(id)).IsJust();

    m_fnRequest = isolate->NewFunction("require", _require, _mod);
    m_fnRequest->Set(context, isolate->NewString("resolve"), isolate->NewFunction("resolve", _resolve, _mod)).IsJust();
    m_fnRequest->Set(context, isolate->NewString("cache"), m_sb->mods()).IsJust();

    m_fnRun = isolate->NewFunction("run", _run, _mod);
}

// The `Module` constructor exposed as module.Module.  Node's Module is the
// container a CommonJS file runs in (`id`, `path`, `exports`, `filename`,
// `loaded`, `children`); fibjs already runs every script in a wrapper of its
// own, so this is the user-constructible shape plus the statics Node puts on the
// class.  Creating one does not register it anywhere: it is a plain container,
// exactly like `new Module(id)` in Node without a parent.
static void _ModuleCtor(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    Isolate* isolate = Isolate::current(args);
    v8::Local<v8::Context> context = isolate->context();

    v8::Local<v8::Object> self = args.This();

    exlib::string id;
    if (args.Length() > 0 && args[0]->IsString())
        GetArgumentValue(isolate, args[0], id);

    self->Set(context, isolate->NewString("id"), isolate->NewString(id)).IsJust();
    self->Set(context, isolate->NewString("path"), isolate->NewString(".")).IsJust();
    self->Set(context, isolate->NewString("exports"), v8::Object::New(isolate->m_isolate)).IsJust();
    self->Set(context, isolate->NewString("filename"), v8::Null(isolate->m_isolate)).IsJust();
    self->Set(context, isolate->NewString("loaded"), v8::False(isolate->m_isolate)).IsJust();
    self->Set(context, isolate->NewString("children"),
         v8::Array::New(isolate->m_isolate))
        .IsJust();

    args.GetReturnValue().Set(self);
}

// A Module instance has to be callable-with-new but also usable straight from
// C++, so the constructor is built once per isolate and cached in the module
// object; this keeps `module.Module === require('module').Module`.
static v8::Local<v8::Function> get_Module_ctor(Isolate* isolate)
{
    return isolate->NewFunction("Module", _ModuleCtor);
}

void SandBox::initModule()
{
    Isolate* isolate = holder();
    v8::Local<v8::Context> context = isolate->context();

    v8::Local<v8::Object> _mod = v8::Object::New(isolate->m_isolate);
    _mod->Set(context, isolate->NewString("createRequire"), isolate->NewFunction("createRequire", _createRequire, wrap(isolate))).IsJust();

    // enableCompileCache is a no-op for compatibility with Node.js v22.8+
    _mod->Set(context, isolate->NewString("enableCompileCache"), isolate->NewFunction("enableCompileCache", _enableCompileCache)).IsJust();

    // isBuiltin is a C++ implementation (module.cpp); initModule builds this object
    // by hand instead of going through the generated class method table, so the
    // entry is wired here too.
    v8::Local<v8::Function> fnIsBuiltin = isolate->NewFunction("isBuiltin", module_base::s_static_isBuiltin);
    _mod->Set(context, isolate->NewString("isBuiltin"), fnIsBuiltin).IsJust();

    // Build builtinModules array
    std::vector<exlib::string> builtinModules;
    module_base::get_builtinModules(builtinModules);

    v8::Local<v8::Array> builtinModulesArray = v8::Array::New(isolate->m_isolate, (int32_t)builtinModules.size());
    for (int32_t i = 0; i < (int32_t)builtinModules.size(); i++)
        builtinModulesArray->Set(context, i, isolate->NewString(builtinModules[i])).IsJust();

    // Set builtinModules as a read-only property
    _mod->DefineOwnProperty(context, isolate->NewString("builtinModules"), builtinModulesArray,
        (v8::PropertyAttribute)(v8::ReadOnly | v8::DontDelete))
        .IsJust();

    // The Module container class, with the statics Node hangs off it.  The
    // functions are the ones the module object itself uses, so the two faces stay
    // in sync and `Module.isBuiltin === module.isBuiltin` holds.
    v8::Local<v8::Function> ModuleCtor = get_Module_ctor(isolate);
    ModuleCtor->Set(context, isolate->NewString("Module"), ModuleCtor).IsJust();
    ModuleCtor->Set(context, isolate->NewString("builtinModules"), builtinModulesArray).IsJust();
    ModuleCtor->Set(context, isolate->NewString("isBuiltin"), fnIsBuiltin).IsJust();
    ModuleCtor->Set(context, isolate->NewString("createRequire"),
         JSValue(_mod->Get(context, isolate->NewString("createRequire"))))
        .IsJust();

    _mod->Set(context, isolate->NewString("Module"), ModuleCtor).IsJust();

    // InstallModule is used instead of add() so that every prefix returns the
    // very same module object, as it does for all other builtin modules
    InstallModule("module", _mod);
    InstallModule("node:module", _mod);
    InstallModule("fibjs:module", _mod);
}

}