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
    uv_utsname_t info;
    int32_t ret = uv_os_uname(&info);
    if (ret < 0)
        return CHECK_ERROR(ret);

    retVal = info.sysname;
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
#ifdef __ANDROID__
    retVal = "android";
#else
    retVal = "linux";
#endif
#elif defined(Windows)
    retVal = "win32";
#elif defined(Darwin)
#ifdef iPhone
    retVal = "ios";
#else
    retVal = "darwin";
#endif
#elif defined(FreeBSD)
    retVal = "freebsd";
#endif

    return 0;
}

result_t os_base::arch(exlib::string& retVal)
{
#ifdef x64
    retVal = "x64";
#elif defined(ia32)
    retVal = "ia32";
#elif defined(arm)
    retVal = "arm";
#elif defined(arm64)
    retVal = "arm64";
#elif defined(mips)
    retVal = "mips";
#elif defined(mips64)
    retVal = "mips64";
#elif defined(ppc64)
    retVal = "ppc64";
#elif defined(riscv64)
    retVal = "riscv64";
#elif defined(loong64)
    retVal = "loong64";
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
    retVal = is_big_endian() ? "BE" : "LE";
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

static int32_t s_cpus = 0;

result_t os_base::cpuNumbers(int32_t& retVal)
{
    if (s_cpus == 0) {
        uv_cpu_info_t* cpu_infos;
        int32_t ret = uv_cpu_info(&cpu_infos, &s_cpus);
        if (ret < 0)
            return CHECK_ERROR(ret);
        uv_free_cpu_info(cpu_infos, s_cpus);
    }

    retVal = s_cpus;
    return 0;
}

result_t os_base::cpus(v8::Local<v8::Array>& retVal)
{
    uv_cpu_info_t* cpu_infos;
    int32_t ret = uv_cpu_info(&cpu_infos, &s_cpus);
    if (ret < 0)
        return CHECK_ERROR(ret);

    Isolate* isolate = Isolate::current();
    v8::Local<v8::Context> context = isolate->context();
    retVal = v8::Array::New(isolate->m_isolate, s_cpus);

    for (int32_t i = 0; i < s_cpus; i++) {
        v8::Local<v8::Object> cpuinfo = v8::Object::New(isolate->m_isolate);
        v8::Local<v8::Object> cputimes = v8::Object::New(isolate->m_isolate);

        cputimes->Set(context, isolate->NewString("user"),
                    v8::Number::New(isolate->m_isolate, (double)cpu_infos[i].cpu_times.user))
            .IsJust();
        cputimes->Set(context, isolate->NewString("nice"),
                    v8::Number::New(isolate->m_isolate, (double)cpu_infos[i].cpu_times.nice))
            .IsJust();
        cputimes->Set(context, isolate->NewString("sys"),
                    v8::Number::New(isolate->m_isolate, (double)cpu_infos[i].cpu_times.sys))
            .IsJust();
        cputimes->Set(context, isolate->NewString("idle"),
                    v8::Number::New(isolate->m_isolate, (double)cpu_infos[i].cpu_times.idle))
            .IsJust();
        cputimes->Set(context, isolate->NewString("irq"),
                    v8::Number::New(isolate->m_isolate, (double)cpu_infos[i].cpu_times.irq))
            .IsJust();

        cpuinfo->Set(context, isolate->NewString("model"), isolate->NewString(cpu_infos[i].model)).IsJust();
        cpuinfo->Set(context, isolate->NewString("speed"), v8::Number::New(isolate->m_isolate, cpu_infos[i].speed)).IsJust();
        cpuinfo->Set(context, isolate->NewString("times"), cputimes).IsJust();

        retVal->Set(context, i, cpuinfo).IsJust();
    }

    uv_free_cpu_info(cpu_infos, s_cpus);

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
    v8::Local<v8::Context> context = isolate->context();
    retVal = v8::Object::New(isolate->m_isolate);

    for (int32_t i = 0; i < ifs;) {
        char* if_name = interfaces[i].name;
        v8::Local<v8::Array> ret;
        v8::Local<v8::Value> v;
        v8::Local<v8::String> name, family;

        name = isolate->NewString(if_name);

        v = retVal->Get(context, name).FromMaybe(v8::Local<v8::Value>());
        if (!v.IsEmpty() && !v->IsUndefined())
            ret = v8::Local<v8::Array>::Cast(v);
        else
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

            o->Set(context, isolate->NewString("address"), isolate->NewString(ip)).IsJust();
            o->Set(context, isolate->NewString("netmask"), isolate->NewString(netmask)).IsJust();
            o->Set(context, isolate->NewString("family"), family).IsJust();

            snprintf(mac, sizeof(mac), "%02x:%02x:%02x:%02x:%02x:%02x",
                phys_addr[0], phys_addr[1], phys_addr[2], phys_addr[3], phys_addr[4], phys_addr[5]);
            o->Set(context, isolate->NewString("mac"), isolate->NewString(mac)).IsJust();

            o->Set(context, isolate->NewString("internal"),
                 interfaces[i].is_internal ? v8::True(isolate->m_isolate) : v8::False(isolate->m_isolate))
                .IsJust();

            if (interfaces[i].address.address4.sin_family == AF_INET6)
                o->Set(context, isolate->NewString("scopeid"),
                     v8::Number::New(isolate->m_isolate, interfaces[i].address.address6.sin6_scope_id))
                    .IsJust();

            ret->Set(context, ret->Length(), o).IsJust();

            i++;
        }

        retVal->Set(context, name, ret).IsJust();
    }

    uv_free_interface_addresses(interfaces, ifs);

    return 0;
}

