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

class RangeStream_base : public SeekableStream_base {
    DECLARE_CLASS(RangeStream_base);

public:
    // RangeStream_base
    static result_t _new(SeekableStream_base* stm, exlib::string range, obj_ptr<RangeStream_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    static result_t _new(SeekableStream_base* stm, int64_t begin, int64_t end, obj_ptr<RangeStream_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    virtual result_t get_begin(int64_t& retVal) = 0;
    virtual result_t get_end(int64_t& retVal) = 0;

public:
    template <typename T>
    static void __new(const T& args);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_begin(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_end(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

namespace fibjs {
inline ClassInfo& RangeStream_base::class_info()
{
    static ClassData::ClassProperty s_property[] = {
        { "begin", s_get_begin, block_set, false },
        { "end", s_get_end, block_set, false }
    };

    static ClassData s_cd = {
        "RangeStream", false, s__new, NULL,
        0, NULL, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &SeekableStream_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void RangeStream_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    CONSTRUCT_INIT();
    __new(args);
}

template <typename T>
void RangeStream_base::__new(const T& args)
{
    obj_ptr<RangeStream_base> vr;

    CONSTRUCT_ENTER();

    METHOD_OVER(2, 2);

    ARG(obj_ptr<SeekableStream_base>, 0);
    ARG(exlib::string, 1);

    hr = _new(v0, v1, vr, args.This());

    METHOD_OVER(3, 3);

    ARG(obj_ptr<SeekableStream_base>, 0);
    ARG(int64_t, 1);
    ARG(int64_t, 2);

    hr = _new(v0, v1, v2, vr, args.This());

    CONSTRUCT_RETURN();
}

inline void RangeStream_base::s_get_begin(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int64_t vr;

    METHOD_INSTANCE(RangeStream_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_begin(vr);

    METHOD_RETURN();
}

inline void RangeStream_base::s_get_end(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int64_t vr;

    METHOD_INSTANCE(RangeStream_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_end(vr);

    METHOD_RETURN();
}
}
