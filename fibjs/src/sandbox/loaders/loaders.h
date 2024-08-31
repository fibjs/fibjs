/*
 * loaders.h
 *
 *  Created on: Jun 3, 2017
 *      Author: lion
 */

#pragma once

#include "SandBox.h"

namespace fibjs {

class cjs_Loader : public SandBox::ExtLoader {
public:
    cjs_Loader()
        : ExtLoader(".cjs")
    {
    }

public:
    virtual result_t compile(SandBox::Context* ctx, Buffer_base* src, exlib::string name,
        exlib::string arg_names, v8::Local<v8::Script>& script);
};

class mjs_Loader : public SandBox::ExtLoader {
public:
    mjs_Loader()
        : ExtLoader(".mjs")
    {
    }

public:
    virtual result_t run(SandBox::Context* ctx, Buffer_base* src, exlib::string name,
        exlib::string arg_names, std::vector<v8::Local<v8::Value>>& args);
};

class js_Loader : public SandBox::ExtLoader {
public:
    js_Loader()
        : ExtLoader(".js")
    {
    }

public:
    virtual result_t run(SandBox::Context* ctx, Buffer_base* src, exlib::string name,
        exlib::string arg_names, std::vector<v8::Local<v8::Value>>& args);

private:
    cjs_Loader m_cjs;
    mjs_Loader m_mjs;
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
        exlib::string arg_names, std::vector<v8::Local<v8::Value>>& args);
};

class node_Loader : public SandBox::ExtLoader {
public:
    node_Loader()
        : ExtLoader(".node")
    {
    }

public:
    virtual result_t run(SandBox::Context* ctx, Buffer_base* src, exlib::string name,
        exlib::string arg_names, std::vector<v8::Local<v8::Value>>& args);
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
