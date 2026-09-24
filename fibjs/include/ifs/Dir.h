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
#include "ifs/Iterator.h"

namespace fibjs {

class Iterator_base;
class DirEntry_base;

class Dir_base : public Iterator_base {
    DECLARE_CLASS(Dir_base);

public:
    // Dir_base
    static result_t _new(exlib::string path, obj_ptr<Dir_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    virtual result_t get_path(exlib::string& retVal) = 0;
    virtual result_t read(obj_ptr<DirEntry_base>& retVal, AsyncEvent* ac) = 0;
    virtual result_t close(AsyncEvent* ac) = 0;

public:
    static void __new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static result_t load(v8::Local<v8::Value> v, obj_ptr<Dir_base>& retVal);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_path(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_read(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_close(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
    ASYNC_MEMBERVALUE1(Dir_base, read, obj_ptr<DirEntry_base>);
    ASYNC_MEMBER0(Dir_base, close);
};
}

#include "ifs/DirEntry.h"

namespace fibjs {
inline ClassInfo& Dir_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "read", s_read, false, ClassData::ASYNC_ASYNC },
        { "close", s_close, false, ClassData::ASYNC_ASYNC }
    };

    static ClassData::ClassProperty s_property[] = {
        { "path", s_get_path, block_set, false }
    };

    static ClassData s_cd = {
        "Dir", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &Iterator_base::class_info(),
        true
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void Dir_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    CONSTRUCT_INIT();
    __new(args);
}

inline void Dir_base::__new(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Dir_base> vr;

    CONSTRUCT_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = _new(v0, vr, args.This());

    CONSTRUCT_RETURN();
}

inline result_t Dir_base::load(v8::Local<v8::Value> v, obj_ptr<Dir_base>& retVal)
{
    obj_ptr<Dir_base> vr;

    LOAD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = _new(v0, vr, args.This());

    LOAD_RETURN();
}

inline void Dir_base::s_get_path(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(Dir_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_path(vr);

    METHOD_RETURN();
}

inline void Dir_base::s_read(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<DirEntry_base> vr;

    ASYNC_METHOD_INSTANCE(Dir_base);
    ASYNC_METHOD_ENTER("Dir.read");

    METHOD_OVER(0, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_read(cb, args);
    else
        hr = pInst->ac_read(vr);

    ASYNC_METHOD_RETURN();
}

inline void Dir_base::s_close(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ASYNC_METHOD_INSTANCE(Dir_base);
    ASYNC_METHOD_ENTER("Dir.close");

    METHOD_OVER(0, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_close(cb, args);
    else
        hr = pInst->ac_close();

    ASYNC_METHOD_VOID();
}
}
