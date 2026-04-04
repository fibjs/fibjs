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

namespace fibjs {

class Stat_base;
class Buffer_base;

class FileHandle_base : public object_base {
    DECLARE_CLASS(FileHandle_base);

public:
    class ReadType : public NType {
    public:
        virtual void to_value(Isolate* isolate, v8::Local<v8::Object>& retVal)
        {
            v8::Local<v8::Context> context = retVal->GetCreationContextChecked();
            retVal->Set(context, isolate->NewString("bytesRead"), GetReturnValue(isolate, bytesRead)).Check();
            retVal->Set(context, isolate->NewString("buffer"), GetReturnValue(isolate, buffer)).Check();
        }

        virtual void to_args(Isolate* isolate, std::vector<v8::Local<v8::Value>>& args)
        {
            args.push_back(GetReturnValue(isolate, bytesRead));
            args.push_back(GetReturnValue(isolate, buffer));
        }

    public:
        int32_t bytesRead;
        obj_ptr<Buffer_base> buffer;
    };

public:
    // FileHandle_base
    static result_t _new(int32_t fd, obj_ptr<FileHandle_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    virtual result_t get_fd(int32_t& retVal) = 0;
    virtual result_t chmod(int32_t mode, AsyncEvent* ac) = 0;
    virtual result_t stat(obj_ptr<Stat_base>& retVal, AsyncEvent* ac) = 0;
    virtual result_t read(Buffer_base* buffer, int32_t offset, int32_t length, int32_t position, obj_ptr<ReadType>& retVal, AsyncEvent* ac) = 0;
    virtual result_t write(Buffer_base* buffer, int32_t offset, int32_t length, int32_t position, int32_t& retVal, AsyncEvent* ac) = 0;
    virtual result_t write(exlib::string string, int32_t position, exlib::string encoding, int32_t& retVal, AsyncEvent* ac) = 0;
    virtual result_t readFile(exlib::string encoding, Variant& retVal, AsyncEvent* ac) = 0;
    virtual result_t readFile(v8::Local<v8::Object> options, Variant& retVal, AsyncEvent* ac) = 0;
    virtual result_t writeFile(Buffer_base* data, exlib::string opt, int32_t& retVal, AsyncEvent* ac) = 0;
    virtual result_t writeFile(exlib::string data, exlib::string opt, int32_t& retVal, AsyncEvent* ac) = 0;
    virtual result_t writeFile(Buffer_base* data, v8::Local<v8::Object> options, int32_t& retVal, AsyncEvent* ac) = 0;
    virtual result_t writeFile(exlib::string data, v8::Local<v8::Object> options, int32_t& retVal, AsyncEvent* ac) = 0;
    virtual result_t close(AsyncEvent* ac) = 0;

public:
    static void __new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static result_t load(v8::Local<v8::Value> v, obj_ptr<FileHandle_base>& retVal);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_fd(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_chmod(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_stat(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_read(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_write(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_readFile(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_writeFile(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_close(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
    ASYNC_MEMBER1(FileHandle_base, chmod, int32_t);
    ASYNC_MEMBERVALUE1(FileHandle_base, stat, obj_ptr<Stat_base>);
    ASYNC_MEMBERVALUE5(FileHandle_base, read, Buffer_base*, int32_t, int32_t, int32_t, obj_ptr<ReadType>);
    ASYNC_MEMBERVALUE5(FileHandle_base, write, Buffer_base*, int32_t, int32_t, int32_t, int32_t);
    ASYNC_MEMBERVALUE4(FileHandle_base, write, exlib::string, int32_t, exlib::string, int32_t);
    ASYNC_MEMBERVALUE2(FileHandle_base, readFile, exlib::string, Variant);
    ASYNC_MEMBERVALUE2(FileHandle_base, readFile, v8::Local<v8::Object>, Variant);
    ASYNC_MEMBERVALUE3(FileHandle_base, writeFile, Buffer_base*, exlib::string, int32_t);
    ASYNC_MEMBERVALUE3(FileHandle_base, writeFile, exlib::string, exlib::string, int32_t);
    ASYNC_MEMBERVALUE3(FileHandle_base, writeFile, Buffer_base*, v8::Local<v8::Object>, int32_t);
    ASYNC_MEMBERVALUE3(FileHandle_base, writeFile, exlib::string, v8::Local<v8::Object>, int32_t);
    ASYNC_MEMBER0(FileHandle_base, close);
};
}

#include "ifs/Stat.h"
#include "ifs/Buffer.h"

namespace fibjs {
inline ClassInfo& FileHandle_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "chmod", s_chmod, false, ClassData::ASYNC_ASYNC },
        { "stat", s_stat, false, ClassData::ASYNC_ASYNC },
        { "read", s_read, false, ClassData::ASYNC_ASYNC },
        { "write", s_write, false, ClassData::ASYNC_ASYNC },
        { "readFile", s_readFile, false, ClassData::ASYNC_ASYNC },
        { "writeFile", s_writeFile, false, ClassData::ASYNC_ASYNC },
        { "close", s_close, false, ClassData::ASYNC_ASYNC }
    };

    static ClassData::ClassProperty s_property[] = {
        { "fd", s_get_fd, block_set, false }
    };

    static ClassData s_cd = {
        "FileHandle", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &object_base::class_info(),
        true
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void FileHandle_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    CONSTRUCT_INIT();
    __new(args);
}

inline void FileHandle_base::__new(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<FileHandle_base> vr;

    CONSTRUCT_ENTER();

    METHOD_OVER(1, 1);

    ARG(int32_t, 0);

    hr = _new(v0, vr, args.This());

    CONSTRUCT_RETURN();
}

inline result_t FileHandle_base::load(v8::Local<v8::Value> v, obj_ptr<FileHandle_base>& retVal)
{
    obj_ptr<FileHandle_base> vr;

    LOAD_ENTER();

    METHOD_OVER(1, 1);

    ARG(int32_t, 0);

    hr = _new(v0, vr, args.This());

    LOAD_RETURN();
}

inline void FileHandle_base::s_get_fd(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(FileHandle_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_fd(vr);

    METHOD_RETURN();
}

inline void FileHandle_base::s_chmod(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ASYNC_METHOD_INSTANCE(FileHandle_base);
    ASYNC_METHOD_ENTER("FileHandle.chmod");

    METHOD_OVER(1, 1);

    ARG(int32_t, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_chmod(v0, cb, args);
    else
        hr = pInst->ac_chmod(v0);

    ASYNC_METHOD_VOID();
}

inline void FileHandle_base::s_stat(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Stat_base> vr;

    ASYNC_METHOD_INSTANCE(FileHandle_base);
    ASYNC_METHOD_ENTER("FileHandle.stat");

    METHOD_OVER(0, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_stat(cb, args);
    else
        hr = pInst->ac_stat(vr);

    ASYNC_METHOD_RETURN();
}

inline void FileHandle_base::s_read(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<ReadType> vr;

    ASYNC_METHOD_INSTANCE(FileHandle_base);
    ASYNC_METHOD_ENTER("FileHandle.read");

    METHOD_OVER(4, 1);

    ARG(obj_ptr<Buffer_base>, 0);
    OPT_ARG(int32_t, 1, 0);
    OPT_ARG(int32_t, 2, 0);
    OPT_ARG(int32_t, 3, -1);

    if (!cb.IsEmpty())
        hr = pInst->acb_read(v0.get(), v1, v2, v3, cb, args);
    else
        hr = pInst->ac_read(v0.get(), v1, v2, v3, vr);

    ASYNC_METHOD_RETURN();
}

inline void FileHandle_base::s_write(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    ASYNC_METHOD_INSTANCE(FileHandle_base);
    ASYNC_METHOD_ENTER("FileHandle.write");

    METHOD_OVER(4, 1);

    ARG(obj_ptr<Buffer_base>, 0);
    OPT_ARG(int32_t, 1, 0);
    OPT_ARG(int32_t, 2, -1);
    OPT_ARG(int32_t, 3, -1);

    if (!cb.IsEmpty())
        hr = pInst->acb_write(v0.get(), v1, v2, v3, cb, args);
    else
        hr = pInst->ac_write(v0.get(), v1, v2, v3, vr);

    METHOD_OVER(3, 1);

    ARG(exlib::string, 0);
    OPT_ARG(int32_t, 1, -1);
    OPT_ARG(exlib::string, 2, "utf8");

    if (!cb.IsEmpty())
        hr = pInst->acb_write(v0, v1, v2, cb, args);
    else
        hr = pInst->ac_write(v0, v1, v2, vr);

    ASYNC_METHOD_RETURN();
}

inline void FileHandle_base::s_readFile(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    Variant vr;

    ASYNC_METHOD_INSTANCE(FileHandle_base);
    ASYNC_METHOD_ENTER("FileHandle.readFile");

    METHOD_OVER(1, 0);

    OPT_ARG(exlib::string, 0, "");

    if (!cb.IsEmpty())
        hr = pInst->acb_readFile(v0, cb, args);
    else
        hr = pInst->ac_readFile(v0, vr);

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Object>, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_readFile(v0, cb, args);
    else
        hr = pInst->ac_readFile(v0, vr);

    ASYNC_METHOD_RETURN();
}

inline void FileHandle_base::s_writeFile(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    ASYNC_METHOD_INSTANCE(FileHandle_base);
    ASYNC_METHOD_ENTER("FileHandle.writeFile");

    METHOD_OVER(2, 1);

    ARG(obj_ptr<Buffer_base>, 0);
    OPT_ARG(exlib::string, 1, "binary");

    if (!cb.IsEmpty())
        hr = pInst->acb_writeFile(v0.get(), v1, cb, args);
    else
        hr = pInst->ac_writeFile(v0.get(), v1, vr);

    METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(exlib::string, 1, "utf8");

    if (!cb.IsEmpty())
        hr = pInst->acb_writeFile(v0, v1, cb, args);
    else
        hr = pInst->ac_writeFile(v0, v1, vr);

    METHOD_OVER(2, 2);

    ARG(obj_ptr<Buffer_base>, 0);
    ARG(v8::Local<v8::Object>, 1);

    if (!cb.IsEmpty())
        hr = pInst->acb_writeFile(v0.get(), v1, cb, args);
    else
        hr = pInst->ac_writeFile(v0.get(), v1, vr);

    METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(v8::Local<v8::Object>, 1);

    if (!cb.IsEmpty())
        hr = pInst->acb_writeFile(v0, v1, cb, args);
    else
        hr = pInst->ac_writeFile(v0, v1, vr);

    ASYNC_METHOD_RETURN();
}

inline void FileHandle_base::s_close(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ASYNC_METHOD_INSTANCE(FileHandle_base);
    ASYNC_METHOD_ENTER("FileHandle.close");

    METHOD_OVER(0, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_close(cb, args);
    else
        hr = pInst->ac_close();

    ASYNC_METHOD_VOID();
}
}
