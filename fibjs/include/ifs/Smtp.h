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

namespace fibjs {

class Stream_base;

class Smtp_base : public object_base {
    DECLARE_CLASS(Smtp_base);

public:
    // Smtp_base
    static result_t _new(obj_ptr<Smtp_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    virtual result_t connect(exlib::string url, AsyncEvent* ac) = 0;
    virtual result_t command(exlib::string cmd, exlib::string arg, exlib::string& retVal, AsyncEvent* ac) = 0;
    virtual result_t hello(exlib::string hostname, AsyncEvent* ac) = 0;
    virtual result_t login(exlib::string username, exlib::string password, AsyncEvent* ac) = 0;
    virtual result_t from(exlib::string address, AsyncEvent* ac) = 0;
    virtual result_t to(exlib::string address, AsyncEvent* ac) = 0;
    virtual result_t data(exlib::string txt, AsyncEvent* ac) = 0;
    virtual result_t quit(AsyncEvent* ac) = 0;
    virtual result_t get_timeout(int32_t& retVal) = 0;
    virtual result_t set_timeout(int32_t newVal) = 0;
    virtual result_t get_socket(obj_ptr<Stream_base>& retVal) = 0;

public:
    template <typename T>
    static void __new(const T& args);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_connect(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_command(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_hello(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_login(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_from(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_to(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_data(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_quit(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_timeout(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_timeout(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_socket(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
    ASYNC_MEMBER1(Smtp_base, connect, exlib::string);
    ASYNC_MEMBERVALUE3(Smtp_base, command, exlib::string, exlib::string, exlib::string);
    ASYNC_MEMBER1(Smtp_base, hello, exlib::string);
    ASYNC_MEMBER2(Smtp_base, login, exlib::string, exlib::string);
    ASYNC_MEMBER1(Smtp_base, from, exlib::string);
    ASYNC_MEMBER1(Smtp_base, to, exlib::string);
    ASYNC_MEMBER1(Smtp_base, data, exlib::string);
    ASYNC_MEMBER0(Smtp_base, quit);
};
}

#include "ifs/Stream.h"

namespace fibjs {
inline ClassInfo& Smtp_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "connect", s_connect, false, ClassData::ASYNC_ASYNC },
        { "command", s_command, false, ClassData::ASYNC_ASYNC },
        { "hello", s_hello, false, ClassData::ASYNC_ASYNC },
        { "login", s_login, false, ClassData::ASYNC_ASYNC },
        { "from", s_from, false, ClassData::ASYNC_ASYNC },
        { "to", s_to, false, ClassData::ASYNC_ASYNC },
        { "data", s_data, false, ClassData::ASYNC_ASYNC },
        { "quit", s_quit, false, ClassData::ASYNC_ASYNC }
    };

    static ClassData::ClassProperty s_property[] = {
        { "timeout", s_get_timeout, s_set_timeout, false },
        { "socket", s_get_socket, block_set, false }
    };

    static ClassData s_cd = {
        "Smtp", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &object_base::class_info(),
        true
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void Smtp_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    CONSTRUCT_INIT();
    __new(args);
}

template <typename T>
void Smtp_base::__new(const T& args)
{
    obj_ptr<Smtp_base> vr;

    CONSTRUCT_ENTER();

    METHOD_OVER(0, 0);

    hr = _new(vr, args.This());

    CONSTRUCT_RETURN();
}

inline void Smtp_base::s_connect(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ASYNC_METHOD_INSTANCE(Smtp_base);
    ASYNC_METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_connect(v0, cb, args);
    else
        hr = pInst->ac_connect(v0);

    METHOD_VOID();
}

inline void Smtp_base::s_command(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    ASYNC_METHOD_INSTANCE(Smtp_base);
    ASYNC_METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(exlib::string, 1);

    if (!cb.IsEmpty())
        hr = pInst->acb_command(v0, v1, cb, args);
    else
        hr = pInst->ac_command(v0, v1, vr);

    METHOD_RETURN();
}

inline void Smtp_base::s_hello(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ASYNC_METHOD_INSTANCE(Smtp_base);
    ASYNC_METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(exlib::string, 0, "localhost");

    if (!cb.IsEmpty())
        hr = pInst->acb_hello(v0, cb, args);
    else
        hr = pInst->ac_hello(v0);

    METHOD_VOID();
}

inline void Smtp_base::s_login(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ASYNC_METHOD_INSTANCE(Smtp_base);
    ASYNC_METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(exlib::string, 1);

    if (!cb.IsEmpty())
        hr = pInst->acb_login(v0, v1, cb, args);
    else
        hr = pInst->ac_login(v0, v1);

    METHOD_VOID();
}

inline void Smtp_base::s_from(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ASYNC_METHOD_INSTANCE(Smtp_base);
    ASYNC_METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_from(v0, cb, args);
    else
        hr = pInst->ac_from(v0);

    METHOD_VOID();
}

inline void Smtp_base::s_to(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ASYNC_METHOD_INSTANCE(Smtp_base);
    ASYNC_METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_to(v0, cb, args);
    else
        hr = pInst->ac_to(v0);

    METHOD_VOID();
}

inline void Smtp_base::s_data(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ASYNC_METHOD_INSTANCE(Smtp_base);
    ASYNC_METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_data(v0, cb, args);
    else
        hr = pInst->ac_data(v0);

    METHOD_VOID();
}

inline void Smtp_base::s_quit(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ASYNC_METHOD_INSTANCE(Smtp_base);
    ASYNC_METHOD_ENTER();

    METHOD_OVER(0, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_quit(cb, args);
    else
        hr = pInst->ac_quit();

    METHOD_VOID();
}

inline void Smtp_base::s_get_timeout(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(Smtp_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_timeout(vr);

    METHOD_RETURN();
}

inline void Smtp_base::s_set_timeout(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(Smtp_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(int32_t, 0);

    hr = pInst->set_timeout(v0);

    METHOD_VOID();
}

inline void Smtp_base::s_get_socket(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Stream_base> vr;

    METHOD_INSTANCE(Smtp_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_socket(vr);

    METHOD_RETURN();
}
}
