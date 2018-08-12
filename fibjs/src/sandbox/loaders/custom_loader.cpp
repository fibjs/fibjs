/**
 * @author Richard
 * @email richardo2016@mail.com
 * @create date 2018-08-11 04:35:51
 * @modify date 2018-08-11 04:35:51
 * @desc CustomExtLoader for add loader to Sandbox dynamiclly.
*/

#include "Buffer.h"
#include "SandBox.h"
#include "ifs/util.h"
#include "loaders.h"
#include "object.h"
#include "path.h"

namespace fibjs {

result_t SandBox::CustomExtLoader::run(SandBox::Context* ctx, Buffer_base* src,
    exlib::string name, exlib::string arg_names,
    v8::Local<v8::Value>* args,
    int32_t args_count)
{
    Isolate* isolate = ctx->m_sb->holder();

    v8::Local<v8::String> soname = isolate->NewString(name);
    exlib::string pname;
    path_base::dirname(name, pname);

    v8::Local<v8::Value> require_fn = ctx->m_sb->GetPrivate(SandBox::_get_extloader_pname(m_ext));

    // transpile with customFile first :start
    std::vector<v8::Local<v8::Value>> transpileArgs;
    transpileArgs.resize(1);

    src->valueOf(transpileArgs[0]);

    TryCatch try_catch;
    v8::Local<v8::Value> fileContent = v8::Local<v8::Function>::Cast(require_fn)
                                           ->Call(v8::Undefined(isolate->m_isolate),
                                               (int32_t)transpileArgs.size(), transpileArgs.data());
    if (try_catch.HasCaught()) {
        v8::Local<v8::StackTrace> stackTrace = v8::StackTrace::CurrentStackTrace(
            isolate->m_isolate, 1, v8::StackTrace::kScriptId);
        if (stackTrace->GetFrameCount() > 0) {
            try_catch.ReThrow();
            return CALL_E_JAVASCRIPT;
        } else
            return CHECK_ERROR(Runtime::setError(GetException(try_catch, 0)));
    }
    // transpile with customFile first :end

    v8::Local<v8::Object> module = v8::Local<v8::Object>::Cast(args[5]);
    module->Set(isolate->NewString("exports"), fileContent);

    return 0;
};
} // namespace fibjs
