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
    virtual result_t namelist(obj_ptr<List_base>& retVal, AsyncEvent* ac) = 0;
    virtual result_t infolist(obj_ptr<List_base>& retVal, AsyncEvent* ac) = 0;
    virtual result_t getinfo(const char* member, obj_ptr<ZipInfo_base>& retVal, AsyncEvent* ac) = 0;
    virtual result_t read(const char* member, const char* password, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac) = 0;
    virtual result_t readAll(const char* password, obj_ptr<List_base>& retVal, AsyncEvent* ac) = 0;
    virtual result_t extract(const char* member, const char* path, const char* password, AsyncEvent* ac) = 0;
    virtual result_t extract(const char* member, SeekableStream_base* strm, const char* password, AsyncEvent* ac) = 0;
    virtual result_t extractAll(const char* path, const char* password, AsyncEvent* ac) = 0;
    virtual result_t setpasswd(const char* password) = 0;
    virtual result_t write(const char* filename, AsyncEvent* ac) = 0;
    virtual result_t write(Buffer_base* data, AsyncEvent* ac) = 0;
    virtual result_t write(SeekableStream_base* strm, AsyncEvent* ac) = 0;
    virtual result_t close(AsyncEvent* ac) = 0;

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        Isolate* isolate = Isolate::current();

        isolate->m_isolate->ThrowException(
            isolate->NewFromUtf8("not a constructor"));
    }

