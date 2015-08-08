/*
 * os_posix.cpp
 *
 *  Created on: Nov 11, 2012
 *      Author: lion
 */

#include <exlib/include/osconfig.h>

#ifndef _WIN32

#include "ifs/os.h"
#include "ifs/process.h"
#include <sys/utsname.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <string.h>

namespace fibjs
{

result_t os_base::get_type(std::string &retVal)
{
    struct utsname info;

    uname(&info);
    retVal = info.sysname;

    return 0;
}

result_t os_base::get_version(std::string &retVal)
{
    struct utsname info;

    uname(&info);
    retVal = info.release;

    return 0;
}

result_t os_base::networkInfo(v8::Local<v8::Object> &retVal)
{
    struct ::ifaddrs *addrs, *ent;
    struct ::sockaddr_in *in4;
    struct ::sockaddr_in6 *in6;
    char ip[INET6_ADDRSTRLEN];

    if (getifaddrs(&addrs) != 0)
        return CHECK_ERROR(LastError());

    Isolate* isolate = Isolate::now();

    retVal = v8::Object::New(isolate->isolate);

    for (ent = addrs; ent != NULL; ent = ent->ifa_next)
    {
        v8::Local<v8::Array> ret;
        v8::Local<v8::Object> o;
        v8::Local<v8::String> name, ipaddr, family;

        bzero(&ip, sizeof(ip));
        if (!(ent->ifa_flags & IFF_UP && ent->ifa_flags & IFF_RUNNING))
            continue;

        if (ent->ifa_addr == NULL)
            continue;

        if (ent->ifa_addr->sa_family != AF_INET6
                && ent->ifa_addr->sa_family != AF_INET)
            continue;

        name = v8::String::NewFromUtf8(isolate->isolate, ent->ifa_name);
        if (retVal->Has(name))
        {
            ret = v8::Local<v8::Array>::Cast(retVal->Get(name));
        }
        else
        {
            ret = v8::Array::New(isolate->isolate);
            retVal->Set(name, ret);
        }

        if (ent->ifa_addr->sa_family == AF_INET6)
        {
            in6 = (struct sockaddr_in6 *) ent->ifa_addr;
            inet_ntop(AF_INET6, &(in6->sin6_addr), ip, INET6_ADDRSTRLEN);
            family = v8::String::NewFromUtf8(isolate->isolate, "IPv6");
        }
        else if (ent->ifa_addr->sa_family == AF_INET)
        {
            in4 = (struct sockaddr_in *) ent->ifa_addr;
            inet_ntop(AF_INET, &(in4->sin_addr), ip, INET6_ADDRSTRLEN);
            family = v8::String::NewFromUtf8(isolate->isolate, "IPv4");
        }

        o = v8::Object::New(isolate->isolate);
        o->Set(v8::String::NewFromUtf8(isolate->isolate, "address"), v8::String::NewFromUtf8(isolate->isolate, ip));
        o->Set(v8::String::NewFromUtf8(isolate->isolate, "family"), family);
        o->Set(v8::String::NewFromUtf8(isolate->isolate, "internal"),
               ent->ifa_flags & IFF_LOOPBACK ? v8::True(isolate->isolate) : v8::False(isolate->isolate));

        ret->Set(ret->Length(), o);
    }

    freeifaddrs(addrs);

    return 0;
}

result_t process_base::cwd(std::string &retVal)
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

result_t process_base::chdir(const char *directory)
{
    if (::chdir(directory))
        return CHECK_ERROR(LastError());

    return 0;
}

}

#endif
