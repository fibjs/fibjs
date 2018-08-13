/*
 * loaders.h
 *
 *  Created on: Jun 3, 2017
 *      Author: lion
 */

#include "SandBox.h"

#ifndef LOADERS_H_
#define LOADERS_H_

namespace fibjs {

class JsLoader : public SandBox::ExtLoader {
public:
    JsLoader()
        : ExtLoader(".js")
    {
    }

public:
    virtual result_t run(SandBox::Context* ctx, Buffer_base* src, exlib::string name,
        exlib::string arg_names, v8::Local<v8::Value>* args, int32_t args_count);
};

class JscLoader : public SandBox::ExtLoader {
public:
    JscLoader()
        : ExtLoader(".jsc")
    {
    }

public:
    virtual result_t run(SandBox::Context* ctx, Buffer_base* src, exlib::string name,
        exlib::string arg_names, v8::Local<v8::Value>* args, int32_t args_count);
};

class JsonLoader : public SandBox::ExtLoader {
public:
    JsonLoader()
        : ExtLoader(".json")
    {
    }

public:
    virtual result_t run(SandBox::Context* ctx, Buffer_base* src, exlib::string name,
        exlib::string arg_names, v8::Local<v8::Value>* args, int32_t args_count);
};

} /* namespace fibjs */
#endif /* LOADERS_H_ */