result_t os_base::userInfo(v8::Local<v8::Object> options, v8::Local<v8::Object>& retVal)
{
    Isolate* isolate = Isolate::current();
    v8::Local<v8::Context> context = isolate->context();
    retVal = v8::Object::New(isolate->m_isolate);

    uv_passwd_t pwd;
    exlib::string encoding = "utf8";
    exlib::string homedir;
    exlib::string username;
    exlib::string shell;

    GetConfigValue(isolate, options, "encoding", encoding, true);

    int32_t ret = uv_os_get_passwd(&pwd);
    if (ret < 0)
        return CHECK_ERROR(ret);

    homedir.assign(pwd.homedir);
    username.assign(pwd.username);
    if (pwd.shell)
        shell.assign(pwd.shell);

    retVal->Set(context, isolate->NewString("uid"), v8::Integer::New(isolate->m_isolate, pwd.uid)).IsJust();
    retVal->Set(context, isolate->NewString("gid"), v8::Integer::New(isolate->m_isolate, pwd.gid)).IsJust();

    uv_os_free_passwd(&pwd);

    if (encoding == "buffer") {
        obj_ptr<Buffer_base> usernameBuffer = new Buffer(username.c_str(), username.length());
        obj_ptr<Buffer_base> homedirBuffer = new Buffer(homedir.c_str(), homedir.length());
        obj_ptr<Buffer_base> shellBuffer = new Buffer(shell.c_str(), shell.length());

        retVal->Set(context, isolate->NewString("username"), usernameBuffer->wrap()).IsJust();
        retVal->Set(context, isolate->NewString("homedir"), homedirBuffer->wrap()).IsJust();
        if (!shell.empty())
            retVal->Set(context, isolate->NewString("shell"), shellBuffer->wrap()).IsJust();
        else
            retVal->Set(context, isolate->NewString("shell"), v8::Null(isolate->m_isolate)).IsJust();

        return 0;
    } else {
        commonEncode(encoding, username, username);
        commonEncode(encoding, homedir, homedir);
        commonEncode(encoding, shell, shell);

        retVal->Set(context, isolate->NewString("username"), isolate->NewString(username)).IsJust();
        retVal->Set(context, isolate->NewString("homedir"), isolate->NewString(homedir)).IsJust();
        if (!shell.empty())
            retVal->Set(context, isolate->NewString("shell"), isolate->NewString(shell)).IsJust();
        else
            retVal->Set(context, isolate->NewString("shell"), v8::Null(isolate->m_isolate)).IsJust();

        return 0;
    }
}

result_t os_base::loadavg(v8::Local<v8::Array>& retVal)
{
    Isolate* isolate = Isolate::current();
    v8::Local<v8::Context> context = isolate->context();
    double avg[3] = { 0, 0, 0 };

    uv_loadavg(avg);

    retVal = v8::Array::New(isolate->m_isolate, 3);
    retVal->Set(context, 0, v8::Number::New(isolate->m_isolate, avg[0])).IsJust();
    retVal->Set(context, 1, v8::Number::New(isolate->m_isolate, avg[1])).IsJust();
    retVal->Set(context, 2, v8::Number::New(isolate->m_isolate, avg[2])).IsJust();

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
