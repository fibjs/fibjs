/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _os_base_H_
#define _os_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs {

class Service_base;
class BufferedStream_base;

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
    static result_t uptime(double& retVal);
    static result_t loadavg(v8::Local<v8::Array>& retVal);
    static result_t totalmem(int64_t& retVal);
    static result_t freemem(int64_t& retVal);
    static result_t cpus(v8::Local<v8::Array>& retVal);
    static result_t cpuNumbers(int32_t& retVal);
    static result_t tmpdir(exlib::string& retVal);
    static result_t userInfo(v8::Local<v8::Object> options, v8::Local<v8::Object>& retVal);
    static result_t networkInterfaces(v8::Local<v8::Object>& retVal);
    static result_t printerInfo(v8::Local<v8::Array>& retVal);
    static result_t openPrinter(exlib::string name, obj_ptr<BufferedStream_base>& retVal, AsyncEvent* ac);
    static result_t platform(exlib::string& retVal);
    static result_t time(exlib::string tmString, date_t& retVal);
    static result_t dateAdd(date_t d, int32_t num, exlib::string part, date_t& retVal);
    static result_t get_execPath(exlib::string& retVal);
    static result_t memoryUsage(v8::Local<v8::Object>& retVal);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        Isolate* isolate = Isolate::current();

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
    static void s_static_get_timezone(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_static_get_EOL(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_static_uptime(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_loadavg(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_totalmem(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_freemem(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_cpus(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_cpuNumbers(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_tmpdir(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_userInfo(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_networkInterfaces(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_printerInfo(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_openPrinter(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_platform(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_time(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_dateAdd(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_get_execPath(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_static_memoryUsage(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
    ASYNC_STATICVALUE2(os_base, openPrinter, exlib::string, obj_ptr<BufferedStream_base>);
};
}

#include "ifs/Service.h"
#include "ifs/BufferedStream.h"

namespace fibjs {
inline ClassInfo& os_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "hostname", s_static_hostname, true },
        { "endianness", s_static_endianness, true },
        { "type", s_static_type, true },
        { "release", s_static_release, true },
        { "homedir", s_static_homedir, true },
        { "arch", s_static_arch, true },
        { "uptime", s_static_uptime, true },
        { "loadavg", s_static_loadavg, true },
        { "totalmem", s_static_totalmem, true },
        { "freemem", s_static_freemem, true },
        { "cpus", s_static_cpus, true },
        { "cpuNumbers", s_static_cpuNumbers, true },
        { "tmpdir", s_static_tmpdir, true },
        { "userInfo", s_static_userInfo, true },
        { "networkInterfaces", s_static_networkInterfaces, true },
        { "printerInfo", s_static_printerInfo, true },
        { "openPrinter", s_static_openPrinter, true },
        { "openPrinterSync", s_static_openPrinter, true },
        { "platform", s_static_platform, true },
        { "time", s_static_time, true },
        { "dateAdd", s_static_dateAdd, true },
        { "memoryUsage", s_static_memoryUsage, true }
    };

    static ClassData::ClassObject s_object[] = {
        { "Service", Service_base::class_info }
    };

    static ClassData::ClassProperty s_property[] = {
        { "timezone", s_static_get_timezone, block_set, true },
        { "EOL", s_static_get_EOL, block_set, true },
        { "execPath", s_static_get_execPath, block_set, true }
    };

    static ClassData s_cd = {
        "os", true, s__new, NULL,
        ARRAYSIZE(s_method), s_method, ARRAYSIZE(s_object), s_object, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &object_base::class_info()
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void os_base::s_static_hostname(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_NAME("os.hostname");
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = hostname(vr);

    METHOD_RETURN();
}

inline void os_base::s_static_endianness(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_NAME("os.endianness");
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = endianness(vr);

    METHOD_RETURN();
}

inline void os_base::s_static_type(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_NAME("os.type");
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = type(vr);

    METHOD_RETURN();
}

inline void os_base::s_static_release(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_NAME("os.release");
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = release(vr);

    METHOD_RETURN();
}

inline void os_base::s_static_homedir(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_NAME("os.homedir");
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = homedir(vr);

    METHOD_RETURN();
}

inline void os_base::s_static_arch(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_NAME("os.arch");
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = arch(vr);

    METHOD_RETURN();
}

inline void os_base::s_static_get_timezone(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_NAME("os.timezone");
    PROPERTY_ENTER();

    hr = get_timezone(vr);

    METHOD_RETURN();
}

inline void os_base::s_static_get_EOL(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_NAME("os.EOL");
    PROPERTY_ENTER();

    hr = get_EOL(vr);

    METHOD_RETURN();
}

inline void os_base::s_static_uptime(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    double vr;

    METHOD_NAME("os.uptime");
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = uptime(vr);

    METHOD_RETURN();
}

inline void os_base::s_static_loadavg(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Array> vr;

    METHOD_NAME("os.loadavg");
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = loadavg(vr);

    METHOD_RETURN();
}

inline void os_base::s_static_totalmem(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int64_t vr;

    METHOD_NAME("os.totalmem");
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = totalmem(vr);

    METHOD_RETURN();
}

inline void os_base::s_static_freemem(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int64_t vr;

    METHOD_NAME("os.freemem");
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = freemem(vr);

    METHOD_RETURN();
}

inline void os_base::s_static_cpus(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Array> vr;

    METHOD_NAME("os.cpus");
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = cpus(vr);

    METHOD_RETURN();
}

inline void os_base::s_static_cpuNumbers(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_NAME("os.cpuNumbers");
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = cpuNumbers(vr);

    METHOD_RETURN();
}

inline void os_base::s_static_tmpdir(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_NAME("os.tmpdir");
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = tmpdir(vr);

    METHOD_RETURN();
}

inline void os_base::s_static_userInfo(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Object> vr;

    METHOD_NAME("os.userInfo");
    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(v8::Local<v8::Object>, 0, v8::Object::New(isolate));

    hr = userInfo(v0, vr);

    METHOD_RETURN();
}

inline void os_base::s_static_networkInterfaces(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Object> vr;

    METHOD_NAME("os.networkInterfaces");
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = networkInterfaces(vr);

    METHOD_RETURN();
}

inline void os_base::s_static_printerInfo(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Array> vr;

    METHOD_NAME("os.printerInfo");
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = printerInfo(vr);

    METHOD_RETURN();
}

inline void os_base::s_static_openPrinter(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<BufferedStream_base> vr;

    METHOD_NAME("os.openPrinter");
    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    if (!cb.IsEmpty())
        hr = acb_openPrinter(v0, cb, args);
    else
        hr = ac_openPrinter(v0, vr);

    METHOD_RETURN();
}

inline void os_base::s_static_platform(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_NAME("os.platform");
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = platform(vr);

    METHOD_RETURN();
}

inline void os_base::s_static_time(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    date_t vr;

    METHOD_NAME("os.time");
    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(exlib::string, 0, "");

    hr = time(v0, vr);

    METHOD_RETURN();
}

inline void os_base::s_static_dateAdd(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    date_t vr;

    METHOD_NAME("os.dateAdd");
    METHOD_ENTER();

    METHOD_OVER(3, 3);

    ARG(date_t, 0);
    ARG(int32_t, 1);
    ARG(exlib::string, 2);

    hr = dateAdd(v0, v1, v2, vr);

    METHOD_RETURN();
}

inline void os_base::s_static_get_execPath(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_NAME("os.execPath");
    PROPERTY_ENTER();

    hr = get_execPath(vr);

    METHOD_RETURN();
}

inline void os_base::s_static_memoryUsage(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Object> vr;

    METHOD_NAME("os.memoryUsage");
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = memoryUsage(vr);

    METHOD_RETURN();
}
}

#endif
