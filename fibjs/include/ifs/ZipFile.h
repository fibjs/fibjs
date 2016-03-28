/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _ZipFile_base_H_
#define _ZipFile_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs
{

class List_base;
class ZipInfo_base;
class Buffer_base;
class SeekableStream_base;

class ZipFile_base : public object_base
{
    DECLARE_CLASS(ZipFile_base);

public:
    // ZipFile_base
    virtual result_t list(obj_ptr<List_base>& retVal, AsyncEvent* ac) = 0;
    virtual result_t get(const char* member, obj_ptr<ZipInfo_base>& retVal, AsyncEvent* ac) = 0;
    virtual result_t read(const char* member, const char* password, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac) = 0;
    virtual result_t extract(const char* member, const char* path, const char* password, AsyncEvent* ac) = 0;
    virtual result_t extract(const char* member, SeekableStream_base* strm, const char* password, AsyncEvent* ac) = 0;
    virtual result_t extractAll(const char* password, obj_ptr<List_base>& retVal, AsyncEvent* ac) = 0;

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        Isolate* isolate = Isolate::current();

        isolate->m_isolate->ThrowException(
            isolate->NewFromUtf8("not a constructor"));
    }

public:
    static void s_list(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_read(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_extract(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_extractAll(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
    ASYNC_MEMBERVALUE1(ZipFile_base, list, obj_ptr<List_base>);
    ASYNC_MEMBERVALUE2(ZipFile_base, get, const char*, obj_ptr<ZipInfo_base>);
    ASYNC_MEMBERVALUE3(ZipFile_base, read, const char*, const char*, obj_ptr<Buffer_base>);
    ASYNC_MEMBER3(ZipFile_base, extract, const char*, const char*, const char*);
    ASYNC_MEMBER3(ZipFile_base, extract, const char*, SeekableStream_base*, const char*);
    ASYNC_MEMBERVALUE2(ZipFile_base, extractAll, const char*, obj_ptr<List_base>);
};

}

#include "List.h"
#include "ZipInfo.h"
#include "Buffer.h"
#include "SeekableStream.h"

namespace fibjs
{
    inline ClassInfo& ZipFile_base::class_info()
    {
        static ClassData::ClassMethod s_method[] = 
        {
            {"list", s_list, false},
            {"get", s_get, false},
            {"read", s_read, false},
            {"extract", s_extract, false},
            {"extractAll", s_extractAll, false}
        };

        static ClassData s_cd = 
        { 
            "ZipFile", s__new, NULL, 
            5, s_method, 0, NULL, 0, NULL, NULL, NULL,
            &object_base::class_info()
        };

        static ClassInfo s_ci(s_cd);
        return s_ci;
    }


    inline void ZipFile_base::s_list(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        obj_ptr<List_base> vr;

        METHOD_INSTANCE(ZipFile_base);
        METHOD_ENTER(0, 0);

        hr = pInst->ac_list(vr);

        METHOD_RETURN();
    }

    inline void ZipFile_base::s_get(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        obj_ptr<ZipInfo_base> vr;

        METHOD_INSTANCE(ZipFile_base);
        METHOD_ENTER(1, 1);

        ARG(arg_string, 0);

        hr = pInst->ac_get(v0, vr);

        METHOD_RETURN();
    }

    inline void ZipFile_base::s_read(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        obj_ptr<Buffer_base> vr;

        METHOD_INSTANCE(ZipFile_base);
        METHOD_ENTER(2, 1);

        ARG(arg_string, 0);
        OPT_ARG(arg_string, 1, "");

        hr = pInst->ac_read(v0, v1, vr);

        METHOD_RETURN();
    }

    inline void ZipFile_base::s_extract(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        METHOD_INSTANCE(ZipFile_base);
        METHOD_ENTER(3, 2);

        ARG(arg_string, 0);
        ARG(arg_string, 1);
        OPT_ARG(arg_string, 2, "");

        hr = pInst->ac_extract(v0, v1, v2);

        METHOD_OVER(3, 2);

        ARG(arg_string, 0);
        ARG(obj_ptr<SeekableStream_base>, 1);
        OPT_ARG(arg_string, 2, "");

        hr = pInst->ac_extract(v0, v1, v2);

        METHOD_VOID();
    }

    inline void ZipFile_base::s_extractAll(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        obj_ptr<List_base> vr;

        METHOD_INSTANCE(ZipFile_base);
        METHOD_ENTER(1, 0);

        OPT_ARG(arg_string, 0, "");

        hr = pInst->ac_extractAll(v0, vr);

        METHOD_RETURN();
    }

}

#endif

