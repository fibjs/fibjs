/*
 * loaders.h
 *
 *  Created on: Jun 3, 2017
 *      Author: lion
 */

#pragma once

#include "SandBox.h"

namespace fibjs {

class JsLoader : public SandBox::ExtLoader {
public:
    JsLoader()
        : ExtLoader(".js")
    {
    }

public:
    virtual result_t compile(SandBox::Context* ctx, Buffer_base* src, exlib::string name,
        exlib::string arg_names, v8::Local<v8::Script>& script);
};

class JscLoader : public SandBox::ExtLoader {
public:
    JscLoader()
        : ExtLoader(".jsc")
    {
    }

public:
    virtual result_t compile(SandBox::Context* ctx, Buffer_base* src, exlib::string name,
        exlib::string arg_names, v8::Local<v8::Script>& script);
};

class JsonLoader : public SandBox::ExtLoader {
public:
    JsonLoader()
        : ExtLoader(".json")
    {
    }

public:
    virtual result_t run(SandBox::Context* ctx, Buffer_base* src, exlib::string name,
        exlib::string arg_names, std::vector<v8::Local<v8::Value>>& args);
};

class CustomExtLoader : public JsLoader {
public:
    CustomExtLoader(exlib::string extname)
        : JsLoader()
    {
        m_ext = extname;
    }

public:
    virtual result_t compile(SandBox::Context* ctx, Buffer_base* src, exlib::string name,
        exlib::string arg_names, v8::Local<v8::Script>& script);
};

} /* namespace fibjs */
