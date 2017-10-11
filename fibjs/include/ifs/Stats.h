/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _Stats_base_H_
#define _Stats_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs {

class Stats_base : public object_base {
    DECLARE_CLASS(Stats_base);

public:
    // Stats_base
    static result_t _new(v8::Local<v8::Array> keys, obj_ptr<Stats_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    static result_t _new(v8::Local<v8::Array> staticKeys, v8::Local<v8::Array> keys, obj_ptr<Stats_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    virtual result_t inc(exlib::string key) = 0;
    virtual result_t dec(exlib::string key) = 0;
    virtual result_t add(exlib::string key, int32_t value) = 0;
    virtual result_t reset() = 0;
    virtual result_t uptime(int32_t& retVal) = 0;
    virtual result_t _named_getter(exlib::string property, int32_t& retVal) = 0;
    virtual result_t _named_enumerator(v8::Local<v8::Array>& retVal) = 0;

public:
    template <typename T>
    static void __new(const T& args);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_inc(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_dec(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_add(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_reset(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_uptime(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void i_NamedGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void i_NamedEnumerator(const v8::PropertyCallbackInfo<v8::Array>& args);
};
}

namespace fibjs {
inline ClassInfo& Stats_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "inc", s_inc, false },
        { "dec", s_dec, false },
        { "add", s_add, false },
        { "reset", s_reset, false },
        { "uptime", s_uptime, false }
    };

    static ClassData::ClassNamed s_named = {
        i_NamedGetter, i_NamedSetter, i_NamedDeleter, i_NamedEnumerator
    };

    static ClassData s_cd = {
        "Stats", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, 0, NULL, NULL, &s_named,
        &object_base::class_info()
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void Stats_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    CONSTRUCT_INIT();
    __new(args);
}

template <typename T>
void Stats_base::__new(const T& args)
{
    obj_ptr<Stats_base> vr;

    METHOD_NAME("new Stats()");
    CONSTRUCT_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Array>, 0);

    hr = _new(v0, vr, args.This());

    METHOD_OVER(2, 2);

    ARG(v8::Local<v8::Array>, 0);
    ARG(v8::Local<v8::Array>, 1);

    hr = _new(v0, v1, vr, args.This());

    CONSTRUCT_RETURN();
}

inline void Stats_base::s_inc(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("Stats.inc");
    METHOD_INSTANCE(Stats_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = pInst->inc(v0);

    METHOD_VOID();
}

inline void Stats_base::s_dec(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("Stats.dec");
    METHOD_INSTANCE(Stats_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = pInst->dec(v0);

    METHOD_VOID();
}

inline void Stats_base::s_add(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("Stats.add");
    METHOD_INSTANCE(Stats_base);
    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(int32_t, 1);

    hr = pInst->add(v0, v1);

    METHOD_VOID();
}

inline void Stats_base::s_reset(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("Stats.reset");
    METHOD_INSTANCE(Stats_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->reset();

    METHOD_VOID();
}

inline void Stats_base::s_uptime(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_NAME("Stats.uptime");
    METHOD_INSTANCE(Stats_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->uptime(vr);

    METHOD_RETURN();
}

inline void Stats_base::i_NamedGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_NAME("Stats.undefined[]");
    METHOD_INSTANCE(Stats_base);
    PROPERTY_ENTER();

    exlib::string k;
    GetArgumentValue(isolate, property, k);
    if (class_info().has(k.c_str()))
        return;

    hr = pInst->_named_getter(k, vr);
    if (hr == CALL_RETURN_NULL)
        return;

    METHOD_RETURN();
}

inline void Stats_base::i_NamedEnumerator(const v8::PropertyCallbackInfo<v8::Array>& args)
{
    v8::Local<v8::Array> vr;

    METHOD_NAME("Stats.undefined");
    METHOD_INSTANCE(Stats_base);
    PROPERTY_ENTER();

    hr = pInst->_named_enumerator(vr);

    METHOD_RETURN1();
}
}

#endif
