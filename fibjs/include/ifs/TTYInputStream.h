/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#pragma once

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "ifs/Stream.h"

namespace fibjs {

class Stream_base;
class FileHandle_base;

class TTYInputStream_base : public Stream_base {
    DECLARE_CLASS(TTYInputStream_base);

public:
    // TTYInputStream_base
    static result_t _new(int32_t fd, v8::Local<v8::Object> opts, obj_ptr<TTYInputStream_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    static result_t _new(FileHandle_base* fd, v8::Local<v8::Object> opts, obj_ptr<TTYInputStream_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    virtual result_t get_isTTY(bool& retVal) = 0;
    virtual result_t get_readable(bool& retVal) = 0;
    virtual result_t get_isRaw(bool& retVal) = 0;
    virtual result_t setRawMode(bool isRawMode, obj_ptr<TTYInputStream_base>& retVal) = 0;

public:
    static void __new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static result_t load(v8::Local<v8::Value> v, obj_ptr<TTYInputStream_base>& retVal);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_isTTY(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_readable(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_isRaw(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_setRawMode(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

#include "ifs/FileHandle.h"

namespace fibjs {
inline ClassInfo& TTYInputStream_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "setRawMode", s_setRawMode, false, ClassData::ASYNC_SYNC }
    };

    static ClassData::ClassProperty s_property[] = {
        { "isTTY", s_get_isTTY, block_set, false },
        { "readable", s_get_readable, block_set, false },
        { "isRaw", s_get_isRaw, block_set, false }
    };

    static ClassData s_cd = {
        "TTYInputStream", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &Stream_base::class_info(),
        true
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void TTYInputStream_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    CONSTRUCT_INIT();
    __new(args);
}

inline void TTYInputStream_base::__new(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<TTYInputStream_base> vr;

    CONSTRUCT_ENTER();

    METHOD_OVER(2, 1);

    ARG(int32_t, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate->m_isolate));

    hr = _new(v0, v1, vr, args.This());

    METHOD_OVER(2, 1);

    ARG(obj_ptr<FileHandle_base>, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate->m_isolate));

    hr = _new(v0.get(), v1, vr, args.This());

    CONSTRUCT_RETURN();
}

inline result_t TTYInputStream_base::load(v8::Local<v8::Value> v, obj_ptr<TTYInputStream_base>& retVal)
{
    obj_ptr<TTYInputStream_base> vr;

    LOAD_ENTER();

    METHOD_OVER(2, 1);

    ARG(int32_t, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate->m_isolate));

    hr = _new(v0, v1, vr, args.This());

    METHOD_OVER(2, 1);

    ARG(obj_ptr<FileHandle_base>, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate->m_isolate));

    hr = _new(v0.get(), v1, vr, args.This());

    LOAD_RETURN();
}

inline void TTYInputStream_base::s_get_isTTY(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_INSTANCE(TTYInputStream_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_isTTY(vr);

    METHOD_RETURN();
}

inline void TTYInputStream_base::s_get_readable(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_INSTANCE(TTYInputStream_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_readable(vr);

    METHOD_RETURN();
}

inline void TTYInputStream_base::s_get_isRaw(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_INSTANCE(TTYInputStream_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_isRaw(vr);

    METHOD_RETURN();
}

inline void TTYInputStream_base::s_setRawMode(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<TTYInputStream_base> vr;

    METHOD_INSTANCE(TTYInputStream_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(bool, 0);

    hr = pInst->setRawMode(v0, vr);

    METHOD_RETURN();
}
}
