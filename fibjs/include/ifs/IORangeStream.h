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

class IORangeStream_base : public SeekableStream_base {
    DECLARE_CLASS(IORangeStream_base);

public:
    // IORangeStream_base
    static result_t _new(SeekableStream_base* stm, exlib::string range, obj_ptr<IORangeStream_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    static result_t _new(SeekableStream_base* stm, int32_t begin, int32_t end, obj_ptr<IORangeStream_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    virtual result_t get_begin(int32_t& retVal) = 0;
    virtual result_t get_end(int32_t& retVal) = 0;

public:
    template <typename T>
    static void __new(const T& args);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_begin(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_end(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
};
}

namespace fibjs {
inline ClassInfo& IORangeStream_base::class_info()
{
    static ClassData::ClassProperty s_property[] = {
        { "begin", s_get_begin, block_set, false },
        { "end", s_get_end, block_set, false }
    };

    static ClassData s_cd = {
        "IORangeStream", false, s__new, NULL,
        0, NULL, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
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

inline void IORangeStream_base::s_get_begin(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_NAME("IORangeStream.begin");
    METHOD_INSTANCE(IORangeStream_base);
    PROPERTY_ENTER();

    hr = pInst->get_begin(vr);

    METHOD_RETURN();
}

inline void IORangeStream_base::s_get_end(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_NAME("IORangeStream.end");
    METHOD_INSTANCE(IORangeStream_base);
    PROPERTY_ENTER();

    hr = pInst->get_end(vr);

    METHOD_RETURN();
}
}

#endif
