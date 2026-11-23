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
        return result_t::from_system(ret);

    retVal = info.sysname;
    return result_t();
}

result_t os_base::release(exlib::string& retVal)
{
    uv_utsname_t info;
    int32_t ret = uv_os_uname(&info);
    if (ret < 0)
        return result_t::from_system(ret);

    retVal = info.release;
    return result_t();
}

result_t os_base::machine(exlib::string& retVal)
{
    uv_utsname_t info;
    int32_t ret = uv_os_uname(&info);
    if (ret < 0)
        return result_t::from_system(ret);

    retVal = info.machine;
    return result_t();
}

result_t os_base::version(exlib::string& retVal)
{
    uv_utsname_t info;
    int32_t ret = uv_os_uname(&info);
    if (ret < 0)
        return result_t::from_system(ret);

    retVal = info.version;
    return result_t();
}

result_t os_base::get_devNull(exlib::string& retVal)
{
#ifdef Windows
    retVal = "\\\\.\\nul";
#else
    retVal = "/dev/null";
#endif

    return result_t();
}

result_t os_base::uptime(int64_t& retVal)
{
    double up;

    // uv_uptime reports fractional seconds; the JS surface takes whole seconds
    // like Node's os.uptime().
    int32_t ret = uv_uptime(&up);
    if (ret < 0)
        return result_t::from_system(ret);

    retVal = (int64_t)up;
    return result_t();
}

result_t os_base::availableParallelism(int32_t& retVal)
{
    // uv_available_parallelism honours the CPU affinity mask and cgroup quota, so
    // inside a container it reports the allowance rather than the machine's CPU
    // count -- the difference from cpuNumbers (a fibjs extension that caches
    // the logical CPU count).
    retVal = (int32_t)uv_available_parallelism();
    return result_t();
}

result_t os_base::hostname(exlib::string& retVal)
{
    char s[255];

    if (gethostname(s, 255) < 0)
        return LastError("gethostname");

    retVal = s;
    return result_t();
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

    return result_t();
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

    return result_t();
}

result_t os_base::get_EOL(exlib::string& retVal)
{
#ifdef Windows
    retVal.assign("\r\n", 2);
#else
    retVal.assign(1, '\n');
#endif

    return result_t();
}

result_t os_base::endianness(exlib::string& retVal)
{
    retVal = is_big_endian() ? "BE" : "LE";
    return result_t();
}

result_t os_base::time(exlib::string tmString, date_t& retVal)
{
    if (tmString.empty())
        retVal.now();
    else
        retVal.parse(tmString);

    return result_t();
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
        return result_t::make(CALL_E_INVALID_DATA).with_type_name(errtype::kTypeError).with_code("ERR_INVALID_ARG_VALUE").with_format("Invalid date part: '%s'.", part.c_str()).with_result(result_t::invalid_arg());

    return result_t();
}

result_t os_base::get_timezone(int32_t& retVal)
{
    retVal = date_t::timezone();
    return result_t();
}

static int32_t s_cpus = 0;

result_t os_base::cpuNumbers(int32_t& retVal)
{
    if (s_cpus == 0) {
        uv_cpu_info_t* cpu_infos;
        int32_t ret = uv_cpu_info(&cpu_infos, &s_cpus);
        if (ret < 0)
            return result_t::from_system(ret);
        uv_free_cpu_info(cpu_infos, s_cpus);
    }

    retVal = s_cpus;
    return result_t();
}

result_t os_base::cpus(std::vector<obj_ptr<CpusType>>& retVal)
{
    uv_cpu_info_t* cpu_infos;
    int32_t ret = uv_cpu_info(&cpu_infos, &s_cpus);
    if (ret < 0)
        return result_t::from_system(ret);

    retVal.clear();
    retVal.reserve(s_cpus);

    for (int32_t i = 0; i < s_cpus; i++) {
        obj_ptr<CpusType> cpuinfo = new CpusType();
        obj_ptr<CpusType_times> cputimes = new CpusType_times();

        cputimes->user = (double)cpu_infos[i].cpu_times.user;
        cputimes->nice = (double)cpu_infos[i].cpu_times.nice;
        cputimes->sys = (double)cpu_infos[i].cpu_times.sys;
        cputimes->idle = (double)cpu_infos[i].cpu_times.idle;
        cputimes->irq = (double)cpu_infos[i].cpu_times.irq;

        cpuinfo->model = cpu_infos[i].model;
        cpuinfo->speed = cpu_infos[i].speed;
        cpuinfo->times = cputimes;

        retVal.push_back(cpuinfo);
    }

    uv_free_cpu_info(cpu_infos, s_cpus);

    return result_t();
}

