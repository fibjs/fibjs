/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _MemoryStream_base_H_
#define _MemoryStream_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "SeekableStream.h"

namespace fibjs {

class SeekableStream_base;

class MemoryStream_base : public SeekableStream_base {
    DECLARE_CLASS(MemoryStream_base);

public:
    // MemoryStream_base
    static result_t _new(obj_ptr<MemoryStream_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    virtual result_t setTime(date_t d) = 0;
    virtual result_t clone(obj_ptr<MemoryStream_base>& retVal) = 0;
    virtual result_t clear() = 0;

public:
    template <typename T>
    static void __new(const T& args);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_setTime(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_clone(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_clear(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

namespace fibjs {
inline ClassInfo& MemoryStream_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "setTime", s_setTime, false },
        { "clone", s_clone, false },
        { "clear", s_clear, false }
    };

    static ClassData s_cd = {
        "MemoryStream", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, 0, NULL, NULL, NULL,
        &SeekableStream_base::class_info()
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void MemoryStream_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    CONSTRUCT_INIT();
    __new(args);
}

template <typename T>
void MemoryStream_base::__new(const T& args)
{
    obj_ptr<MemoryStream_base> vr;

    CONSTRUCT_ENTER("new MemoryStream()");

    METHOD_OVER(0, 0);

    hr = _new(vr, args.This());

    CONSTRUCT_RETURN();
}

inline void MemoryStream_base::s_setTime(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(MemoryStream_base);
    METHOD_ENTER("MemoryStream.setTime");

    METHOD_OVER(1, 1);

    ARG(date_t, 0);

    hr = pInst->setTime(v0);

    METHOD_VOID();
}

inline void MemoryStream_base::s_clone(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<MemoryStream_base> vr;

    METHOD_INSTANCE(MemoryStream_base);
    METHOD_ENTER("MemoryStream.clone");

    METHOD_OVER(0, 0);

    hr = pInst->clone(vr);

    METHOD_RETURN();
}

inline void MemoryStream_base::s_clear(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(MemoryStream_base);
    METHOD_ENTER("MemoryStream.clear");

    METHOD_OVER(0, 0);

    hr = pInst->clear();

    METHOD_VOID();
}
}

#endif
