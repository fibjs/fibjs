/*
 * base_loader.cpp
 *
 *  Created on: Jun 4, 2017
 *      Author: lion
 */

#include "object.h"
#include "SandBox.h"
#include "ifs/global.h"

namespace fibjs {

const char* SandBox::script_args = "(function(__filename,__dirname,require,run,argv){";
const int32_t SandBox::script_args_count = 5;

const char* SandBox::main_args = "(function(__filename,__dirname,require,run,argv,repl){";
const int32_t SandBox::main_args_count = 6;

const char* SandBox::worker_args = "(function(__filename,__dirname,require,run,Master){";
const int32_t SandBox::worker_args_count = 5;

const char* SandBox::module_args = "(function(__filename,__dirname,require,run,exports,module){";
const int32_t SandBox::module_args_count = 6;

result_t SandBox::ExtLoader::run_script(Context* ctx, Buffer_base* src,
    exlib::string name, v8::Local<v8::Array> argv)
{
    v8::Local<v8::Value> args[10] = {
        v8::Local<v8::Value>(), v8::Local<v8::Value>(),
        ctx->m_fnRequest,
        ctx->m_fnRun,
        argv
    };

    return run(ctx, src, name, script_args, args, script_args_count);
}

result_t SandBox::ExtLoader::run_main(Context* ctx, Buffer_base* src,
    exlib::string name, v8::Local<v8::Array> argv)
{
    Isolate* isolate = ctx->m_sb->holder();

    v8::Local<v8::Value> replFunc = global_base::class_info().getModule(isolate)->Get(
        isolate->NewFromUtf8("repl"));

    v8::Local<v8::Value> args[10] = {
        v8::Local<v8::Value>(), v8::Local<v8::Value>(),
        ctx->m_fnRequest,
        ctx->m_fnRun,
        argv,
        replFunc
    };
    return run(ctx, src, name, main_args, args, main_args_count);
}

result_t SandBox::ExtLoader::run_worker(Context* ctx, Buffer_base* src,
    exlib::string name, Worker_base* master)
{
    v8::Local<v8::Value> args[10] = {
        v8::Local<v8::Value>(), v8::Local<v8::Value>(),
        ctx->m_fnRequest,
        ctx->m_fnRun,
        master->wrap()
    };
    return run(ctx, src, name, worker_args, args, worker_args_count);
}

result_t SandBox::ExtLoader::run_module(Context* ctx, Buffer_base* src,
    exlib::string name, v8::Local<v8::Object> module, v8::Local<v8::Object> exports)
{
    v8::Local<v8::Value> args[10] = {
        v8::Local<v8::Value>(), v8::Local<v8::Value>(),
        ctx->m_fnRequest,
        ctx->m_fnRun,
        exports,
        module
    };

    return run(ctx, src, name, module_args, args, module_args_count);
}
}
