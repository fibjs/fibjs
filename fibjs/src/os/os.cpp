/*
 * os.cpp
 *
 *  Created on: Apr 7, 2012
 *      Author: lion
 */

#include "object.h"
#include "ifs/os.h"
#include "AsyncUV.h"
#include "Buffer.h"
#include "encoding.h"

#ifndef _WIN32
#include <unistd.h> // gethostname, sysconf
#include <sys/utsname.h>
#endif

namespace fibjs {

DECLARE_MODULE(os);

result_t os_base::type(exlib::string& retVal)
{
#ifndef _WIN32
    uv_utsname_t info;
    int32_t ret = uv_os_uname(&info);
    if (ret < 0)
        return CHECK_ERROR(ret);

    retVal = info.sysname;
#else
    retVal = "Windows";
#endif
    return 0;
}

result_t os_base::release(exlib::string& retVal)
{
    uv_utsname_t info;
    int32_t ret = uv_os_uname(&info);
    if (ret < 0)
        return CHECK_ERROR(ret);

    retVal = info.release;
    return 0;
}

result_t os_base::hostname(exlib::string& retVal)
{
    char s[255];

    if (gethostname(s, 255) < 0)
        return CHECK_ERROR(LastError());

    retVal = s;
    return 0;
}

result_t os_base::platform(exlib::string& retVal)
{
#ifdef Linux
    retVal = "linux";
#elif defined(Windows)
    retVal = "win32";
#elif defined(Darwin)
    retVal = "darwin";
#elif defined(FreeBSD)
    retVal = "freebsd";
#endif

    return 0;
}

result_t os_base::arch(exlib::string& retVal)
{
#ifdef amd64
    retVal = "x64";
#elif defined(i386)
    retVal = "ia32";
#elif defined(arm)
    retVal = "arm";
#elif defined(arm64)
    retVal = "arm64";
#elif defined(mips)
    retVal = "mips";
#elif defined(mips64)
    retVal = "mips64";
#endif

    return 0;
}

result_t os_base::get_EOL(exlib::string& retVal)
{
#ifdef Windows
    retVal.assign("\r\n", 2);
#else
    retVal.assign(1, '\n');
#endif

    return 0;
}

result_t os_base::endianness(exlib::string& retVal)
{
    const union {
        uint8_t u8[2];
        uint16_t u16;
    } u = {
        { 1, 0 }
    };

    retVal = u.u16 == 1 ? "LE" : "BE";
    return 0;
}

result_t os_base::time(exlib::string tmString, date_t& retVal)
{
    if (tmString.empty())
        retVal.now();
    else
        retVal.parse(tmString);

    return 0;
}

result_t os_base::dateAdd(date_t d, int32_t num, exlib::string part, date_t& retVal)
{
    retVal = d;
    if (part == "year")
        retVal.add(num, date_t::_YEAR);
    else if (part == "month")
        retVal.add(num, date_t::_MONTH);
    else if (part == "day")
        retVal.add(num, date_t::_DAY);
    else if (part == "hour")
        retVal.add(num, date_t::_HOUR);
    else if (part == "minute")
        retVal.add(num, date_t::_MINUTE);
    else if (part == "second")
        retVal.add(num, date_t::_SECOND);
    else
        return CHECK_ERROR(CALL_E_INVALIDARG);

    return 0;
}

result_t os_base::get_timezone(int32_t& retVal)
{
    retVal = date_t::timezone();
    return 0;
}

result_t os_base::uptime(double& retVal)
{
    int32_t ret = uv_uptime(&retVal);
    if (ret < 0)
        return CHECK_ERROR(ret);
    return 0;
}

static int32_t s_cpus = 0;
static uv_cpu_info_t* s_cpu_infos;

result_t os_base::cpuNumbers(int32_t& retVal)
{
    if (s_cpus == 0) {
        int32_t ret = uv_cpu_info(&s_cpu_infos, &s_cpus);
        if (ret < 0)
            return CHECK_ERROR(ret);
    }

    retVal = s_cpus;
    return 0;
}

result_t os_base::cpus(v8::Local<v8::Array>& retVal)
{
    if (s_cpus == 0) {
        int32_t ret = uv_cpu_info(&s_cpu_infos, &s_cpus);
        if (ret < 0)
            return CHECK_ERROR(ret);
    }

    Isolate* isolate = Isolate::current();
    retVal = v8::Array::New(isolate->m_isolate, s_cpus);

    for (int32_t i = 0; i < s_cpus; i++) {
        v8::Local<v8::Object> cpuinfo = v8::Object::New(isolate->m_isolate);
        v8::Local<v8::Object> cputimes = v8::Object::New(isolate->m_isolate);

        cputimes->Set(isolate->NewString("user"),
            v8::Number::New(isolate->m_isolate, (double)s_cpu_infos[i].cpu_times.user));
        cputimes->Set(isolate->NewString("nice"),
            v8::Number::New(isolate->m_isolate, (double)s_cpu_infos[i].cpu_times.nice));
        cputimes->Set(isolate->NewString("sys"),
            v8::Number::New(isolate->m_isolate, (double)s_cpu_infos[i].cpu_times.sys));
        cputimes->Set(isolate->NewString("idle"),
            v8::Number::New(isolate->m_isolate, (double)s_cpu_infos[i].cpu_times.idle));
        cputimes->Set(isolate->NewString("irq"),
            v8::Number::New(isolate->m_isolate, (double)s_cpu_infos[i].cpu_times.irq));

        cpuinfo->Set(isolate->NewString("model"), isolate->NewString(s_cpu_infos[i].model));
        cpuinfo->Set(isolate->NewString("speed"), v8::Number::New(isolate->m_isolate, s_cpu_infos[i].speed));
        cpuinfo->Set(isolate->NewString("times"), cputimes);

        retVal->Set(i, cpuinfo);
    }

    return 0;
}

result_t os_base::networkInterfaces(v8::Local<v8::Object>& retVal)
{
    int32_t ifs = 0;
    uv_interface_address_t* interfaces;

    int32_t ret = uv_interface_addresses(&interfaces, &ifs);
    if (ret < 0)
        return CHECK_ERROR(ret);

    Isolate* isolate = Isolate::current();
    retVal = v8::Object::New(isolate->m_isolate);

    for (int32_t i = 0; i < ifs;) {
        char* if_name = interfaces[i].name;
        v8::Local<v8::Array> ret;
        v8::Local<v8::Object> o;
        v8::Local<v8::String> name, family;

        name = isolate->NewString(if_name);

        ret = v8::Array::New(isolate->m_isolate);
        while (i < ifs && !qstrcmp(if_name, interfaces[i].name)) {
            unsigned char* phys_addr = (unsigned char*)interfaces[i].phys_addr;
            char ip[INET6_ADDRSTRLEN] = "";
            char netmask[INET6_ADDRSTRLEN] = "";
            char mac[18];

            v8::Local<v8::Object> o = v8::Object::New(isolate->m_isolate);

            if (interfaces[i].address.address4.sin_family == AF_INET6) {
                uv_ip6_name(&interfaces[i].address.address6, ip, INET6_ADDRSTRLEN);
                uv_ip6_name(&interfaces[i].netmask.netmask6, netmask, INET6_ADDRSTRLEN);
                family = isolate->NewString("IPv6");
            } else if (interfaces[i].address.address4.sin_family == AF_INET) {
                uv_ip4_name(&interfaces[i].address.address4, ip, INET6_ADDRSTRLEN);
                uv_ip4_name(&interfaces[i].netmask.netmask4, netmask, INET6_ADDRSTRLEN);
                family = isolate->NewString("IPv4");
            } else {
                family = isolate->NewString("Unknown");
            }

            o->Set(isolate->NewString("address"), isolate->NewString(ip));
            o->Set(isolate->NewString("netmask"), isolate->NewString(netmask));
            o->Set(isolate->NewString("family"), family);

            sprintf(mac, "%02x:%02x:%02x:%02x:%02x:%02x",
                phys_addr[0], phys_addr[1], phys_addr[2], phys_addr[3], phys_addr[4], phys_addr[5]);
            o->Set(isolate->NewString("mac"), isolate->NewString(mac));

            o->Set(isolate->NewString("internal"),
                interfaces[i].is_internal ? v8::True(isolate->m_isolate) : v8::False(isolate->m_isolate));

            if (interfaces[i].address.address4.sin_family == AF_INET6)
                o->Set(isolate->NewString("scopeid"),
                    v8::Number::New(isolate->m_isolate, interfaces[i].address.address6.sin6_scope_id));

            ret->Set(ret->Length(), o);

            i++;
        }

        retVal->Set(name, ret);
    }

    uv_free_interface_addresses(interfaces, ifs);

    return 0;
}

result_t os_base::userInfo(v8::Local<v8::Object> options, v8::Local<v8::Object>& retVal)
{
    Isolate* isolate = Isolate::current();
    retVal = v8::Object::New(isolate->m_isolate);

    uv_passwd_t pwd;
    exlib::string encoding = "utf8";
    exlib::string homedir;
    exlib::string username;
    exlib::string shell;

    GetConfigValue(isolate->m_isolate, options, "encoding", encoding, true);

    int32_t ret = uv_os_get_passwd(&pwd);
    if (ret < 0)
        return CHECK_ERROR(ret);

    homedir.assign(pwd.homedir);
    username.assign(pwd.username);
    if (pwd.shell)
        shell.assign(pwd.shell);

    retVal->Set(isolate->NewString("uid"), v8::Integer::New(isolate->m_isolate, pwd.uid));
    retVal->Set(isolate->NewString("gid"), v8::Integer::New(isolate->m_isolate, pwd.gid));

    uv_os_free_passwd(&pwd);

    if (encoding == "buffer") {
        obj_ptr<Buffer_base> usernameBuffer = new Buffer(username);
        obj_ptr<Buffer_base> homedirBuffer = new Buffer(homedir);
        obj_ptr<Buffer_base> shellBuffer = new Buffer(shell);

        retVal->Set(isolate->NewString("username"), usernameBuffer->wrap());
        retVal->Set(isolate->NewString("homedir"), homedirBuffer->wrap());
        if (!shell.empty())
            retVal->Set(isolate->NewString("shell"), shellBuffer->wrap());
        else
            retVal->Set(isolate->NewString("shell"), v8::Null(isolate->m_isolate));

        return 0;
    } else {
        commonEncode(encoding, username, username);
        commonEncode(encoding, homedir, homedir);
        commonEncode(encoding, shell, shell);

        retVal->Set(isolate->NewString("username"), isolate->NewString(username));
        retVal->Set(isolate->NewString("homedir"), isolate->NewString(homedir));
        if (!shell.empty())
            retVal->Set(isolate->NewString("shell"), isolate->NewString(shell));
        else
            retVal->Set(isolate->NewString("shell"), v8::Null(isolate->m_isolate));

        return 0;
    }
}

result_t os_base::loadavg(v8::Local<v8::Array>& retVal)
{
    Isolate* isolate = Isolate::current();
    double avg[3] = { 0, 0, 0 };

    uv_loadavg(avg);

    retVal = v8::Array::New(isolate->m_isolate, 3);
    retVal->Set(0, v8::Number::New(isolate->m_isolate, avg[0]));
    retVal->Set(1, v8::Number::New(isolate->m_isolate, avg[1]));
    retVal->Set(2, v8::Number::New(isolate->m_isolate, avg[2]));

    return 0;
}

result_t os_base::totalmem(int64_t& retVal)
{
    retVal = uv_get_total_memory();
    return 0;
}

result_t os_base::freemem(int64_t& retVal)
{
    retVal = uv_get_free_memory();
    return 0;
}

result_t os_base::get_execPath(exlib::string& retVal)
{
    char buf[1024] = "";
    size_t size = sizeof(buf);

    int32_t ret = uv_exepath(buf, &size);
    if (ret < 0)
        return CHECK_ERROR(ret);

    retVal = buf;

    return 0;
}

result_t os_base::memoryUsage(v8::Local<v8::Object>& retVal)
{
    Isolate* isolate = Isolate::current();
    v8::Local<v8::Object> info = v8::Object::New(isolate->m_isolate);

    size_t rss;
    int32_t ret = uv_resident_set_memory(&rss);
    if (ret < 0)
        return CHECK_ERROR(ret);

    info->Set(isolate->NewString("rss"), v8::Number::New(isolate->m_isolate, (double)rss));

    v8::HeapStatistics v8_heap_stats;
    isolate->m_isolate->GetHeapStatistics(&v8_heap_stats);

    info->Set(isolate->NewString("heapTotal"),
        v8::Number::New(isolate->m_isolate, (double)v8_heap_stats.total_heap_size()));
    info->Set(isolate->NewString("heapUsed"),
        v8::Number::New(isolate->m_isolate, (double)v8_heap_stats.used_heap_size()));
    info->Set(isolate->NewString("external"),
        v8::Number::New(isolate->m_isolate, (double)v8_heap_stats.external_memory()));

    v8::Local<v8::Object> objs;
    object_base::class_info().dump(objs);
    info->Set(isolate->NewString("nativeObjects"), objs);

    retVal = info;

    return 0;
}

result_t os_base::homedir(exlib::string& retVal)
{
    char buf[1024] = "";
    size_t size = sizeof(buf);

    int32_t ret = uv_os_homedir(buf, &size);
    if (ret < 0)
        return CHECK_ERROR(ret);
    retVal = buf;

    return 0;
}

result_t os_base::tmpdir(exlib::string& retVal)
{
    char buf[1024] = "";
    size_t size = sizeof(buf);

    int32_t ret = uv_os_tmpdir(buf, &size);
    if (ret < 0)
        return CHECK_ERROR(ret);
    retVal = buf;

    return 0;
}
}
