/*
 * mjs_loader.cpp
 *
 *  Created on: Jun 10, 2023
 *      Author: lion
 */

#include "object.h"
#include "path.h"
#include "SandBox.h"
#include "Buffer.h"
#include "loaders.h"
#include "../../util/ts_strip/ts_strip.h"
#include "ifs/url.h"

namespace fibjs {
class esm_importer : public object_base {
private:
    struct module_data {
        exlib::string id;
        v8::Global<v8::Module> module;
    };

public:
    esm_importer(SandBox* sb)
        : m_sb(sb)
        , m_isolate(sb->holder())
    {
    }

public:
    static esm_importer* getInstance(v8::Local<v8::Value> o)
    {
        return (esm_importer*)object_base::getInstance(o);
    }

    static bool is_typescript(exlib::string id)
    {
        if (id.length() < 3)
            return false;

        if (!qstrcmp(id.c_str() + id.length() - 3, ".ts"))
            return true;

        if (id.length() < 4)
            return false;

        if (!qstrcmp(id.c_str() + id.length() - 4, ".cts")
            || !qstrcmp(id.c_str() + id.length() - 4, ".mts"))
            return true;

        return false;
    }

public:
    result_t require(exlib::string id, Buffer_base* data, v8::Local<v8::Object> mod)
    {
        v8::Local<v8::Context> _context = m_isolate->context();

        v8::Local<v8::Module> root_module = load_module(id, (Buffer*)data, v8::Local<v8::Value>());
        if (root_module.IsEmpty())
            return CALL_E_JAVASCRIPT;

        v8::Local<v8::Promise::Resolver> resolver = v8::Promise::Resolver::New(_context).FromMaybe(v8::Local<v8::Promise::Resolver>());
        m_resolver.Reset(m_isolate->m_isolate, resolver);

        v8::Local<v8::Private> strPendding = v8::Private::ForApi(m_isolate->m_isolate, m_isolate->NewString("pendding"));
        mod->SetPrivate(_context, strPendding, resolver->GetPromise()).IsJust();

        // Mark as ESM module
        v8::Local<v8::Private> strIsModule = v8::Private::ForApi(m_isolate->m_isolate, m_isolate->NewString("isModule"));
        mod->SetPrivate(_context, strIsModule, v8::True(m_isolate->m_isolate)).IsJust();

        result_t hr = evaluate(id, mod, root_module);
        if (hr < 0) {
            saveModule();
            return hr;
        }

        v8::Local<v8::Value> result = m_isolate->await(resolver->GetPromise());
        if (result.IsEmpty())
            return CALL_E_JAVASCRIPT;

        return 0;
    }

