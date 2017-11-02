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

namespace fibjs {

class SeekableStream_base;

class File_base : public SeekableStream_base {
    DECLARE_CLASS(File_base);

public:
    // File_base
    virtual result_t get_name(exlib::string& retVal) = 0;
    virtual result_t get_fd(int32_t& retVal) = 0;
    virtual result_t chmod(int32_t mode, AsyncEvent* ac) = 0;

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        Isolate* isolate = Isolate::current();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_get_name(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_fd(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_chmod(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
    ASYNC_MEMBER1(File_base, chmod, int32_t);
};
}

namespace fibjs {
inline ClassInfo& File_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "chmod", s_chmod, false },
        { "chmodSync", s_chmod, false }
    };

    static ClassData::ClassProperty s_property[] = {
        { "name", s_get_name, block_set, false },
        { "fd", s_get_fd, block_set, false }
    };

    static ClassData s_cd = {
        "File", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &SeekableStream_base::class_info()
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void File_base::s_get_name(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_NAME("File.name");
    METHOD_INSTANCE(File_base);
    PROPERTY_ENTER();

    hr = pInst->get_name(vr);

    METHOD_RETURN();
}

inline void File_base::s_get_fd(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_NAME("File.fd");
    METHOD_INSTANCE(File_base);
    PROPERTY_ENTER();

    hr = pInst->get_fd(vr);

    METHOD_RETURN();
}

inline void File_base::s_chmod(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("File.chmod");
    METHOD_INSTANCE(File_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(int32_t, 0);

    if (!cb.IsEmpty()) {
        pInst->acb_chmod(v0, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = pInst->ac_chmod(v0);

    METHOD_VOID();
}
}

#endif
