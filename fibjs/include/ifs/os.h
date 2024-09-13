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

class Service_base;

class os_base : public object_base {
    DECLARE_CLASS(os_base);

public:
    // os_base
    static result_t hostname(exlib::string& retVal);
    static result_t endianness(exlib::string& retVal);
    static result_t type(exlib::string& retVal);
    static result_t release(exlib::string& retVal);
    static result_t homedir(exlib::string& retVal);
    static result_t arch(exlib::string& retVal);
    static result_t get_timezone(int32_t& retVal);
    static result_t get_EOL(exlib::string& retVal);
    static result_t loadavg(v8::Local<v8::Array>& retVal);
    static result_t totalmem(int64_t& retVal);
    static result_t freemem(int64_t& retVal);
    static result_t cpus(v8::Local<v8::Array>& retVal);
    static result_t cpuNumbers(int32_t& retVal);
    static result_t tmpdir(exlib::string& retVal);
    static result_t userInfo(v8::Local<v8::Object> options, v8::Local<v8::Object>& retVal);
    static result_t networkInterfaces(v8::Local<v8::Object>& retVal);
    static result_t platform(exlib::string& retVal);
    static result_t time(exlib::string tmString, date_t& retVal);
    static result_t dateAdd(date_t d, int32_t num, exlib::string part, date_t& retVal);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_static_hostname(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_endianness(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_type(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_release(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_homedir(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_arch(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_get_timezone(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_get_EOL(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_loadavg(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_totalmem(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_freemem(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_cpus(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_cpuNumbers(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_tmpdir(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_userInfo(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_networkInterfaces(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_platform(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_time(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_dateAdd(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

#include "ifs/Service.h"

namespace fibjs {
inline ClassInfo& os_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "hostname", s_static_hostname, true, ClassData::ASYNC_SYNC },
        { "endianness", s_static_endianness, true, ClassData::ASYNC_SYNC },
        { "type", s_static_type, true, ClassData::ASYNC_SYNC },
        { "release", s_static_release, true, ClassData::ASYNC_SYNC },
        { "homedir", s_static_homedir, true, ClassData::ASYNC_SYNC },
        { "arch", s_static_arch, true, ClassData::ASYNC_SYNC },
        { "loadavg", s_static_loadavg, true, ClassData::ASYNC_SYNC },
        { "totalmem", s_static_totalmem, true, ClassData::ASYNC_SYNC },
        { "freemem", s_static_freemem, true, ClassData::ASYNC_SYNC },
        { "cpus", s_static_cpus, true, ClassData::ASYNC_SYNC },
        { "cpuNumbers", s_static_cpuNumbers, true, ClassData::ASYNC_SYNC },
        { "tmpdir", s_static_tmpdir, true, ClassData::ASYNC_SYNC },
        { "userInfo", s_static_userInfo, true, ClassData::ASYNC_SYNC },
        { "networkInterfaces", s_static_networkInterfaces, true, ClassData::ASYNC_SYNC },
        { "platform", s_static_platform, true, ClassData::ASYNC_SYNC },
        { "time", s_static_time, true, ClassData::ASYNC_SYNC },
        { "dateAdd", s_static_dateAdd, true, ClassData::ASYNC_SYNC }
    };

    static ClassData::ClassObject s_object[] = {
        { "Service", Service_base::class_info }
    };

    static ClassData::ClassProperty s_property[] = {
        { "timezone", s_static_get_timezone, block_set, true },
        { "EOL", s_static_get_EOL, block_set, true }
    };

    static ClassData s_cd = {
        "os", true, s__new, NULL,
        ARRAYSIZE(s_method), s_method, ARRAYSIZE(s_object), s_object, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &object_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void os_base::s_static_hostname(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = hostname(vr);

    METHOD_RETURN();
}

inline void os_base::s_static_endianness(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = endianness(vr);

    METHOD_RETURN();
}

inline void os_base::s_static_type(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = type(vr);

    METHOD_RETURN();
}

inline void os_base::s_static_release(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = release(vr);

    METHOD_RETURN();
}

inline void os_base::s_static_homedir(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = homedir(vr);

    METHOD_RETURN();
}

inline void os_base::s_static_arch(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = arch(vr);

    METHOD_RETURN();
}

inline void os_base::s_static_get_timezone(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = get_timezone(vr);

    METHOD_RETURN();
}

inline void os_base::s_static_get_EOL(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = get_EOL(vr);

    METHOD_RETURN();
}

inline void os_base::s_static_loadavg(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Array> vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = loadavg(vr);

    METHOD_RETURN();
}

inline void os_base::s_static_totalmem(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int64_t vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = totalmem(vr);

    METHOD_RETURN();
}

inline void os_base::s_static_freemem(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int64_t vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = freemem(vr);

    METHOD_RETURN();
}

inline void os_base::s_static_cpus(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Array> vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = cpus(vr);

    METHOD_RETURN();
}

inline void os_base::s_static_cpuNumbers(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = cpuNumbers(vr);

    METHOD_RETURN();
}

inline void os_base::s_static_tmpdir(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = tmpdir(vr);

    METHOD_RETURN();
}

inline void os_base::s_static_userInfo(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Object> vr;

    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(v8::Local<v8::Object>, 0, v8::Object::New(isolate->m_isolate));

    hr = userInfo(v0, vr);

    METHOD_RETURN();
}

inline void os_base::s_static_networkInterfaces(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Object> vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = networkInterfaces(vr);

    METHOD_RETURN();
}

inline void os_base::s_static_platform(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = platform(vr);

    METHOD_RETURN();
}

inline void os_base::s_static_time(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    date_t vr;

    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(exlib::string, 0, "");

    hr = time(v0, vr);

    METHOD_RETURN();
}

inline void os_base::s_static_dateAdd(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    date_t vr;

    METHOD_ENTER();

    METHOD_OVER(3, 3);

    ARG(date_t, 0);
    ARG(int32_t, 1);
    ARG(exlib::string, 2);

    hr = dateAdd(v0, v1, v2, vr);

    METHOD_RETURN();
}
}