    v8::MaybeLocal<v8::Promise> async_import(exlib::string id, exlib::string base)
    {
        result_t hr;
        v8::Local<v8::Context> _context = m_isolate->context();

        obj_ptr<Buffer_base> data;
        v8::Local<v8::Value> exports;
        v8::Local<v8::Value> pendding;
        bool isCJS = false;

        hr = resove_module(id, base, data, exports, pendding, isCJS);
        if (hr < 0) {
            ThrowResult(hr);
            return v8::MaybeLocal<v8::Promise>();
        }

        // If module is being loaded by another concurrent import, return its pendding promise
        if (!IsEmpty(pendding))
            return pendding.As<v8::Promise>();

        if (!IsEmpty(exports)) {
            // For cached modules, directly return the exports
            v8::Local<v8::Promise::Resolver> resolver = v8::Promise::Resolver::New(_context).FromMaybe(v8::Local<v8::Promise::Resolver>());
            resolver->Resolve(_context, exports).IsJust();

            return resolver->GetPromise();
        }

        SandBox::Context context(m_sb, id);

        v8::Local<v8::String> strExports = m_isolate->NewString("exports");
        v8::Local<v8::String> strModule = m_isolate->NewString(id);
        v8::Local<v8::Value> mod_exports = v8::Object::New(m_isolate->m_isolate);

        v8::Local<v8::Object> mod = v8::Object::New(m_isolate->m_isolate);
        mod->Set(_context, m_isolate->NewString("id"), strModule).IsJust();
        mod->Set(_context, strExports, mod_exports).IsJust();
        mod->Set(_context, m_isolate->NewString("filename"), strModule).IsJust();

        v8::Local<v8::Promise::Resolver> resolver = v8::Promise::Resolver::New(_context).FromMaybe(v8::Local<v8::Promise::Resolver>());
        m_resolver.Reset(m_isolate->m_isolate, resolver);

        v8::Local<v8::Private> strPendding = v8::Private::ForApi(m_isolate->m_isolate, m_isolate->NewString("pendding"));
        mod->SetPrivate(_context, strPendding, resolver->GetPromise()).IsJust();

        // Mark as ESM module
        v8::Local<v8::Private> strIsModule = v8::Private::ForApi(m_isolate->m_isolate, m_isolate->NewString("isModule"));
        mod->SetPrivate(_context, strIsModule, v8::True(m_isolate->m_isolate)).IsJust();

        v8::Local<v8::Object> mods = m_sb->mods();

        // Register module to mods BEFORE load_module to prevent race condition
        // when concurrent imports try to load the same module
        mods->Set(_context, strModule, mod).IsJust();

        v8::Local<v8::Module> root_module = load_module(id, data.As<Buffer>(), exports);
        if (root_module.IsEmpty()) {
            // Clean up on failure
            mod->DeletePrivate(_context, strPendding).IsJust();
            mod->Delete(_context, strExports).IsJust();
            mods->Delete(_context, strModule).IsJust();
            return v8::MaybeLocal<v8::Promise>();
        }

        hr = evaluate(id, mod, root_module);
        if (hr >= 0)
            return m_resolver.Get(m_isolate->m_isolate)->GetPromise();

        saveModule();

        mod->DeletePrivate(_context, strPendding).IsJust();

        mod->Delete(_context, strExports).IsJust();
        mods->Delete(_context, strModule).IsJust();

        return v8::MaybeLocal<v8::Promise>();
    }

private:
    // For dynamic import: check pendding promise and return it if module is being loaded
    result_t resove_module(exlib::string& id, exlib::string base, obj_ptr<Buffer_base>& data,
        v8::Local<v8::Value>& exports, v8::Local<v8::Value>& pendding, bool& isCJS)
    {
        result_t hr;

        v8::Local<v8::Object> mod;
        hr = m_sb->resolve(base, id, data, SandBox::kESModule, mod, &pendding);
        if (hr == CALL_E_FILE_NOT_FOUND)
            return CHECK_ERROR(Runtime::setError("Cannot find module '" + id + "' imported from " + m_sb->m_pending_module));

        if (hr < 0)
            return hr;

        // If pendding promise is returned, let caller handle it
        if (!IsEmpty(pendding))
            return 0;

        if (IsEmpty(mod)) {
            result_t hr;
            SandBox::ModuleType type;

            hr = m_sb->resolveModuleType(id, type);
            if (hr)
                return hr;

            if (type == SandBox::ModuleType::kCommonJS) {
                hr = m_sb->installScript(id, data, mod, false);
                if (hr < 0)
                    return hr;
            }
        }

        if (!IsEmpty(mod)) {
            // Check if module is ESM or CJS by looking at isModule private marker
            v8::Local<v8::Context> _context = m_isolate->context();
            v8::Local<v8::Private> strIsModule = v8::Private::ForApi(m_isolate->m_isolate, m_isolate->NewString("isModule"));
            JSValue isModuleVal = mod->GetPrivate(_context, strIsModule);
            bool isESM = !isModuleVal->IsUndefined() && isModuleVal->IsTrue();
            isCJS = !isESM;

            hr = m_sb->wait_module(mod, exports);
            if (hr < 0)
                return hr;

            return 0;
        }

        return 0;
    }