public:
    static void s_namelist(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_infolist(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_getinfo(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_read(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_readAll(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_extract(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_extractAll(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_setpasswd(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_write(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_close(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
    ASYNC_MEMBERVALUE1(ZipFile_base, namelist, obj_ptr<List_base>);
    ASYNC_MEMBERVALUE1(ZipFile_base, infolist, obj_ptr<List_base>);
    ASYNC_MEMBERVALUE2(ZipFile_base, getinfo, const char*, obj_ptr<ZipInfo_base>);
    ASYNC_MEMBERVALUE3(ZipFile_base, read, const char*, const char*, obj_ptr<Buffer_base>);
    ASYNC_MEMBERVALUE2(ZipFile_base, readAll, const char*, obj_ptr<List_base>);
    ASYNC_MEMBER3(ZipFile_base, extract, const char*, const char*, const char*);
    ASYNC_MEMBER3(ZipFile_base, extract, const char*, SeekableStream_base*, const char*);
    ASYNC_MEMBER2(ZipFile_base, extractAll, const char*, const char*);
    ASYNC_MEMBER1(ZipFile_base, write, const char*);
    ASYNC_MEMBER1(ZipFile_base, write, Buffer_base*);
    ASYNC_MEMBER1(ZipFile_base, write, SeekableStream_base*);
    ASYNC_MEMBER0(ZipFile_base, close);
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
            {"namelist", s_namelist, false},
            {"infolist", s_infolist, false},
            {"getinfo", s_getinfo, false},
            {"read", s_read, false},
            {"readAll", s_readAll, false},
            {"extract", s_extract, false},
            {"extractAll", s_extractAll, false},
            {"setpasswd", s_setpasswd, false},
            {"write", s_write, false},
            {"close", s_close, false}
        };

        static ClassData s_cd = 
        { 
            "ZipFile", s__new, NULL, 
            10, s_method, 0, NULL, 0, NULL, NULL, NULL,
            &object_base::class_info()
        };

        static ClassInfo s_ci(s_cd);
        return s_ci;
    }


    inline void ZipFile_base::s_namelist(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        obj_ptr<List_base> vr;

        METHOD_INSTANCE(ZipFile_base);
        ASYNC_METHOD_ENTER(0, 0);

        if(!cb.IsEmpty()) {
            pInst->acb_namelist(vr, cb);
            hr = CALL_RETURN_NULL;
        } else
            hr = pInst->ac_namelist(vr);

        METHOD_RETURN();
    }

    inline void ZipFile_base::s_infolist(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        obj_ptr<List_base> vr;

        METHOD_INSTANCE(ZipFile_base);
        ASYNC_METHOD_ENTER(0, 0);

        if(!cb.IsEmpty()) {
            pInst->acb_infolist(vr, cb);
            hr = CALL_RETURN_NULL;
        } else
            hr = pInst->ac_infolist(vr);

        METHOD_RETURN();
    }

    inline void ZipFile_base::s_getinfo(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        obj_ptr<ZipInfo_base> vr;

        METHOD_INSTANCE(ZipFile_base);
        ASYNC_METHOD_ENTER(1, 1);

        ARG(arg_string, 0);

        if(!cb.IsEmpty()) {
            pInst->acb_getinfo(v0, vr, cb);
            hr = CALL_RETURN_NULL;
        } else
            hr = pInst->ac_getinfo(v0, vr);

        METHOD_RETURN();
    }

    inline void ZipFile_base::s_read(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        obj_ptr<Buffer_base> vr;

        METHOD_INSTANCE(ZipFile_base);
        ASYNC_METHOD_ENTER(2, 1);

        ARG(arg_string, 0);
        OPT_ARG(arg_string, 1, "");

        if(!cb.IsEmpty()) {
            pInst->acb_read(v0, v1, vr, cb);
            hr = CALL_RETURN_NULL;
        } else
            hr = pInst->ac_read(v0, v1, vr);

        METHOD_RETURN();
    }

    inline void ZipFile_base::s_readAll(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        obj_ptr<List_base> vr;

        METHOD_INSTANCE(ZipFile_base);
        ASYNC_METHOD_ENTER(1, 0);

        OPT_ARG(arg_string, 0, "");

        if(!cb.IsEmpty()) {
            pInst->acb_readAll(v0, vr, cb);
            hr = CALL_RETURN_NULL;
        } else
            hr = pInst->ac_readAll(v0, vr);

        METHOD_RETURN();
    }

    inline void ZipFile_base::s_extract(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        METHOD_INSTANCE(ZipFile_base);
        ASYNC_METHOD_ENTER(3, 2);

        ARG(arg_string, 0);
        ARG(arg_string, 1);
        OPT_ARG(arg_string, 2, "");

        if(!cb.IsEmpty()) {
            pInst->acb_extract(v0, v1, v2, cb);
            hr = CALL_RETURN_NULL;
        } else
            hr = pInst->ac_extract(v0, v1, v2);

        METHOD_OVER(3, 2);

        ARG(arg_string, 0);
        ARG(obj_ptr<SeekableStream_base>, 1);
        OPT_ARG(arg_string, 2, "");

        if(!cb.IsEmpty()) {
            pInst->acb_extract(v0, v1, v2, cb);
            hr = CALL_RETURN_NULL;
        } else
            hr = pInst->ac_extract(v0, v1, v2);

        METHOD_VOID();
    }

    inline void ZipFile_base::s_extractAll(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        METHOD_INSTANCE(ZipFile_base);
        ASYNC_METHOD_ENTER(2, 1);

        ARG(arg_string, 0);
        OPT_ARG(arg_string, 1, "");

        if(!cb.IsEmpty()) {
            pInst->acb_extractAll(v0, v1, cb);
            hr = CALL_RETURN_NULL;
        } else
            hr = pInst->ac_extractAll(v0, v1);

        METHOD_VOID();
    }

    inline void ZipFile_base::s_setpasswd(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        METHOD_INSTANCE(ZipFile_base);
        METHOD_ENTER(1, 1);

        ARG(arg_string, 0);

        hr = pInst->setpasswd(v0);

        METHOD_VOID();
    }

    inline void ZipFile_base::s_write(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        METHOD_INSTANCE(ZipFile_base);
        ASYNC_METHOD_ENTER(1, 1);

        ARG(arg_string, 0);

        if(!cb.IsEmpty()) {
            pInst->acb_write(v0, cb);
            hr = CALL_RETURN_NULL;
        } else
            hr = pInst->ac_write(v0);

        METHOD_OVER(1, 1);

        ARG(obj_ptr<Buffer_base>, 0);

        if(!cb.IsEmpty()) {
            pInst->acb_write(v0, cb);
            hr = CALL_RETURN_NULL;
        } else
            hr = pInst->ac_write(v0);

        METHOD_OVER(1, 1);

        ARG(obj_ptr<SeekableStream_base>, 0);

        if(!cb.IsEmpty()) {
            pInst->acb_write(v0, cb);
            hr = CALL_RETURN_NULL;
        } else
            hr = pInst->ac_write(v0);

        METHOD_VOID();
    }

    inline void ZipFile_base::s_close(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        METHOD_INSTANCE(ZipFile_base);
        ASYNC_METHOD_ENTER(0, 0);

        if(!cb.IsEmpty()) {
            pInst->acb_close(cb);
            hr = CALL_RETURN_NULL;
        } else
            hr = pInst->ac_close();

        METHOD_VOID();
    }

}

#endif

