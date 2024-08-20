/*
 * mjs_loader.cpp
 *
 *  Created on: Jun 10, 2023
 *      Author: lion
 */

#include "object.h"
#include "path.h"
#include "SandBox.h"
#include "Fiber.h"
#include "Buffer.h"
#include "loaders.h"
#include "ifs/util.h"
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

public:
    result_t require(exlib::string id, Buffer_base* data, v8::Local<v8::Object> mod)
    {
        v8::Local<v8::Context> _context = m_isolate->context();

        v8::Local<v8::Module> root_module = compile_module(id, (Buffer*)data, v8::Local<v8::Value>());
        if (root_module.IsEmpty())
            return CALL_E_JAVASCRIPT;

        v8::Local<v8::Promise::Resolver> resolver = v8::Promise::Resolver::New(_context).FromMaybe(v8::Local<v8::Promise::Resolver>());
        m_resolver.Reset(m_isolate->m_isolate, resolver);

        v8::Local<v8::Private> strPendding = v8::Private::ForApi(m_isolate->m_isolate, m_isolate->NewString("pendding"));
        mod->SetPrivate(_context, strPendding, resolver->GetPromise()).IsJust();

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

        hr = resove_module(id, base, data, exports);
        if (hr < 0) {
            ThrowResult(hr);
            return v8::MaybeLocal<v8::Promise>();
        }

        if (!IsEmpty(exports)) {
            v8::Local<v8::Promise::Resolver> resolver = v8::Promise::Resolver::New(_context).FromMaybe(v8::Local<v8::Promise::Resolver>());
            resolver->Resolve(_context, exports).IsJust();

            return resolver->GetPromise();
        }

        SandBox::Context context(m_sb, id);

        v8::Local<v8::Module> root_module = compile_module(id, data.As<Buffer>(), exports);
        if (root_module.IsEmpty())
            return v8::MaybeLocal<v8::Promise>();

        v8::Local<v8::String> strExports = m_isolate->NewString("exports");
        v8::Local<v8::String> strModule = m_isolate->NewString(id);
        exports = v8::Object::New(m_isolate->m_isolate);

        v8::Local<v8::Object> mod = v8::Object::New(m_isolate->m_isolate);
        mod->Set(_context, m_isolate->NewString("id"), strModule).IsJust();
        mod->Set(_context, strExports, exports).IsJust();
        mod->Set(_context, m_isolate->NewString("filename"), strModule).IsJust();

        v8::Local<v8::Promise::Resolver> resolver = v8::Promise::Resolver::New(_context).FromMaybe(v8::Local<v8::Promise::Resolver>());
        m_resolver.Reset(m_isolate->m_isolate, resolver);

        v8::Local<v8::Private> strPendding = v8::Private::ForApi(m_isolate->m_isolate, m_isolate->NewString("pendding"));
        mod->SetPrivate(_context, strPendding, resolver->GetPromise()).IsJust();

        v8::Local<v8::Object> mods = m_sb->mods();

        mods->Set(_context, strModule, mod).IsJust();

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
    result_t resove_module(exlib::string& id, exlib::string base, obj_ptr<Buffer_base>& data, v8::Local<v8::Value>& exports)
    {
        result_t hr;

        exlib::string pname;
        path_base::dirname(base, pname);

        v8::Local<v8::Object> mod;
        hr = m_sb->resolve(pname, id, data, mod);
        if (hr == CALL_E_FILE_NOT_FOUND)
            return CHECK_ERROR(Runtime::setError("Cannot find module '" + id + "' imported from " + base));

        if (hr < 0)
            return hr;

        if (IsEmpty(mod)) {
            result_t hr;
            SandBox::ModuleType type;

            hr = m_sb->resolveModuleType(id, type);
            if (hr)
                return hr;

            if (type == SandBox::ModuleType::kCommonJS) {
                hr = m_sb->installScript(id, data, mod);
                if (hr < 0)
                    return hr;
            }
        }

        if (!IsEmpty(mod)) {
            hr = m_sb->wait_module(mod, exports);
            if (hr < 0)
                return hr;

            return 0;
        }

        return 0;
    }

    v8::Local<v8::Module> compile_module(exlib::string id, Buffer* data_, v8::Local<v8::Value> exports)
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
            std::vector<v8::Local<v8::String>> export_names(length + 1);

            export_names[0] = m_isolate->NewString("default");
            for (int i = 0; i < length; ++i)
                export_names[i + 1] = names->Get(_context, i).ToLocalChecked()->ToString(_context).ToLocalChecked();

            module = v8::Module::CreateSyntheticModule(m_isolate->m_isolate, m_isolate->NewString(id),
                export_names, ModuleEvaluationSteps);

            module->InstantiateModule(_context, resolveModuleCallback).IsJust();
            module->Evaluate(_context).FromMaybe(v8::Local<v8::Value>());

            module->SetSyntheticModuleExport(m_isolate->m_isolate, export_names[0], exports).IsJust();
            for (int i = 0; i < length; ++i) {
                v8::Local<v8::String> name = export_names[i + 1];
                v8::Local<v8::Value> value = obj->Get(_context, name).ToLocalChecked();
                module->SetSyntheticModuleExport(m_isolate->m_isolate, name, value).IsJust();
            }
        } else {
            exlib::string exception;

            {
                TryCatch try_catch;

                v8::Local<v8::PrimitiveArray> pargs = v8::PrimitiveArray::New(m_isolate->m_isolate, 1);
                pargs->Set(m_isolate->m_isolate, 0, v8::Number::New(m_isolate->m_isolate, m_sb->m_id));
                v8::ScriptOrigin so_origin(m_isolate->m_isolate, m_isolate->NewString(id), 0, 0, false,
                    -1, v8::Local<v8::Value>(), false, false, true, pargs);

                v8::ScriptCompiler::Source source(m_isolate->NewString((const char*)data_->data(), data_->length()), so_origin);
                module = v8::ScriptCompiler::CompileModule(m_isolate->m_isolate, &source).FromMaybe(v8::Local<v8::Module>());
                if (module.IsEmpty())
                    exception = GetException(try_catch, 0, false, false);
            }

            if (module.IsEmpty()) {
                ThrowError(exception);
                return v8::Local<v8::Module>();
            }
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
            hr = resove_module(id, base, data, exports);
            if (hr < 0)
                return hr;

            v8::Local<v8::Module> dep_module = compile_module(id, data.As<Buffer>(), exports);
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

        Runtime* rt = Runtime::current();
        result_t hr;

        m_sb->m_module_pendings++;
        rt->m_module_pending = m_sb;

        hr = resolveModuleTree(id, root_module);
        if (hr >= 0) {
            v8::Maybe<bool> result = root_module->InstantiateModule(_context, resolveModuleCallback);
            if (!result.FromMaybe(false))
                hr = CALL_E_JAVASCRIPT;
        }

        rt->m_module_pending = nullptr;
        if (--m_sb->m_module_pendings == 0)
            m_sb->module_deps_map.clear();

        if (hr < 0)
            return hr;

        TryCatch try_catch;
        v8::Local<v8::Value> result = root_module->Evaluate(_context).FromMaybe(v8::Local<v8::Value>());
        if (result.IsEmpty()) {
            try_catch.ReThrow();
            return CALL_E_JAVASCRIPT;
        }

        v8::Local<v8::Promise> promise = v8::Local<v8::Promise>::Cast(result);
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
        v8::Local<v8::Object> mod = mods->Get(context, isolate->NewString(root_module->first)).ToLocalChecked().As<v8::Object>();
        if (!IsEmpty(mod)) {
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
        v8::Local<v8::Object> mod = mods->Get(context, isolate->NewString(root_module->first)).ToLocalChecked().As<v8::Object>();
        if (!IsEmpty(mod)) {
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

v8::MaybeLocal<v8::Promise> SandBox::ImportModuleDynamically(v8::Local<v8::Context> context,
    v8::Local<v8::Data> host_defined_options, v8::Local<v8::Value> resource_name,
    v8::Local<v8::String> specifier, v8::Local<v8::FixedArray> import_assertions)
{
    Isolate* isolate = Isolate::current(context);
    v8::Local<v8::PrimitiveArray> _host_options = host_defined_options.As<v8::PrimitiveArray>();
    uint32_t id = _host_options->Get(isolate->m_isolate, 0)->Uint32Value(context).FromMaybe(0);

    obj_ptr<esm_importer> importer = new esm_importer(isolate->m_sandboxes.find(id)->second);
    return importer->async_import(ToString(isolate->m_isolate, specifier), ToString(isolate->m_isolate, resource_name));
}

result_t mjs_Loader::run(SandBox::Context* ctx, Buffer_base* src, exlib::string name,
    exlib::string arg_names, std::vector<v8::Local<v8::Value>>& args)
{
    obj_ptr<esm_importer> importer = new esm_importer(ctx->m_sb);
    return importer->require(name, src, args[2].As<v8::Object>());
}
}