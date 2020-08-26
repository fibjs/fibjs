/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _Range_base_H_
#define _Range_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs {

class SeekableStream_base;
class Buffer_base;

class Range_base : public object_base {
    DECLARE_CLASS(Range_base);

public:
    // Range_base
    static result_t _new(SeekableStream_base* stm, exlib::string range, obj_ptr<Range_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    static result_t _new(SeekableStream_base* stm, int32_t begin, int32_t end, obj_ptr<Range_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    virtual result_t clone(obj_ptr<SeekableStream_base>& retVal, AsyncEvent* ac) = 0;
    virtual result_t seek(int64_t offset, int32_t whence) = 0;
    virtual result_t current(int64_t& retVal) = 0;
    virtual result_t rewind() = 0;
    virtual result_t slice(obj_ptr<Buffer_base>& retVal) = 0;
    virtual result_t size(int64_t& retVal) = 0;
    virtual result_t get_begin(int32_t& retVal) = 0;
    virtual result_t get_end(int32_t& retVal) = 0;

public:
    template <typename T>
    static void __new(const T& args);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_clone(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_seek(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_current(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_rewind(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_slice(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_size(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_begin(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_end(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);

public:
    ASYNC_MEMBERVALUE1(Range_base, clone, obj_ptr<SeekableStream_base>);
};
}

#include "ifs/SeekableStream.h"
#include "ifs/Buffer.h"

namespace fibjs {
inline ClassInfo& Range_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "clone", s_clone, false },
        { "cloneSync", s_clone, false },
        { "seek", s_seek, false },
        { "current", s_current, false },
        { "rewind", s_rewind, false },
        { "slice", s_slice, false },
        { "size", s_size, false }
    };

    static ClassData::ClassProperty s_property[] = {
        { "begin", s_get_begin, block_set, false },
        { "end", s_get_end, block_set, false }
    };

    static ClassData s_cd = {
        "Range", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &object_base::class_info()
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void Range_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    CONSTRUCT_INIT();
    __new(args);
}

template <typename T>
void Range_base::__new(const T& args)
{
    obj_ptr<Range_base> vr;

    METHOD_NAME("new Range()");
    CONSTRUCT_ENTER();

    METHOD_OVER(2, 2);

    ARG(obj_ptr<SeekableStream_base>, 0);
    ARG(exlib::string, 1);

    hr = _new(v0, v1, vr, args.This());

    METHOD_OVER(3, 3);

    ARG(obj_ptr<SeekableStream_base>, 0);
    ARG(int32_t, 1);
    ARG(int32_t, 2);

    hr = _new(v0, v1, v2, vr, args.This());

    CONSTRUCT_RETURN();
}

inline void Range_base::s_clone(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<SeekableStream_base> vr;

    METHOD_NAME("Range.clone");
    METHOD_INSTANCE(Range_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(0, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_clone(cb, args);
    else
        hr = pInst->ac_clone(vr);

    METHOD_RETURN();
}

inline void Range_base::s_seek(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("Range.seek");
    METHOD_INSTANCE(Range_base);
    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(int64_t, 0);
    ARG(int32_t, 1);

    hr = pInst->seek(v0, v1);

    METHOD_VOID();
}

inline void Range_base::s_current(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int64_t vr;

    METHOD_NAME("Range.current");
    METHOD_INSTANCE(Range_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->current(vr);

    METHOD_RETURN();
}

inline void Range_base::s_rewind(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("Range.rewind");
    METHOD_INSTANCE(Range_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->rewind();

    METHOD_VOID();
}

inline void Range_base::s_slice(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    METHOD_NAME("Range.slice");
    METHOD_INSTANCE(Range_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->slice(vr);

    METHOD_RETURN();
}

inline void Range_base::s_size(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int64_t vr;

    METHOD_NAME("Range.size");
    METHOD_INSTANCE(Range_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->size(vr);

    METHOD_RETURN();
}

inline void Range_base::s_get_begin(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_NAME("Range.begin");
    METHOD_INSTANCE(Range_base);
    PROPERTY_ENTER();

    hr = pInst->get_begin(vr);

    METHOD_RETURN();
}

inline void Range_base::s_get_end(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_NAME("Range.end");
    METHOD_INSTANCE(Range_base);
    PROPERTY_ENTER();

    hr = pInst->get_end(vr);

    METHOD_RETURN();
}
}

#endif
