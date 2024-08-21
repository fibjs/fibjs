/*
 * SandBox.h
 *
 *  Created on: Oct 22, 2012
 *      Author: lion
 */

#pragma once

#include "ifs/SandBox.h"
#include "ifs/Stream.h"
#include "ifs/process.h"
#include "ifs/Worker.h"
#include "Lock.h"
#include <unordered_map>
#include <vector>

namespace fibjs {

class SandBox : public SandBox_base {
public:
    SandBox(bool extLoader = true);
    ~SandBox();

public:
    // SandBox_base
    virtual result_t addBuiltinModules();
    virtual result_t add(exlib::string id, v8::Local<v8::Value> mod);
    virtual result_t add(v8::Local<v8::Object> mods);
    virtual result_t addScript(exlib::string srcname, Buffer_base* script, v8::Local<v8::Value>& retVal);
    virtual result_t remove(exlib::string id);
    virtual result_t has(exlib::string id, bool& retVal);
    virtual result_t clone(obj_ptr<SandBox_base>& retVal);
    virtual result_t freeze();
    virtual result_t run(exlib::string fname);
    virtual result_t resolve(exlib::string id, exlib::string base, exlib::string& retVal);
    virtual result_t require(exlib::string id, exlib::string base, v8::Local<v8::Value>& retVal);
    virtual result_t setModuleCompiler(exlib::string extname, v8::Local<v8::Function> compiler);
    virtual result_t get_global(v8::Local<v8::Object>& retVal);
    virtual result_t get_modules(v8::Local<v8::Object>& retVal);

public:
    v8::Local<v8::Object> mods()
    {
        Isolate* isolate = holder();

        v8::Local<v8::Value> v = GetPrivate("_mods");
        v8::Local<v8::Object> o;

        if (!v->IsUndefined())
            o = v8::Local<v8::Object>::Cast(v);
        else {
            o = v8::Object::New(isolate->m_isolate);
            SetPrivate("_mods", o);
        }

        return o;
    }

    v8::Local<v8::Object> InstallModule(exlib::string fname, v8::Local<v8::Value> o, v8::Local<v8::Object> m = v8::Local<v8::Object>())
    {
        Isolate* isolate = holder();
        v8::Local<v8::Context> _context = isolate->context();

        if (m.IsEmpty()) {
            m = v8::Object::New(isolate->m_isolate);
            if (!o.IsEmpty())
                m->Set(_context, isolate->NewString("exports"), o).IsJust();
        }

        mods()->Set(_context, isolate->NewString(fname), m).IsJust();

        return m;
    }

    v8::Local<v8::Object> get_module(v8::Local<v8::Object> mods, exlib::string id);
    result_t wait_module(v8::Local<v8::Object> module, v8::Local<v8::Value>& retVal);

    void installBuffer();
    void attachBuffer();

public:
    class Context {
    public:
        Context(SandBox* sb, exlib::string id);
        result_t repl(exlib::string src);

    public:
        obj_ptr<SandBox> m_sb;
        exlib::string m_id;
        v8::Local<v8::Function> m_fnRequest;
        v8::Local<v8::Function> m_fnRun;
    };

    class Scope {
    public:
        Scope(SandBox* sb)
        {
            if (sb->m_global) {
                v8::Local<v8::Object> _global = v8::Local<v8::Object>::Cast(sb->GetPrivate("_global"));
                _context = _global->GetCreationContextChecked();
                _context->Enter();
            }
        }

        ~Scope()
        {
            if (!_context.IsEmpty())
                _context->Exit();
        }

    private:
        v8::Local<v8::Context> _context;
    };

public:
    class ExtLoader : public obj_base {
    public:
        ExtLoader(const char* ext)
            : m_ext(ext)
        {
        }

    public:
        typedef std::pair<exlib::string, v8::Local<v8::Value>> arg;

    public:
        result_t run_script(Context* ctx, Buffer_base* src, exlib::string name,
            std::vector<arg>& extarg, bool is_main);
        result_t run_module(Context* ctx, Buffer_base* src, exlib::string name,
            v8::Local<v8::Object> module, v8::Local<v8::Object> exports,
            std::vector<arg>& extarg);

    public:
        virtual result_t compile(Context* ctx, Buffer_base* src, exlib::string name,
            exlib::string arg_names, v8::Local<v8::Script>& script)
        {
            return CHECK_ERROR(Runtime::setError("SandBox: Invalid file format."));
        }

        virtual result_t run(Context* ctx, Buffer_base* src, exlib::string name,
            exlib::string arg_names, std::vector<v8::Local<v8::Value>>& args);

