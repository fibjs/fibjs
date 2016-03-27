/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _zip_base_H_
#define _zip_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs
{

class ZipFile_base;
class Buffer_base;
class SeekableStream_base;

class zip_base : public object_base
{
    DECLARE_CLASS(zip_base);

public:
    // zip_base
    static result_t open(const char* path, obj_ptr<ZipFile_base>& retVal, AsyncEvent* ac);
    static result_t open(Buffer_base* data, obj_ptr<ZipFile_base>& retVal, AsyncEvent* ac);
    static result_t open(SeekableStream_base* strm, obj_ptr<ZipFile_base>& retVal, AsyncEvent* ac);
    static result_t create(const char* path, obj_ptr<ZipFile_base>& retVal, AsyncEvent* ac);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        Isolate* isolate = Isolate::current();

        isolate->m_isolate->ThrowException(
            isolate->NewFromUtf8("not a constructor"));
    }

public:
    static void s_open(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_create(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
    ASYNC_STATICVALUE2(zip_base, open, const char*, obj_ptr<ZipFile_base>);
    ASYNC_STATICVALUE2(zip_base, open, Buffer_base*, obj_ptr<ZipFile_base>);
    ASYNC_STATICVALUE2(zip_base, open, SeekableStream_base*, obj_ptr<ZipFile_base>);
    ASYNC_STATICVALUE2(zip_base, create, const char*, obj_ptr<ZipFile_base>);
};

}

#include "ZipFile.h"
#include "Buffer.h"
#include "SeekableStream.h"

namespace fibjs
{
    inline ClassInfo& zip_base::class_info()
    {
        static ClassData::ClassMethod s_method[] = 
        {
            {"open", s_open, true},
            {"create", s_create, true}
        };

        static ClassData s_cd = 
        { 
            "zip", s__new, NULL, 
            2, s_method, 0, NULL, 0, NULL, NULL, NULL,
            NULL
        };

        static ClassInfo s_ci(s_cd);
        return s_ci;
    }


    inline void zip_base::s_open(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        obj_ptr<ZipFile_base> vr;

        METHOD_ENTER(1, 1);

        ARG(arg_string, 0);

        hr = ac_open(v0, vr);

        METHOD_OVER(1, 1);

        ARG(obj_ptr<Buffer_base>, 0);

        hr = ac_open(v0, vr);

        METHOD_OVER(1, 1);

        ARG(obj_ptr<SeekableStream_base>, 0);

        hr = ac_open(v0, vr);

        METHOD_RETURN();
    }

    inline void zip_base::s_create(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        obj_ptr<ZipFile_base> vr;

        METHOD_ENTER(1, 0);

        OPT_ARG(arg_string, 0, "");

        hr = ac_create(v0, vr);

        METHOD_RETURN();
    }

}

#endif

