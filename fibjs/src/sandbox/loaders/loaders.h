/*
 * loaders.h
 *
 *  Created on: Jun 3, 2017
 *      Author: lion
 */

#pragma once

#include "SandBox.h"

namespace fibjs {

// Whether a failed CommonJS compilation should be retried as an ES module.
//
// The error message settles the unambiguous cases; the ambiguous ones are
// settled by compiling the source as a module. `typescript` selects whether
// that probe runs on the source as it is or on its stripped form - V8 cannot
// compile TypeScript, so the raw source would fail the probe for the wrong
// reason. Implemented in esm_detect.cpp; used by both the .js and .ts loaders.
bool shouldRetryAsESM(exlib::string err_msg, Buffer_base* src, exlib::string name, bool typescript);

class cjs_Loader : public SandBox::ExtLoader {
public:
    cjs_Loader(const char* ext = ".cjs")
        : ExtLoader(ext)
    {
    }

public:
    virtual result_t compile(SandBox::Context* ctx, Buffer_base* src, exlib::string name,
        exlib::string arg_names, v8::Local<v8::Script>& script);
};

class mjs_Loader : public SandBox::ExtLoader {
public:
    mjs_Loader(const char* ext = ".mjs")
        : ExtLoader(ext)
    {
    }

public:
    virtual result_t run(SandBox::Context* ctx, Buffer_base* src, exlib::string name,
        exlib::string arg_names, std::vector<v8::Local<v8::Value>>& args, bool in_cjs);
};

class js_Loader : public SandBox::ExtLoader {
public:
    js_Loader(const char* ext = ".js")
        : ExtLoader(ext)
    {
    }

public:
    virtual result_t run(SandBox::Context* ctx, Buffer_base* src, exlib::string name,
        exlib::string arg_names, std::vector<v8::Local<v8::Value>>& args, bool in_cjs);

private:
    cjs_Loader m_cjs;
    mjs_Loader m_mjs;
};

class cts_Loader : public cjs_Loader {
public:
    cts_Loader(const char* ext = ".cts")
        : cjs_Loader(ext)
    {
    }

public:
    virtual result_t compile(SandBox::Context* ctx, Buffer_base* src, exlib::string name,
        exlib::string arg_names, v8::Local<v8::Script>& script);

public:
    static result_t ts_compile(Isolate* isolate, Buffer_base* src, obj_ptr<Buffer_base>& retVal);
};

class mts_Loader : public mjs_Loader {
public:
    mts_Loader(const char* ext = ".mts")
        : mjs_Loader(ext)
    {
    }
};

class ts_Loader : public SandBox::ExtLoader {
public:
    ts_Loader(const char* ext = ".ts")
        : ExtLoader(ext)
    {
    }

public:
    virtual result_t run(SandBox::Context* ctx, Buffer_base* src, exlib::string name,
        exlib::string arg_names, std::vector<v8::Local<v8::Value>>& args, bool in_cjs);

private:
    cts_Loader m_cts;
    mts_Loader m_mts;
};

class jsc_Loader : public SandBox::ExtLoader {
public:
    jsc_Loader()
        : ExtLoader(".jsc")
    {
    }

public:
    virtual result_t compile(SandBox::Context* ctx, Buffer_base* src, exlib::string name,
        exlib::string arg_names, v8::Local<v8::Script>& script);
};

class json_Loader : public SandBox::ExtLoader {
public:
    json_Loader()
        : ExtLoader(".json")
    {
    }

public:
    virtual result_t run(SandBox::Context* ctx, Buffer_base* src, exlib::string name,
        exlib::string arg_names, std::vector<v8::Local<v8::Value>>& args, bool in_cjs);
};

class node_Loader : public SandBox::ExtLoader {
public:
    node_Loader()
        : ExtLoader(".node")
    {
    }

public:
    virtual result_t run(SandBox::Context* ctx, Buffer_base* src, exlib::string name,
        exlib::string arg_names, std::vector<v8::Local<v8::Value>>& args, bool in_cjs);
};

class custom_Loader : public cjs_Loader {
public:
    custom_Loader(exlib::string extname)
        : cjs_Loader()
    {
        m_ext = extname;
    }

public:
    virtual result_t compile(SandBox::Context* ctx, Buffer_base* src, exlib::string name,
        exlib::string arg_names, v8::Local<v8::Script>& script);
};

} /* namespace fibjs */