    // For static imports in resolveModuleTree: no pendding check needed
    result_t resove_module(exlib::string& id, exlib::string base, obj_ptr<Buffer_base>& data, v8::Local<v8::Value>& exports, bool& isCJS)
    {
        result_t hr;

        v8::Local<v8::Object> mod;
        hr = m_sb->resolve(base, id, data, SandBox::kESModule, mod);
        if (hr == CALL_E_FILE_NOT_FOUND)
            return CHECK_ERROR(Runtime::setError("Cannot find module '" + id + "' imported from " + m_sb->m_pending_module));

        if (hr < 0)
            return hr;

        if (IsEmpty(mod)) {
            result_t hr;
            SandBox::ModuleType type;

            hr = m_sb->resolveModuleType(id, type);
            if (hr)
                return hr;

            if (type == SandBox::ModuleType::kCommonJS) {
                hr = m_sb->installScript(id, data, mod, false);
                if (hr < 0)
                    return hr;
            }
        }

        if (!IsEmpty(mod)) {
            // Check if module is ESM or CJS by looking at isModule private marker
            v8::Local<v8::Context> _context = m_isolate->context();
            v8::Local<v8::Private> strIsModule = v8::Private::ForApi(m_isolate->m_isolate, m_isolate->NewString("isModule"));
            JSValue isModuleVal = mod->GetPrivate(_context, strIsModule);
            bool isESM = !isModuleVal->IsUndefined() && isModuleVal->IsTrue();
            isCJS = !isESM;

            hr = m_sb->wait_module(mod, exports);
            if (hr < 0)
                return hr;

            return 0;
        }

        return 0;
    }

    v8::Local<v8::Module> load_module(exlib::string id, Buffer* data_, v8::Local<v8::Value> exports)
    {
        class LoadScope {
        public:
            LoadScope(SandBox* sb)
                : m_rt(Runtime::current())
                , m_sb(sb)

            {
                m_sb->m_module_pendings++;
                m_prev_sb = m_rt->m_module_pending;
                m_rt->m_module_pending = m_sb;
            }

            ~LoadScope()
            {
                m_rt->m_module_pending = m_prev_sb;
                if (--m_sb->m_module_pendings == 0)
                    m_sb->module_deps_map.clear();
            }

        private:
            Runtime* m_rt;
            SandBox* m_sb;
            SandBox* m_prev_sb;
        };

        v8::Local<v8::Context> _context = m_isolate->context();

        LoadScope scope(m_sb);

        v8::Local<v8::Module> root_module = compile_module(id, data_, exports);
        if (root_module.IsEmpty())
            return root_module;

        result_t hr = resolveModuleTree(id, root_module);
        if (hr >= 0) {
            v8::Maybe<bool> result = root_module->InstantiateModule(_context, resolveModuleCallback);
            if (!result.FromMaybe(false))
                hr = CALL_E_JAVASCRIPT;
        }

        if (hr < 0) {
            if (hr != CALL_E_JAVASCRIPT)
                ThrowResult(hr);
            return v8::Local<v8::Module>();
        }

        return root_module;
    }