    public:
        exlib::string m_ext;
    };

    static exlib::string _get_extloader_pname(exlib::string extname)
    {
        return "extloader_p_" + extname;
    }

public:
    virtual result_t custom_resolveId(exlib::string& id, v8::Local<v8::Object>& retVal);

public:
    void initRequire(v8::Local<v8::Function> func)
    {
        SetPrivate("require", func);
    }

    enum ModuleType {
        kCommonJS = 0,
        kESModule = 1,
    };

    void initGlobal(v8::Local<v8::Object> global);

    result_t installScript(exlib::string srcname, Buffer_base* script, v8::Local<v8::Object>& retVal);

    result_t loadFile(exlib::string fname, obj_ptr<Buffer_base>& data);
    result_t realpath(exlib::string fname, exlib::string& retVal);
    result_t resolveModuleType(exlib::string fname, ModuleType& retVal);

    result_t resolveFile(v8::Local<v8::Object> mods, exlib::string& fname, obj_ptr<Buffer_base>& data,
        v8::Local<v8::Object>* retVal);
    result_t resolvePackage(v8::Local<v8::Object> mods, exlib::string module_name, exlib::string script_name,
        obj_ptr<Buffer_base>& data, ModuleType type, exlib::string& out, v8::Local<v8::Object>* retVal);

    result_t resolveFile(exlib::string module_name, exlib::string script_name, obj_ptr<Buffer_base>& data, ModuleType type,
        exlib::string& out, v8::Local<v8::Object>* retVal);
    result_t resolveId(exlib::string& id, v8::Local<v8::Object>& retVal);
    result_t resolveModule(exlib::string base, exlib::string& id, obj_ptr<Buffer_base>& data, ModuleType type,
        v8::Local<v8::Object>& retVal);
    result_t resolve(exlib::string base, exlib::string& id, obj_ptr<Buffer_base>& data, ModuleType type,
        v8::Local<v8::Object>& retVal);
    result_t resolve(exlib::string base, exlib::string& id, obj_ptr<Buffer_base>& data,
        v8::Local<v8::Object>& retVal);

    result_t repl(exlib::string src);

    result_t run_module(exlib::string id, exlib::string base, v8::Local<v8::Value>& retVal);
    result_t run_main(exlib::string fname, v8::Local<v8::Array> argv);
    result_t run_worker(exlib::string fname, Worker_base* worker);

    result_t get_loader(exlib::string fname, obj_ptr<ExtLoader>& retVal)
    {
        find_loader(fname, retVal);

        if (!retVal)
            retVal = m_loaders[0];

        return 0;
    }

    result_t find_loader(exlib::string fname, obj_ptr<ExtLoader>& retVal)
    {
        size_t cnt = m_loaders.size();

        for (size_t i = 0; i < cnt; i++) {
            obj_ptr<ExtLoader>& l = m_loaders[i];

            if ((fname.length() > l->m_ext.length())
                && !qstrcmp(fname.c_str() + fname.length() - l->m_ext.length(), l->m_ext.c_str())) {
                retVal = l;
                return 0;
            }
        }

        return 0;
    }

    static bool is_relative(exlib::string p)
    {
        const char* c_str = p.c_str();
        if (c_str[0] != '.')
            return false;

        if (c_str[1] == 0 || isPathSlash(c_str[1]))
            return true;

        if (c_str[1] != '.')
            return false;

        if (c_str[2] == 0 || isPathSlash(c_str[2]))
            return true;

        return false;
    }

public:
    static v8::MaybeLocal<v8::Promise> ImportModuleDynamically(v8::Local<v8::Context> context,
        v8::Local<v8::Data> host_defined_options, v8::Local<v8::Value> resource_name,
        v8::Local<v8::String> specifier, v8::Local<v8::FixedArray> import_assertions);

public:
    static const char* worker_args;
    static const char* module_args;
    static const char* base_args;

public:
    int32_t m_module_pendings = 0;
    typedef std::unordered_map<exlib::string, std::pair<v8::Global<v8::Module>, int32_t>>::iterator module_map_iter;
    std::unordered_map<exlib::string, std::pair<v8::Global<v8::Module>, int32_t>> module_map;
    std::unordered_map<int32_t, std::unordered_map<exlib::string, v8::Global<v8::Module>>> module_deps_map;

public:
    std::vector<obj_ptr<ExtLoader>> m_loaders;
    bool m_global;
    uint32_t m_id;
};

} /* namespace fibjs */
