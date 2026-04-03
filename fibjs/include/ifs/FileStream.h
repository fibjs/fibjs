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
#include "ifs/SeekableStream.h"

namespace fibjs {

class SeekableStream_base;

class FileStream_base : public SeekableStream_base {
    DECLARE_CLASS(FileStream_base);

public:
    // FileStream_base
    virtual result_t get_name(exlib::string& retVal) = 0;
    virtual result_t chmod(int32_t mode, AsyncEvent* ac) = 0;

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        ThrowTypeError("not a constructor");
    }

    static result_t load(v8::Local<v8::Value> v, obj_ptr<FileStream_base>& retVal)
    { return CALL_E_TYPEMISMATCH; }

public:
    static void s_get_name(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_chmod(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
    ASYNC_MEMBER1(FileStream_base, chmod, int32_t);
};
}

namespace fibjs {
inline ClassInfo& FileStream_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "chmod", s_chmod, false, ClassData::ASYNC_ASYNC }
    };

    static ClassData::ClassProperty s_property[] = {
        { "name", s_get_name, block_set, false }
    };

    static ClassData s_cd = {
        "FileStream", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &SeekableStream_base::class_info(),
        true
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void FileStream_base::s_get_name(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(FileStream_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_name(vr);

    METHOD_RETURN();
}

inline void FileStream_base::s_chmod(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ASYNC_METHOD_INSTANCE(FileStream_base);
    ASYNC_METHOD_ENTER("FileStream.chmod");

    METHOD_OVER(1, 1);

    ARG(int32_t, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_chmod(v0, cb, args);
    else
        hr = pInst->ac_chmod(v0);

    ASYNC_METHOD_VOID();
}
}
