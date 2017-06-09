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

void _resovle(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t argc = args.Length();
    if (argc > 1) {
        ThrowResult(CALL_E_BADPARAMCOUNT);
        return;
    }
    if (argc < 1) {
        ThrowResult(CALL_E_PARAMNOTOPTIONAL);
        return;
    }

    v8::Isolate* isolate = args.GetIsolate();
    V8_SCOPE(isolate);

    exlib::string id;
    result_t hr = GetArgumentValue(args[0], id);
    if (hr < 0) {
        ThrowResult(hr);
        return;
    }

    v8::Local<v8::Object> _mod = args.Data()->ToObject();
    v8::Local<v8::Value> path = _mod->Get(v8::String::NewFromUtf8(isolate, "_id"));
    obj_ptr<SandBox> sbox = (SandBox*)SandBox_base::getInstance(
        _mod->Get(v8::String::NewFromUtf8(isolate, "_sbox")));

    exlib::string v;
    hr = sbox->resovle(id, *v8::String::Utf8Value(path), v);
    if (hr < 0) {
        if (hr == CALL_E_JAVASCRIPT) {
            args.GetReturnValue().Set(v8::Local<v8::Value>());
            return;
        }
        ThrowResult(hr);
        return;
    }

    args.GetReturnValue().Set(V8_RETURN(v8::String::NewFromUtf8(isolate, v.c_str(),
        v8::String::kNormalString, (int32_t)v.length())));
}

void _require(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t argc = args.Length();
    if (argc > 1) {
        ThrowResult(CALL_E_BADPARAMCOUNT);
        return;
    }
    if (argc < 1) {
        ThrowResult(CALL_E_PARAMNOTOPTIONAL);
        return;
    }

    v8::Isolate* isolate = args.GetIsolate();
    V8_SCOPE(isolate);

    exlib::string id;
    result_t hr = GetArgumentValue(args[0], id);
    if (hr < 0) {
        ThrowResult(hr);
        return;
    }

    v8::Local<v8::Object> _mod = args.Data()->ToObject();
    v8::Local<v8::Value> path = _mod->Get(v8::String::NewFromUtf8(isolate, "_id"));
    obj_ptr<SandBox> sbox = (SandBox*)SandBox_base::getInstance(
        _mod->Get(v8::String::NewFromUtf8(isolate, "_sbox")));

    v8::Local<v8::Value> v;
    hr = sbox->require(id, *v8::String::Utf8Value(path), v);
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
    v8::Isolate* isolate = args.GetIsolate();
    int32_t argc = args.Length();

    if (argc > 2) {
        ThrowResult(CALL_E_BADPARAMCOUNT);
        return;
    }

    if (argc < 1) {
        ThrowResult(CALL_E_PARAMNOTOPTIONAL);
        return;
    }

    exlib::string id;
    result_t hr = GetArgumentValue(args[0], id);
    if (hr < 0) {
        ThrowResult(hr);
        return;
    }

    v8::Local<v8::Array> argv;
    if (argc == 2) {
        result_t hr = GetArgumentValue(args[1], argv);
        if (hr < 0) {
            ThrowResult(hr);
            return;
        }
    } else
        argv = v8::Array::New(isolate);

    v8::Local<v8::Object> _mod = args.Data()->ToObject();
    obj_ptr<SandBox> sbox = (SandBox*)SandBox_base::getInstance(
        _mod->Get(v8::String::NewFromUtf8(isolate, "_sbox")));

    if (id[0] == '.' && (isPathSlash(id[1]) || (id[1] == '.' && isPathSlash(id[2])))) {
        v8::Local<v8::Value> path = _mod->Get(v8::String::NewFromUtf8(isolate, "_id"));

        exlib::string strPath;

        path_base::dirname(*v8::String::Utf8Value(path), strPath);
        if (strPath.length())
            strPath += PATH_SLASH;
        id = strPath + id;
    }

    hr = sbox->run(id, argv);
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
{
    Isolate* isolate = m_sb->holder();
    m_id = isolate->NewFromUtf8(id);

    v8::Local<v8::Object> _mod = v8::Object::New(isolate->m_isolate);

    _mod->Set(isolate->NewFromUtf8("_sbox"), m_sb->wrap());
    _mod->Set(isolate->NewFromUtf8("_id"), m_id);

    m_fnRequest = isolate->NewFunction("require", _require, _mod);
    m_fnRequest->Set(isolate->NewFromUtf8("resovle"), isolate->NewFunction("resovle", _resovle, _mod));

    m_fnRun = isolate->NewFunction("run", _run, _mod);
}
}