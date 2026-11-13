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
#include <boost/preprocessor.hpp>
#include "ifs/SeekableStream.h"

namespace fibjs {

class SeekableStream_base;
class Options_FileStream_FileStream_options;

class FileStream_base : public SeekableStream_base {
    DECLARE_CLASS(FileStream_base);

public:
    // FileStream_base
    static result_t _new(exlib::string path, Options_FileStream_FileStream_options* options, obj_ptr<FileStream_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    virtual result_t get_name(exlib::string& retVal) = 0;
    virtual result_t chmod(int32_t mode, AsyncHandle ac) = 0;

public:
    static void __new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static result_t load(v8::Local<v8::Value> v, obj_ptr<FileStream_base>& retVal);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_name(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_chmod(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
    ASYNC_MEMBER1(FileStream_base, chmod, int32_t);
};
}

namespace fibjs {
class Options_FileStream_FileStream_options : public obj_base {
public:
    LOAD_OPTIONS(Options_FileStream_FileStream_options, (flags)(start)(end));

public:
    std::optional<exlib::string> flags = "r";
    std::optional<int64_t> start = -1LL;
    std::optional<int64_t> end = -1LL;
};

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

inline void FileStream_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    CONSTRUCT_INIT();
    __new(args);
}

inline void FileStream_base::__new(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<FileStream_base> vr;

    CONSTRUCT_ENTER();

    METHOD_OVER_NAMED(2, 1, "path");

    ARG_NAMED(exlib::string, 0, "path");
    OPT_ARG_NAMED(obj_ptr<Options_FileStream_FileStream_options>, 1, new Options_FileStream_FileStream_options(), "options");

    hr = _new(v0, (Options_FileStream_FileStream_options*)v1, vr, args.This());

    CONSTRUCT_RETURN();
}

inline result_t FileStream_base::load(v8::Local<v8::Value> v, obj_ptr<FileStream_base>& retVal)
{
    obj_ptr<FileStream_base> vr;

    LOAD_ENTER();

    METHOD_OVER_NAMED(2, 1, "path");

    ARG_NAMED(exlib::string, 0, "path");
    OPT_ARG_NAMED(obj_ptr<Options_FileStream_FileStream_options>, 1, new Options_FileStream_FileStream_options(), "options");

    hr = _new(v0, (Options_FileStream_FileStream_options*)v1, vr, args.This());

    LOAD_RETURN();
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

    METHOD_OVER_NAMED(1, 1, "mode");

    ARG_NAMED(int32_t, 0, "mode");

    if (!cb.IsEmpty())
        hr = pInst->acb_chmod(v0, cb, args);
    else
        hr = pInst->ac_chmod(v0);

    ASYNC_METHOD_VOID();
}
}