    v8::Local<v8::Module> compile_module(exlib::string id, Buffer* data_, v8::Local<v8::Value> exports, bool isCJS = false)
    {
        v8::Local<v8::Context> _context = m_isolate->context();
        v8::Local<v8::Module> module;

        SandBox::module_map_iter it = m_sb->module_map.find(id);
        if (it != m_sb->module_map.end()) {
            it->second.second++;
            module_refs.push_back(it);

            return it->second.first.Get(m_isolate->m_isolate);
        }

        if (!IsEmpty(exports)) {
            v8::Local<v8::Object> obj = exports->ToObject(_context).ToLocalChecked();
            v8::Local<v8::Array> names = obj->GetPropertyNames(_context).ToLocalChecked();
            int length = names->Length();
            std::vector<v8::Local<v8::String>> export_names;

            v8::Local<v8::String> strDefault = m_isolate->NewString("default");
            // For ESM (not CJS): exports is ModuleNamespace, need to get 'default' from it
            // For CJS: exports itself is the default value
            bool isESMNamespace = !isCJS && exports->IsModuleNamespaceObject();

            // For CJS: always add default (exports itself is the default)
            // For ESM: only add default if the module has it
            bool hasDefault = isCJS || obj->Has(_context, strDefault).FromMaybe(false);
            if (hasDefault)
                export_names.push_back(strDefault);
            for (int i = 0; i < length; ++i) {
                v8::Local<v8::String> name = names->Get(_context, i).ToLocalChecked()->ToString(_context).ToLocalChecked();
                v8::String::Utf8Value sname(m_isolate->m_isolate, name);
                if (qstrcmp(*sname, "default"))
                    export_names.push_back(name);
            }

            module = v8::Module::CreateSyntheticModule(m_isolate->m_isolate, m_isolate->NewString(id),
                v8::MemorySpan<const v8::Local<v8::String>>(export_names.data(), export_names.size()), ModuleEvaluationSteps);

            module->InstantiateModule(_context, resolveModuleCallback).IsJust();
            module->Evaluate(_context).FromMaybe(v8::Local<v8::Value>());

            // Set default export if present
            if (hasDefault) {
                // For ESM ModuleNamespace: get 'default' from namespace object
                // For CJS: use exports object itself as default
                if (isESMNamespace) {
                    v8::Local<v8::Value> defaultValue = obj->Get(_context, strDefault).FromMaybe(v8::Local<v8::Value>());
                    module->SetSyntheticModuleExport(m_isolate->m_isolate, strDefault, defaultValue).IsJust();
                } else {
                    module->SetSyntheticModuleExport(m_isolate->m_isolate, strDefault, exports).IsJust();
                }
            }

            // Set named exports
            size_t startIdx = hasDefault ? 1 : 0;
            for (size_t i = startIdx; i < export_names.size(); ++i) {
                v8::Local<v8::String> name = export_names[i];
                v8::Local<v8::Value> value = obj->Get(_context, name).FromMaybe(v8::Local<v8::Value>());
                if (value.IsEmpty())
                    return v8::Local<v8::Module>();
                module->SetSyntheticModuleExport(m_isolate->m_isolate, name, value).IsJust();
            }
        } else {
            exlib::string exception;

            {
                TryCatch try_catch;

                if (is_typescript(id)) {
                    // For TypeScript: strip types in-place on the buffer
                    try {
                        ts_strip::stripInPlace(data_->data(), data_->length());
                    } catch (const std::exception& e) {
                        ThrowError(e.what());
                        return v8::Local<v8::Module>();
                    }

                    v8::Local<v8::PrimitiveArray> pargs = v8::PrimitiveArray::New(m_isolate->m_isolate, 1);
                    pargs->Set(m_isolate->m_isolate, 0, v8::Number::New(m_isolate->m_isolate, m_sb->m_id));
                    v8::ScriptOrigin so_origin(m_isolate->NewString(id), 0, 0, false,
                        -1, v8::Local<v8::Value>(), false, false, true, pargs);

                    v8::ScriptCompiler::Source source(m_isolate->NewString((const char*)data_->data(), data_->length()), so_origin);
                    module = v8::ScriptCompiler::CompileModule(m_isolate->m_isolate, &source)
                                 .FromMaybe(v8::Local<v8::Module>());
                    if (module.IsEmpty())
                        exception = GetException(try_catch, 0, false, false);
                } else {
                    v8::Local<v8::PrimitiveArray> pargs = v8::PrimitiveArray::New(m_isolate->m_isolate, 1);
                    pargs->Set(m_isolate->m_isolate, 0, v8::Number::New(m_isolate->m_isolate, m_sb->m_id));
                    v8::ScriptOrigin so_origin(m_isolate->NewString(id), 0, 0, false,
                        -1, v8::Local<v8::Value>(), false, false, true, pargs);

                    v8::ScriptCompiler::Source source(m_isolate->NewString((const char*)data_->data(), data_->length()), so_origin);
                    module = v8::ScriptCompiler::CompileModule(m_isolate->m_isolate, &source).FromMaybe(v8::Local<v8::Module>());
                    if (module.IsEmpty())
                        exception = GetException(try_catch, 0, false, false);
                }
            }

            if (module.IsEmpty()) {
                ThrowError(exception);
                return v8::Local<v8::Module>();
            }

            m_sb->m_pending_module = id;
            initImportMeta(m_isolate->m_isolate, module);
        }

        it = m_sb->module_map.emplace(id, std::make_pair<v8::Global<v8::Module>, int32_t>(v8::Global<v8::Module>(m_isolate->m_isolate, module), 1)).first;
        module_refs.push_back(it);

        return module;
    }

