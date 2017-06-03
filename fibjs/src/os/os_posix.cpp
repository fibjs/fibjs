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
#include "path.h"
#include "encoding.h"
#include "utils.h"
#include "ifs/process.h"
#include <sys/utsname.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

#ifdef Linux
#include <linux/if_packet.h>
#elif defined(Darwin)
#include <net/if_dl.h>
#endif

namespace fibjs {

result_t os_base::type(exlib::string& retVal)
{
    struct utsname info;

    uname(&info);
    retVal = info.sysname;

    return 0;
}

result_t os_base::release(exlib::string& retVal)
{
    struct utsname info;
    if (uname(&info) < 0) {
        return CHECK_ERROR(LastError());
    }
#ifdef _AIX
    char release[256];
    snprintf(release, sizeof(release),
        "%s.%s", info.version, info.release);
    retVal = release;
#else
    retVal = info.release;
#endif
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

result_t os_base::get_EOL(exlib::string& retVal)
{
    retVal.assign("\n", 1);
    return 0;
}

result_t os_base::networkInterfaces(v8::Local<v8::Object>& retVal)
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
        if (!((ent->ifa_flags & IFF_UP) && (ent->ifa_flags & IFF_RUNNING)) || (ent->ifa_addr == NULL) ||
#ifdef Linux
            (ent->ifa_addr->sa_family != AF_PACKET)) {
#else
            (ent->ifa_addr->sa_family != AF_LINK)) {
#endif
            continue;
        }

        v8::Local<v8::String> name;
        v8::Local<v8::Array> ret;
        unsigned char* ptr;
        char mac[18];

        name = isolate->NewFromUtf8(ent->ifa_name);

        if (!retVal->Has(name)) {
            continue;
        }
        ret = v8::Local<v8::Array>::Cast(retVal->Get(name));

#ifdef Linux
        struct sockaddr_ll* s = (struct sockaddr_ll*)ent->ifa_addr;
        int macAddrlen = 0;
        for (int i = 0; i < 6; i++)
            macAddrlen += sprintf(mac + macAddrlen, "%02X%s", s->sll_addr[i], i < 5 ? ":" : "");
#else
        ptr = (unsigned char*)LLADDR((struct sockaddr_dl*)(ent->ifa_addr));
        sprintf(mac, "%02x:%02x:%02x:%02x:%02x:%02x",
            *ptr, *(ptr + 1), *(ptr + 2), *(ptr + 3), *(ptr + 4), *(ptr + 5));
#endif
        int32_t len = ret->Length();
        for (int i = 0; i < len; i++) {
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

result_t os_base::userInfo(v8::Local<v8::Object> options, v8::Local<v8::Object>& retVal)
{
    Isolate* isolate = Isolate::current();
    retVal = v8::Object::New(isolate->m_isolate);

    exlib::string encoding = "utf8";
    exlib::string homedir;
    exlib::string username;
    exlib::string shell;

    GetConfigValue(isolate->m_isolate, options, "encoding", encoding, true);

    struct passwd pwd;
    struct passwd* result;
    char* buf;
    size_t bufsize;
    int s;
    bufsize = sysconf(_SC_GETPW_R_SIZE_MAX);
    if (bufsize == -1)
        bufsize = 16384;
    buf = (char*)malloc(bufsize);
    if (buf == NULL) {
        return CHECK_ERROR(LastError());
    }
    s = getpwuid_r(geteuid(), &pwd, buf, bufsize, &result);
    if (result == NULL) {
        free(buf);
        return CHECK_ERROR(LastError());
    }
    homedir.append(pwd.pw_dir, strlen(pwd.pw_dir));
    username.append(pwd.pw_name, strlen(pwd.pw_name));
    shell.append(pwd.pw_shell, strlen(pwd.pw_shell));

    free(buf);

    retVal->Set(isolate->NewFromUtf8("uid"), v8::Integer::New(isolate->m_isolate, pwd.pw_uid));
    retVal->Set(isolate->NewFromUtf8("gid"), v8::Integer::New(isolate->m_isolate, pwd.pw_gid));

    if (encoding != "utf8") {
        if (encoding == "buffer") {
            obj_ptr<Buffer_base> usernameBuffer = new Buffer(username);
            obj_ptr<Buffer_base> homedirBuffer = new Buffer(homedir);
            obj_ptr<Buffer_base> shellBuffer = new Buffer(shell);

            retVal->Set(isolate->NewFromUtf8("username"), usernameBuffer->wrap());
            retVal->Set(isolate->NewFromUtf8("homedir"), homedirBuffer->wrap());
            retVal->Set(isolate->NewFromUtf8("shell"), shellBuffer->wrap());
            return 0;
        } else {
            commonEncode(encoding, username, username);
            commonEncode(encoding, homedir, homedir);
            commonEncode(encoding, shell, shell);
        }
    }

    retVal->Set(isolate->NewFromUtf8("username"), isolate->NewFromUtf8(username));
    retVal->Set(isolate->NewFromUtf8("homedir"), isolate->NewFromUtf8(homedir));
    retVal->Set(isolate->NewFromUtf8("shell"), isolate->NewFromUtf8(shell));

    return 0;
}

result_t os_base::homedir(exlib::string& retVal)
{
    Isolate* isolate = Isolate::current();
    v8::Local<v8::Object> env;
    process_base::get_env(env);

    GetConfigValue(isolate->m_isolate, env, "HOME", retVal, true);

    // process.env.HOME does not exist , call getpwuid_r()
    if (retVal.empty()) {
        struct passwd pwd;
        struct passwd* result;
        char* buf;
        size_t bufsize;
        int s;
        bufsize = sysconf(_SC_GETPW_R_SIZE_MAX);
        if (bufsize == -1)
            bufsize = 16384;
        buf = (char*)malloc(bufsize);
        if (buf == NULL) {
            return CHECK_ERROR(LastError());
        }
        s = getpwuid_r(getuid(), &pwd, buf, bufsize, &result);
        if (result == NULL) {
            free(buf);
            return CHECK_ERROR(LastError());
        }
        retVal.append(pwd.pw_dir, strlen(pwd.pw_dir) + 1);
        free(buf);
    }
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
