/**
 * @author Richard
 * @email richardo2016@mail.com
 * @create date 2018-08-11 04:35:51
 * @modify date 2018-08-11 04:35:51
 * @desc CustomExtLoader for add loader to Sandbox dynamiclly.
*/

#include "Buffer.h"
#include "SandBox.h"
#include "loaders.h"
#include "object.h"

namespace fibjs {

result_t CustomExtLoader::compile(SandBox::Context* ctx, Buffer_base* src, exlib::string name,
    exlib::string arg_names, v8::Local<v8::Script>& script)
{
    Isolate* isolate = ctx->m_sb->holder();
    v8::Local<v8::Context> context = isolate->context();
    v8::Local<v8::Function> require_fn = v8::Local<v8::Function>::Cast(ctx->m_sb->GetPrivate(SandBox::_get_extloader_pname(m_ext)));

    exlib::string strScript;
    // read filecontent and compile to strScript :start
    v8::Local<v8::Value> transpileArgs[2];

    src->valueOf(transpileArgs[0]);
    v8::Local<v8::Object> requireInfo = v8::Object::New(isolate->m_isolate);
    transpileArgs[1] = requireInfo;

    requireInfo->Set(context, isolate->NewString("filename"), isolate->NewString(name)).IsJust();;

    v8::Local<v8::Value> fileContent = require_fn->Call(context, v8::Undefined(isolate->m_isolate), 2, transpileArgs).FromMaybe(v8::Local<v8::Value>());
    if (fileContent.IsEmpty())
        return CALL_E_JAVASCRIPT;

    result_t hr = GetArgumentValue(fileContent, strScript, true);
    if (hr < 0)
        return CHECK_ERROR(hr);
    // read filecontent and compile to strScript :end

    obj_ptr<Buffer_base> buf = new Buffer(strScript);

    return JsLoader::compile(ctx, buf, name, arg_names, script);
}
} // namespace fibjs
