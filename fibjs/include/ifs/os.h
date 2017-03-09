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

namespace fibjs
{

class Service_base;
class BufferedStream_base;

class os_base : public object_base
{
    DECLARE_CLASS(os_base);

public:
    // os_base
    static result_t get_hostname(exlib::string& retVal);
    static result_t get_type(exlib::string& retVal);
    static result_t get_version(exlib::string& retVal);
    static result_t get_arch(exlib::string& retVal);
    static result_t get_timezone(int32_t& retVal);
    static result_t get_EOL(exlib::string& retVal);
    static result_t uptime(double& retVal);
    static result_t loadavg(v8::Local<v8::Array>& retVal);
    static result_t totalmem(int64_t& retVal);
    static result_t freemem(int64_t& retVal);
    static result_t CPUInfo(v8::Local<v8::Array>& retVal);
    static result_t CPUs(int32_t& retVal);
    static result_t tmpdir(exlib::string& retVal);
    static result_t networkInfo(v8::Local<v8::Object>& retVal);
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
            isolate->NewFromUtf8("not a constructor"));
    }

public:
    static void s_get_hostname(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get_type(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get_version(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get_arch(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get_timezone(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get_EOL(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_uptime(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_loadavg(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_totalmem(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_freemem(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_CPUInfo(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_CPUs(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_tmpdir(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_networkInfo(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_printerInfo(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_openPrinter(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_platform(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_time(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_dateAdd(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_execPath(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_memoryUsage(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
    ASYNC_STATICVALUE2(os_base, openPrinter, exlib::string, obj_ptr<BufferedStream_base>);
};

}

#include "Service.h"
#include "BufferedStream.h"

namespace fibjs
{
    inline ClassInfo& os_base::class_info()
    {
        static ClassData::ClassMethod s_method[] = 
        {
            {"uptime", s_uptime, true},
            {"loadavg", s_loadavg, true},
            {"totalmem", s_totalmem, true},
            {"freemem", s_freemem, true},
            {"CPUInfo", s_CPUInfo, true},
            {"CPUs", s_CPUs, true},
            {"tmpdir", s_tmpdir, true},
            {"networkInfo", s_networkInfo, true},
            {"printerInfo", s_printerInfo, true},
            {"openPrinter", s_openPrinter, true},
            {"platform", s_platform, true},
            {"time", s_time, true},
            {"dateAdd", s_dateAdd, true},
            {"memoryUsage", s_memoryUsage, true}
        };

        static ClassData::ClassObject s_object[] = 
        {
            {"Service", Service_base::class_info}
        };

        static ClassData::ClassProperty s_property[] = 
        {
            {"hostname", s_get_hostname, block_set, true},
            {"type", s_get_type, block_set, true},
            {"version", s_get_version, block_set, true},
            {"arch", s_get_arch, block_set, true},
            {"timezone", s_get_timezone, block_set, true},
            {"EOL", s_get_EOL, block_set, true},
            {"execPath", s_get_execPath, block_set, true}
        };

        static ClassData s_cd = 
        { 
            "os", true, s__new, NULL, 
            ARRAYSIZE(s_method), s_method, ARRAYSIZE(s_object), s_object, ARRAYSIZE(s_property), s_property, NULL, NULL,
            &object_base::class_info()
        };

        static ClassInfo s_ci(s_cd);
        return s_ci;
    }

    inline void os_base::s_get_hostname(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        exlib::string vr;

        PROPERTY_ENTER();

        hr = get_hostname(vr);

        METHOD_RETURN();
    }

    inline void os_base::s_get_type(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        exlib::string vr;

        PROPERTY_ENTER();

        hr = get_type(vr);

        METHOD_RETURN();
    }

    inline void os_base::s_get_version(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        exlib::string vr;

        PROPERTY_ENTER();

        hr = get_version(vr);

        METHOD_RETURN();
    }

    inline void os_base::s_get_arch(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        exlib::string vr;

        PROPERTY_ENTER();

        hr = get_arch(vr);

        METHOD_RETURN();
    }

    inline void os_base::s_get_timezone(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        int32_t vr;

        PROPERTY_ENTER();

        hr = get_timezone(vr);

        METHOD_RETURN();
    }

    inline void os_base::s_get_EOL(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        exlib::string vr;

        PROPERTY_ENTER();

        hr = get_EOL(vr);

        METHOD_RETURN();
    }

    inline void os_base::s_uptime(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        double vr;

        METHOD_ENTER();

        METHOD_OVER(0, 0);

        hr = uptime(vr);

        METHOD_RETURN();
    }

    inline void os_base::s_loadavg(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        v8::Local<v8::Array> vr;

        METHOD_ENTER();

        METHOD_OVER(0, 0);

        hr = loadavg(vr);

        METHOD_RETURN();
    }

    inline void os_base::s_totalmem(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        int64_t vr;

        METHOD_ENTER();

        METHOD_OVER(0, 0);

        hr = totalmem(vr);

        METHOD_RETURN();
    }

    inline void os_base::s_freemem(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        int64_t vr;

        METHOD_ENTER();

        METHOD_OVER(0, 0);

        hr = freemem(vr);

        METHOD_RETURN();
    }

    inline void os_base::s_CPUInfo(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        v8::Local<v8::Array> vr;

        METHOD_ENTER();

        METHOD_OVER(0, 0);

        hr = CPUInfo(vr);

        METHOD_RETURN();
    }

    inline void os_base::s_CPUs(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        int32_t vr;

        METHOD_ENTER();

        METHOD_OVER(0, 0);

        hr = CPUs(vr);

        METHOD_RETURN();
    }

    inline void os_base::s_tmpdir(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        exlib::string vr;

        METHOD_ENTER();

        METHOD_OVER(0, 0);

        hr = tmpdir(vr);

        METHOD_RETURN();
    }

    inline void os_base::s_networkInfo(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        v8::Local<v8::Object> vr;

        METHOD_ENTER();

        METHOD_OVER(0, 0);

        hr = networkInfo(vr);

        METHOD_RETURN();
    }

    inline void os_base::s_printerInfo(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        v8::Local<v8::Array> vr;

        METHOD_ENTER();

        METHOD_OVER(0, 0);

        hr = printerInfo(vr);

        METHOD_RETURN();
    }

    inline void os_base::s_openPrinter(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        obj_ptr<BufferedStream_base> vr;

        METHOD_ENTER();

        ASYNC_METHOD_OVER(1, 1);

        ARG(exlib::string, 0);

        if(!cb.IsEmpty()) {
            acb_openPrinter(v0, cb);
            hr = CALL_RETURN_NULL;
        } else
            hr = ac_openPrinter(v0, vr);

        METHOD_RETURN();
    }

    inline void os_base::s_platform(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        exlib::string vr;

        METHOD_ENTER();

        METHOD_OVER(0, 0);

        hr = platform(vr);

        METHOD_RETURN();
    }

    inline void os_base::s_time(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        date_t vr;

        METHOD_ENTER();

        METHOD_OVER(1, 0);

        OPT_ARG(exlib::string, 0, "");

        hr = time(v0, vr);

        METHOD_RETURN();
    }

    inline void os_base::s_dateAdd(const v8::FunctionCallbackInfo<v8::Value>& args)
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

    inline void os_base::s_get_execPath(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        exlib::string vr;

        PROPERTY_ENTER();

        hr = get_execPath(vr);

        METHOD_RETURN();
    }

    inline void os_base::s_memoryUsage(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        v8::Local<v8::Object> vr;

        METHOD_ENTER();

        METHOD_OVER(0, 0);

        hr = memoryUsage(vr);

        METHOD_RETURN();
    }

}

#endif

