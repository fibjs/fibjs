/*
 * SandBox_context.cpp
 *
 *  Created on: Jun 7, 2017
 *      Author: lion
 */

#include "object.h"
#include "SandBox.h"
#include "path.h"

namespace fibjs {

void _resolve(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t argc = args.Length();

    if (argc < 1) {
        ThrowResult(CALL_E_PARAMNOTOPTIONAL);
        return;
    }

    Isolate* isolate = Isolate::current(args);
    V8_SCOPE(isolate->m_isolate);

    exlib::string id;
    result_t hr = GetArgumentValue(isolate, args[0], id);
    if (hr < 0) {
        ThrowResult(hr);
        return;
    }

    v8::Local<v8::Context> context = isolate->context();
    v8::Local<v8::Object> _mod = args.Data()->ToObject(context).FromMaybe(v8::Local<v8::Object>());
    JSValue path = _mod->Get(context, isolate->NewString("_id"));
    obj_ptr<SandBox> sbox = (SandBox*)SandBox_base::getInstance(
        JSValue(_mod->Get(context, isolate->NewString("_sbox"))));

    exlib::string strPath;
    path_base::dirname(isolate->toString(path), strPath);

    exlib::string v;
    hr = sbox->resolve(id, strPath, v);
    if (hr < 0) {
        if (hr == CALL_E_JAVASCRIPT) {
            args.GetReturnValue().Set(v8::Local<v8::Value>());
            return;
        }
        ThrowResult(hr);
        return;
    }

    args.GetReturnValue().Set(V8_RETURN(sbox->holder()->NewString(v)));
}

void _require(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t argc = args.Length();

    if (argc < 1) {
        ThrowResult(CALL_E_PARAMNOTOPTIONAL);
        return;
    }

    if (argc > 1) {
        ThrowResult(CALL_E_BADPARAMCOUNT);
        return;
    }

    Isolate* isolate = Isolate::current(args);
    V8_SCOPE(isolate->m_isolate);

    exlib::string id;
    result_t hr = GetArgumentValue(isolate, args[0], id);
    if (hr < 0) {
        ThrowResult(hr);
        return;
    }

    v8::Local<v8::Context> context = isolate->context();
    v8::Local<v8::Object> _mod = args.Data()->ToObject(context).FromMaybe(v8::Local<v8::Object>());
    JSValue path = _mod->Get(context, isolate->NewString("_id"));
    obj_ptr<SandBox> sbox = (SandBox*)SandBox_base::getInstance(
        JSValue(_mod->Get(context, isolate->NewString("_sbox"))));

    exlib::string strPath;
    path_base::dirname(isolate->toString(path), strPath);

    v8::Local<v8::Value> v;
    hr = sbox->run_module(id, strPath, v);
    if (hr < 0) {
        if (hr == CALL_E_JAVASCRIPT) {
            args.GetReturnValue().Set(v8::Local<v8::Value>());
            return;
        }
        ThrowResult(hr);
        return;
    }

    args.GetReturnValue().Set(V8_RETURN(v));
}

void _run(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    Isolate* isolate = Isolate::current(args);
    V8_SCOPE(isolate->m_isolate);
    int32_t argc = args.Length();

    if (argc < 1) {
        ThrowResult(CALL_E_PARAMNOTOPTIONAL);
        return;
    }

    if (argc > 1) {
        ThrowResult(CALL_E_BADPARAMCOUNT);
        return;
    }

    exlib::string id;
    result_t hr = GetArgumentValue(isolate, args[0], id);
    if (hr < 0) {
        ThrowResult(hr);
        return;
    }

    v8::Local<v8::Context> context = isolate->context();
    v8::Local<v8::Object> _mod = args.Data()->ToObject(context).FromMaybe(v8::Local<v8::Object>());
    obj_ptr<SandBox> sbox = (SandBox*)SandBox_base::getInstance(
        JSValue(_mod->Get(context, isolate->NewString("_sbox"))));

    if (SandBox::is_relative(id)) {
        JSValue path = _mod->Get(context, isolate->NewString("_id"));

        exlib::string strPath;

        path_base::dirname(isolate->toString(path), strPath);
        if (strPath.length()) {
            resolvePath(strPath, id);
            id = strPath;
        }
    }

    hr = sbox->run(id);
    if (hr < 0) {
        if (hr == CALL_E_JAVASCRIPT) {
            args.GetReturnValue().Set(v8::Local<v8::Value>());
            return;
        }
        ThrowResult(hr);
        return;
    }
}

SandBox::Context::Context(SandBox* sb, exlib::string id)
    : m_sb(sb)
    , m_id(id)
{
    Isolate* isolate = m_sb->holder();
    v8::Local<v8::Context> context = isolate->context();

    v8::Local<v8::Object> _mod = v8::Object::New(isolate->m_isolate);

    _mod->Set(context, isolate->NewString("_sbox"), m_sb->wrap(isolate)).IsJust();
    _mod->Set(context, isolate->NewString("_id"), isolate->NewString(id)).IsJust();

    m_fnRequest = isolate->NewFunction("require", _require, _mod);
    m_fnRequest->Set(context, isolate->NewString("resolve"), isolate->NewFunction("resolve", _resolve, _mod)).IsJust();
    m_fnRequest->Set(context, isolate->NewString("cache"), m_sb->mods()).IsJust();

    m_fnRun = isolate->NewFunction("run", _run, _mod);
}
}