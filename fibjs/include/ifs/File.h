/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _File_base_H_
#define _File_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "SeekableStream.h"

namespace fibjs
{

class SeekableStream_base;

class File_base : public SeekableStream_base
{
    DECLARE_CLASS(File_base);

public:
    // File_base
    virtual result_t get_name(exlib::string& retVal) = 0;
    virtual result_t truncate(int64_t bytes, AsyncEvent* ac) = 0;
    virtual result_t eof(bool& retVal) = 0;
    virtual result_t flush(AsyncEvent* ac) = 0;
    virtual result_t chmod(int32_t mode, AsyncEvent* ac) = 0;

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        Isolate* isolate = Isolate::current();

        isolate->m_isolate->ThrowException(
            isolate->NewFromUtf8("not a constructor"));
    }

public:
    static void s_get_name(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_truncate(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_eof(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_flush(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_chmod(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
    ASYNC_MEMBER1(File_base, truncate, int64_t);
    ASYNC_MEMBER0(File_base, flush);
    ASYNC_MEMBER1(File_base, chmod, int32_t);
};

}


namespace fibjs
{
    inline ClassInfo& File_base::class_info()
    {
        static ClassData::ClassMethod s_method[] = 
        {
            {"truncate", s_truncate, false},
            {"eof", s_eof, false},
            {"flush", s_flush, false},
            {"chmod", s_chmod, false}
        };

        static ClassData::ClassProperty s_property[] = 
        {
            {"name", s_get_name, block_set, false}
        };

        static ClassData s_cd = 
        { 
            "File", s__new, NULL, 
            4, s_method, 0, NULL, 1, s_property, NULL, NULL,
            &SeekableStream_base::class_info()
        };

        static ClassInfo s_ci(s_cd);
        return s_ci;
    }

    inline void File_base::s_get_name(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        exlib::string vr;

        PROPERTY_ENTER();
        PROPERTY_INSTANCE(File_base);

        hr = pInst->get_name(vr);

        METHOD_RETURN();
    }

    inline void File_base::s_truncate(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        METHOD_INSTANCE(File_base);
        ASYNC_METHOD_ENTER(1, 1);

        ARG(int64_t, 0);

        if(!cb.IsEmpty()) {
            pInst->acb_truncate(v0, cb);
            hr = CALL_RETURN_NULL;
        } else
            hr = pInst->ac_truncate(v0);

        METHOD_VOID();
    }

    inline void File_base::s_eof(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        bool vr;

        METHOD_INSTANCE(File_base);
        METHOD_ENTER(0, 0);

        hr = pInst->eof(vr);

        METHOD_RETURN();
    }

    inline void File_base::s_flush(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        METHOD_INSTANCE(File_base);
        ASYNC_METHOD_ENTER(0, 0);

        if(!cb.IsEmpty()) {
            pInst->acb_flush(cb);
            hr = CALL_RETURN_NULL;
        } else
            hr = pInst->ac_flush();

        METHOD_VOID();
    }

    inline void File_base::s_chmod(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        METHOD_INSTANCE(File_base);
        ASYNC_METHOD_ENTER(1, 1);

        ARG(int32_t, 0);

        if(!cb.IsEmpty()) {
            pInst->acb_chmod(v0, cb);
            hr = CALL_RETURN_NULL;
        } else
            hr = pInst->ac_chmod(v0);

        METHOD_VOID();
    }

}

#endif