result_t os_base::networkInterfaces(ValueMap& retVal)
{
    int32_t ifs = 0;
    uv_interface_address_t* interfaces;

    int32_t ret = uv_interface_addresses(&interfaces, &ifs);
    if (ret < 0)
        return result_t::from_system(ret);

    Isolate* isolate = Isolate::current();
    v8::Local<v8::Context> context = isolate->context();
    ValueMap map;

    for (int32_t i = 0; i < ifs;) {
        const char* if_name = interfaces[i].name;
        exlib::string if_key(if_name);
        v8::Local<v8::Array> ret;
        v8::Local<v8::Value>* existing = map.find(if_key);
        v8::Local<v8::String> family;

        if (existing && !existing->IsEmpty() && (*existing)->IsArray())
            ret = existing->As<v8::Array>();
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

        map.set(if_key, ret);
    }

    uv_free_interface_addresses(interfaces, ifs);

    retVal = std::move(map);
    return result_t();
}

result_t os_base::userInfo(v8::Local<v8::Object> options, obj_ptr<UserInfoType>& retVal)
{
    exlib::string encoding = "utf8";
    exlib::string homedir;
    exlib::string username;
    exlib::string shell;

    // Deliberately lenient, matching Node: os.userInfo({encoding: 123})
    // returns the default-encoded result instead of throwing, because the
    // option only selects how the strings are rendered.
    GetConfigValue(options, "encoding", encoding, true);

    uv_passwd_t pwd;
    int32_t ret = uv_os_get_passwd(&pwd);
    if (ret < 0)
        return result_t::from_system(ret);

    homedir.assign(pwd.homedir);
    username.assign(pwd.username);
    if (pwd.shell)
        shell.assign(pwd.shell);

    retVal = new UserInfoType();
    retVal->uid = (int32_t)pwd.uid;
    retVal->gid = (int32_t)pwd.gid;

    uv_os_free_passwd(&pwd);

    if (encoding == "buffer") {
        retVal->username = obj_ptr<Buffer_base>(new Buffer(username.c_str(), username.length()));
        retVal->homedir = obj_ptr<Buffer_base>(new Buffer(homedir.c_str(), homedir.length()));

        if (!shell.empty())
            retVal->shell = obj_ptr<Buffer_base>(new Buffer(shell.c_str(), shell.length()));
        else
            retVal->shell.setNull();
    } else {
        commonEncode(encoding, username, username);
        commonEncode(encoding, homedir, homedir);
        commonEncode(encoding, shell, shell);

        retVal->username = username;
        retVal->homedir = homedir;

        if (!shell.empty())
            retVal->shell = shell;
        else
            retVal->shell.setNull();
    }

    return result_t();
}

result_t os_base::loadavg(std::vector<double>& retVal)
{
    double avg[3] = { 0, 0, 0 };

    uv_loadavg(avg);

    retVal.push_back(avg[0]);
    retVal.push_back(avg[1]);
    retVal.push_back(avg[2]);

    return result_t();
}

result_t os_base::totalmem(int64_t& retVal)
{
    retVal = uv_get_total_memory();
    return result_t();
}

result_t os_base::freemem(int64_t& retVal)
{
    retVal = uv_get_free_memory();
    return result_t();
}

#ifdef iPhone

extern "C" const char* get_documents_directory();

result_t os_base::homedir(exlib::string& retVal)
{
    retVal = get_documents_directory();
    return result_t();
}
#else
result_t os_base::homedir(exlib::string& retVal)
{
    char buf[1024] = "";
    size_t size = sizeof(buf);

    int32_t ret = uv_os_homedir(buf, &size);
    if (ret < 0)
        return result_t::from_system(ret);
    retVal = buf;

    return result_t();
}
#endif

result_t os_base::tmpdir(exlib::string& retVal)
{
    char buf[1024] = "";
    size_t size = sizeof(buf);

    int32_t ret = uv_os_tmpdir(buf, &size);
    if (ret < 0)
        return result_t::from_system(ret);
    retVal = buf;

    return result_t();
}
}