    result_t resolveModuleTree(exlib::string base, v8::Local<v8::Module> module)
    {
        v8::Local<v8::Context> _context = m_isolate->context();
        int module_id = module->GetIdentityHash();
        std::unordered_map<int32_t, std::unordered_map<exlib::string, v8::Global<v8::Module>>>::iterator it = m_sb->module_deps_map.find(module_id);
        if (it == m_sb->module_deps_map.end())
            it = m_sb->module_deps_map.emplace(module_id, std::unordered_map<exlib::string, v8::Global<v8::Module>>()).first;
        std::unordered_map<exlib::string, v8::Global<v8::Module>>& deps = it->second;

        v8::Local<v8::FixedArray> module_requests = module->GetModuleRequests();
        for (int i = 0, length = module_requests->Length(); i < length; ++i) {
            v8::Local<v8::ModuleRequest> module_request = module_requests->Get(_context, i).As<v8::ModuleRequest>();
            exlib::string dep_id = ToString(m_isolate->m_isolate, module_request->GetSpecifier());
            exlib::string id = dep_id;

            if (deps.find(id) != deps.end())
                continue;

            result_t hr;
            obj_ptr<Buffer_base> data;
            v8::Local<v8::Value> exports;
            bool isCJS = false;

            exlib::string pname;
            path_base::dirname(base, pname);

            hr = resove_module(id, pname, data, exports, isCJS);
            if (hr < 0)
                return hr;

            v8::Local<v8::Module> dep_module = compile_module(id, data.As<Buffer>(), exports, isCJS);
            if (dep_module.IsEmpty())
                return CALL_E_JAVASCRIPT;

            deps.emplace(dep_id, v8::Global<v8::Module>(m_isolate->m_isolate, dep_module));

            hr = resolveModuleTree(id, dep_module);
            if (hr < 0)
                return hr;
        }

        return 0;
    }

    result_t evaluate(exlib::string id, v8::Local<v8::Object> mod, v8::Local<v8::Module> root_module)
    {
        v8::Local<v8::Context> _context = m_isolate->context();

        m_isolate->m_module_evaluating++;

        TryCatch try_catch;
        v8::Local<v8::Value> result = root_module->Evaluate(_context).FromMaybe(v8::Local<v8::Value>());

        m_isolate->m_module_evaluating--;

        if (result.IsEmpty()) {
            try_catch.ReThrow();
            return CALL_E_JAVASCRIPT;
        }

        v8::Local<v8::Promise> promise = result.As<v8::Promise>();
        promise->Then(_context,
                   v8::Function::New(_context, promise_then, wrap(m_isolate)).ToLocalChecked(),
                   v8::Function::New(_context, promise_reject, wrap(m_isolate)).ToLocalChecked())
            .FromMaybe(v8::Local<v8::Promise>());

        return 0;
    }

private:
    static v8::MaybeLocal<v8::Value> ModuleEvaluationSteps(v8::Local<v8::Context> context, v8::Local<v8::Module> module)
    {
        v8::Isolate* isolate = context->GetIsolate();

        v8::Local<v8::Promise::Resolver> resolver = v8::Promise::Resolver::New(context).ToLocalChecked();
        resolver->Resolve(context, v8::Undefined(isolate)).ToChecked();
        return resolver->GetPromise();
    }

