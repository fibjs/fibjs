/*
 * loaders.h
 *
 *  Created on: Jun 3, 2017
 *      Author: lion
 */

#pragma once

#include "SandBox.h"

namespace fibjs {

class js_Loader : public SandBox::ExtLoader {
public:
    js_Loader()
        : ExtLoader(".js")
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

class custom_Loader : public js_Loader {
public:
    custom_Loader(exlib::string extname)
        : js_Loader()
    {
        m_ext = extname;
    }

public:
    virtual result_t compile(SandBox::Context* ctx, Buffer_base* src, exlib::string name,
        exlib::string arg_names, v8::Local<v8::Script>& script);
};

} /* namespace fibjs */
