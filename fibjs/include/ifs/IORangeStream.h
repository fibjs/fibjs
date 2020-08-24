/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _IORangeStream_base_H_
#define _IORangeStream_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "ifs/SeekableStream.h"

namespace fibjs {

class SeekableStream_base;
class Stat_base;

class IORangeStream_base : public SeekableStream_base {
    DECLARE_CLASS(IORangeStream_base);

public:
    // IORangeStream_base
    static result_t _new(SeekableStream_base* stm, int32_t begin, int32_t end, obj_ptr<IORangeStream_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    virtual result_t stat(obj_ptr<Stat_base>& retVal, AsyncEvent* ac) = 0;

public:
    template <typename T>
    static void __new(const T& args);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_stat(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
    ASYNC_MEMBERVALUE1(IORangeStream_base, stat, obj_ptr<Stat_base>);
};
}

#include "ifs/Stat.h"

namespace fibjs {
inline ClassInfo& IORangeStream_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "stat", s_stat, false },
        { "statSync", s_stat, false }
    };

    static ClassData s_cd = {
        "IORangeStream", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, 0, NULL, 0, NULL, NULL, NULL,
        &SeekableStream_base::class_info()
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void IORangeStream_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    CONSTRUCT_INIT();
    __new(args);
}

template <typename T>
void IORangeStream_base::__new(const T& args)
{
    obj_ptr<IORangeStream_base> vr;

    METHOD_NAME("new IORangeStream()");
    CONSTRUCT_ENTER();

    METHOD_OVER(3, 3);

    ARG(obj_ptr<SeekableStream_base>, 0);
    ARG(int32_t, 1);
    ARG(int32_t, 2);

    hr = _new(v0, v1, v2, vr, args.This());

    CONSTRUCT_RETURN();
}

inline void IORangeStream_base::s_stat(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Stat_base> vr;

    METHOD_NAME("IORangeStream.stat");
    METHOD_INSTANCE(IORangeStream_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(0, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_stat(cb, args);
    else
        hr = pInst->ac_stat(vr);

    METHOD_RETURN();
}
}

#endif