    static v8::MaybeLocal<v8::Module> resolveModuleCallback(v8::Local<v8::Context> context,
        v8::Local<v8::String> specifier, v8::Local<v8::FixedArray> import_assertions,
        v8::Local<v8::Module> referrer)
    {
        Isolate* isolate = Isolate::current(context);
        Runtime* rt = Runtime::current();

        SandBox* sb = rt->m_module_pending;

        int module_id = referrer->GetIdentityHash();
        std::unordered_map<int32_t, std::unordered_map<exlib::string, v8::Global<v8::Module>>>::iterator it = sb->module_deps_map.find(module_id);
        if (it == sb->module_deps_map.end())
            return v8::MaybeLocal<v8::Module>();

        std::unordered_map<exlib::string, v8::Global<v8::Module>>& deps = it->second;
        exlib::string id = ToString(isolate->m_isolate, specifier);
        std::unordered_map<exlib::string, v8::Global<v8::Module>>::iterator it2 = deps.find(id);
        if (it2 == deps.end())
            return v8::MaybeLocal<v8::Module>();

        return v8::MaybeLocal<v8::Module>(it2->second.Get(isolate->m_isolate));
    }

    static void promise_then(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        Isolate* isolate = Isolate::current(args);
        v8::Local<v8::Context> context = isolate->context();
        obj_ptr<esm_importer> impoter = esm_importer::getInstance(args.Data());
        SandBox::module_map_iter& root_module = impoter->module_refs[0];
        v8::Local<v8::Module> module = root_module->second.first.Get(isolate->m_isolate);
        v8::Local<v8::Value> result = module->GetModuleNamespace();

        v8::Local<v8::Promise::Resolver> resolver = impoter->m_resolver.Get(isolate->m_isolate);
        resolver->Resolve(context, result).IsJust();

        v8::Local<v8::Object> mods = impoter->m_sb->mods();
        v8::Local<v8::Value> _mod = mods->Get(context, isolate->NewString(root_module->first)).FromMaybe(v8::Local<v8::Value>());
        if (!IsEmpty(_mod)) {
            v8::Local<v8::Object> mod = _mod.As<v8::Object>();
            v8::Local<v8::Private> strPendding = v8::Private::ForApi(isolate->m_isolate, isolate->NewString("pendding"));

            mod->DeletePrivate(context, strPendding).IsJust();
            mod->Set(context, isolate->NewString("exports"), result).IsJust();
        }

        impoter->saveModule();
    }

    static void promise_reject(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        Isolate* isolate = Isolate::current(args);
        v8::Local<v8::Context> context = isolate->context();

        obj_ptr<esm_importer> impoter = esm_importer::getInstance(args.Data());
        v8::Local<v8::Promise::Resolver> resolver = impoter->m_resolver.Get(isolate->m_isolate);
        resolver->Reject(context, args[0]).IsJust();

        SandBox::module_map_iter& root_module = impoter->module_refs[0];
        v8::Local<v8::Object> mods = impoter->m_sb->mods();
        v8::Local<v8::Value> _mod = mods->Get(context, isolate->NewString(root_module->first)).FromMaybe(v8::Local<v8::Value>());
        if (!IsEmpty(_mod)) {
            v8::Local<v8::Object> mod = _mod.As<v8::Object>();
            v8::Local<v8::Private> strPendding = v8::Private::ForApi(isolate->m_isolate, isolate->NewString("pendding"));

            mod->DeletePrivate(context, strPendding).IsJust();
            mod->Delete(context, isolate->NewString("exports")).IsJust();
            mods->Delete(context, isolate->NewString(root_module->first)).IsJust();
        }

        impoter->saveModule();
    }

