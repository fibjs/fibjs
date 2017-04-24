/*
 * os_posix.cpp
 *
 *  Created on: Nov 11, 2012
 *      Author: lion
 */

#include <exlib/include/osconfig.h>

#ifndef _WIN32

#include "object.h"
#include "ifs/os.h"
#include "ifs/path.h"
#include "ifs/process.h"
#include <sys/utsname.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <string.h>

#ifdef Linux
#include <linux/if_packet.h>
#elif defined(Darwin)
#include <net/if_dl.h>
#endif

namespace fibjs {

result_t os_base::get_type(exlib::string& retVal)
{
    struct utsname info;

    uname(&info);
    retVal = info.sysname;

    return 0;
}

result_t os_base::platform(exlib::string& retVal)
{
#ifdef Linux
    retVal = "linux";
#elif defined(Darwin)
    retVal = "darwin";
#elif defined(FreeBSD)
    retVal = "freebsd";
#endif

    return 0;
}

result_t os_base::get_version(exlib::string& retVal)
{
    struct utsname info;

    uname(&info);
    retVal = info.release;

    return 0;
}

result_t os_base::get_EOL(exlib::string& retVal)
{
    retVal.assign("\n", 1);
    return 0;
}

result_t os_base::networkInfo(v8::Local<v8::Object>& retVal)
{
    struct ::ifaddrs *addrs, *ent;
    struct ::sockaddr_in* in4;
    struct ::sockaddr_in6* in6;
    struct ::sockaddr_in* mask4;
    struct ::sockaddr_in6* mask6;
    char ip[INET6_ADDRSTRLEN];
    char netmask[INET6_ADDRSTRLEN];

    if (getifaddrs(&addrs) != 0)
        return CHECK_ERROR(LastError());

    Isolate* isolate = Isolate::current();

    retVal = v8::Object::New(isolate->m_isolate);

    for (ent = addrs; ent != NULL; ent = ent->ifa_next) {
        v8::Local<v8::Array> ret;
        v8::Local<v8::Object> o;
        v8::Local<v8::String> name, ipaddr, family;
        
        bzero(&ip, sizeof(ip));
        bzero(&netmask, sizeof(netmask));
        if (!(ent->ifa_flags & IFF_UP && ent->ifa_flags & IFF_RUNNING))
            continue;

        if (ent->ifa_addr == NULL)
            continue;

        if (ent->ifa_addr->sa_family != AF_INET6
            && ent->ifa_addr->sa_family != AF_INET)
            continue;
        
        name = isolate->NewFromUtf8(ent->ifa_name);
        if (retVal->Has(name)) {
            ret = v8::Local<v8::Array>::Cast(retVal->Get(name));
        } else {
            ret = v8::Array::New(isolate->m_isolate);
            retVal->Set(name, ret);
        }

        if (ent->ifa_addr->sa_family == AF_INET6) {
            in6 = (struct sockaddr_in6*)ent->ifa_addr;
            mask6 = (struct sockaddr_in6*)ent->ifa_netmask;

            inet_ntop(AF_INET6, &(in6->sin6_addr), ip, INET6_ADDRSTRLEN);
            inet_ntop(AF_INET6, &(mask6->sin6_addr), netmask, INET6_ADDRSTRLEN);
            family = isolate->NewFromUtf8("IPv6");
        } else if (ent->ifa_addr->sa_family == AF_INET) {
            in4 = (struct sockaddr_in*)ent->ifa_addr;
            mask4 = (struct sockaddr_in*)ent->ifa_netmask;

            inet_ntop(AF_INET, &(in4->sin_addr), ip, INET6_ADDRSTRLEN);
            inet_ntop(AF_INET, &(mask4->sin_addr), netmask, INET6_ADDRSTRLEN);
            family = isolate->NewFromUtf8("IPv4");
        }

        o = v8::Object::New(isolate->m_isolate);
        o->Set(isolate->NewFromUtf8("address"), isolate->NewFromUtf8(ip));
        o->Set(isolate->NewFromUtf8("netmask"), isolate->NewFromUtf8(netmask));
        o->Set(isolate->NewFromUtf8("family"), family);
        o->Set(isolate->NewFromUtf8("internal"),
            ent->ifa_flags & IFF_LOOPBACK ? v8::True(isolate->m_isolate) : v8::False(isolate->m_isolate));

        ret->Set(ret->Length(), o);
    }

    for (ent = addrs; ent != NULL; ent = ent->ifa_next) {
        if (!((ent->ifa_flags & IFF_UP) && (ent->ifa_flags & IFF_RUNNING)) ||
            (ent->ifa_addr == NULL) ||
#ifdef Linux
            (ent->ifa_addr->sa_family != AF_PACKET)) {
#else
            (ent->ifa_addr->sa_family != AF_LINK)) {
#endif
            continue;
        }

        v8::Local<v8::String> name;
        v8::Local<v8::Array> ret;
        unsigned char *ptr;
        char mac[18];

        name = isolate->NewFromUtf8(ent->ifa_name);

        if (!retVal->Has(name)) {
            continue;
        }
        ret = v8::Local<v8::Array>::Cast(retVal->Get(name));

#ifdef Linux
        struct sockaddr_ll *s = (struct sockaddr_ll*)ent->ifa_addr;
        int macAddrlen = 0;
        for(int i = 0; i < 6; i++)
            macAddrlen += sprintf(mac + macAddrlen, "%02X%s", s->sll_addr[i], i < 5 ? ":" : "");
#else
        ptr = (unsigned char *)LLADDR((struct sockaddr_dl*)(ent->ifa_addr));
        sprintf(mac, "%02x:%02x:%02x:%02x:%02x:%02x",
                *ptr, *(ptr+1), *(ptr+2), *(ptr+3), *(ptr+4), *(ptr+5));
#endif
        int32_t len = ret->Length();
        for (int i = 0; i < len; i++)
        {
            v8::Local<v8::Object> o = v8::Local<v8::Object>::Cast(ret->Get(i));
            o->Set(isolate->NewFromUtf8("mac"), isolate->NewFromUtf8(mac));
        }
    }

    freeifaddrs(addrs);

    return 0;
}

result_t os_base::printerInfo(v8::Local<v8::Array>& retVal)
{
    return CALL_E_INVALID_CALL;
}

result_t os_base::openPrinter(exlib::string name, obj_ptr<BufferedStream_base>& retVal,
    AsyncEvent* ac)
{
    return CALL_E_INVALID_CALL;
}

result_t os_base::tmpdir(exlib::string& retVal)
{
    Isolate* isolate = Isolate::current();
    v8::Local<v8::Object> env;
    process_base::get_env(env);

    do {
        GetConfigValue(isolate->m_isolate, env, "TMPDIR", retVal, true);
        if (!retVal.empty())
            break;

        GetConfigValue(isolate->m_isolate, env, "TMP", retVal, true);
        if (!retVal.empty())
            break;

        GetConfigValue(isolate->m_isolate, env, "TEMP", retVal, true);
        if (!retVal.empty())
            break;

        retVal = "/tmp";
    } while (false);

    path_base::normalize(retVal, retVal);

    if (retVal.length() > 1 && isPathSlash(retVal[retVal.length() - 1]))
        retVal.resize(retVal.length() - 1);

    return 0;
}

result_t process_base::cwd(exlib::string& retVal)
{

#ifdef PATH_MAX
    char buf[PATH_MAX];
#else
    char buf[4096];
#endif

    if (!::getcwd(buf, sizeof(buf)))
        return CHECK_ERROR(LastError());

    retVal = buf;
    return 0;
}

result_t process_base::chdir(exlib::string directory)
{
    if (::chdir(directory.c_str()))
        return CHECK_ERROR(LastError());

    return 0;
}
}

#endif