    void saveModule()
    {
        v8::Local<v8::Context> _context = m_isolate->context();
        v8::Local<v8::Object> mods = m_sb->mods();

        for (int32_t i = 0, length = module_refs.size(); i < length; ++i) {
            SandBox::module_map_iter& it = module_refs[i];

            if (i > 0) {
                v8::Local<v8::Module> module = it->second.first.Get(m_isolate->m_isolate);
                v8::Module::Status status = module->GetStatus();
                if (status == v8::Module::Status::kEvaluated) {
                    v8::Local<v8::String> strId = m_isolate->NewString(it->first);
                    if (!mods->Has(_context, strId).FromMaybe(false)) {
                        v8::Local<v8::Object> mod = v8::Object::New(m_isolate->m_isolate);

                        mod->Set(_context, m_isolate->NewString("id"), strId).IsJust();
                        mod->Set(_context, m_isolate->NewString("exports"), module->GetModuleNamespace()).IsJust();
                        mod->Set(_context, m_isolate->NewString("filename"), strId).IsJust();

                        // Mark as ESM module
                        v8::Local<v8::Private> strIsModule = v8::Private::ForApi(m_isolate->m_isolate, m_isolate->NewString("isModule"));
                        mod->SetPrivate(_context, strIsModule, v8::True(m_isolate->m_isolate)).IsJust();

                        mods->Set(_context, strId, mod).IsJust();
                    }
                }
            }

            if (--it->second.second == 0)
                m_sb->module_map.erase(it);
        }
    }

public:
    obj_ptr<SandBox> m_sb;
    Isolate* m_isolate;

    std::vector<SandBox::module_map_iter> module_refs;
    v8::Global<v8::Promise::Resolver> m_resolver;
};

void SandBox::ImportMetaObjectCallback(v8::Local<v8::Context> context, v8::Local<v8::Module> module,
    v8::Local<v8::Object> meta)
{
    Isolate* isolate = Isolate::current(context);
    Runtime* rt = Runtime::current();

    SandBox* sb = rt->m_module_pending;
    if (sb == nullptr)
        return;

    exlib::string path_name;
    path_base::dirname(sb->m_pending_module, path_name);

    meta->Set(context, isolate->NewString("dirname"), isolate->NewString(path_name)).IsJust();
    meta->Set(context, isolate->NewString("filename"), isolate->NewString(sb->m_pending_module)).IsJust();

    obj_ptr<UrlObject_base> u;
    url_base::pathToFileURL(sb->m_pending_module, v8::Local<v8::Object>(), u);

    exlib::string url;
    u->get_href(url);

    meta->Set(context, isolate->NewString("url"), isolate->NewString(url)).IsJust();
}

v8::MaybeLocal<v8::Promise> SandBox::async_import(exlib::string id, exlib::string base)
{
    Isolate* isolate = holder();
    obj_ptr<esm_importer> importer = new esm_importer(this);
    return importer->async_import(id, base);
}

v8::MaybeLocal<v8::Promise> SandBox::ImportModuleDynamically(v8::Local<v8::Context> context,
    v8::Local<v8::Data> host_defined_options, v8::Local<v8::Value> resource_name,
    v8::Local<v8::String> specifier, v8::Local<v8::FixedArray> import_assertions)
{
    Isolate* isolate = Isolate::current(context);
    v8::Local<v8::PrimitiveArray> _host_options = host_defined_options.As<v8::PrimitiveArray>();
    uint32_t id = _host_options->Get(isolate->m_isolate, 0)->Uint32Value(context).FromMaybe(0);

    exlib::string base = ToString(isolate->m_isolate, resource_name);
    exlib::string pname;
    path_base::dirname(base, pname);

    return isolate->m_sandboxes.find(id)->second->async_import(ToString(isolate->m_isolate, specifier), pname);
}

result_t mjs_Loader::run(SandBox::Context* ctx, Buffer_base* src, exlib::string name,
    exlib::string arg_names, std::vector<v8::Local<v8::Value>>& args, bool in_cjs)
{
    if (!in_cjs)
        return Runtime::setError("SandBox: ECMAScript modules are not supported in require; please use import instead.");

    obj_ptr<esm_importer> importer = new esm_importer(ctx->m_sb);
    return importer->require(name, src, args[2].As<v8::Object>());
}
}