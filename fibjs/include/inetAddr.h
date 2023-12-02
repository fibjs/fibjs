/*
 * inetAddr4.h
 *
 *  Created on: Jul 10, 2012
 *      Author: lion
 */

#pragma once

#include "ifs/net.h"
#include <string.h>
#include <uv/include/uv.h>

#ifdef _WIN32
struct sockaddr_un {
    short sun_family; /* AF_UNIX */
    char sun_path[108]; /* Pathname */
};
#define AF_UNIX 1
#else
#include <sys/un.h>
#endif

namespace fibjs {

union inetAddr {
    struct sockaddr_in addr4;
    struct sockaddr_in6 addr6;
    struct sockaddr_un addrun;

    int32_t size()
    {
        switch (addr4.sin_family) {
        case AF_UNIX:
            return (int32_t)sizeof(addrun);
        case AF_INET:
            return (int32_t)sizeof(addr4);
        case AF_INET6:
            return (int32_t)sizeof(addr6);
        }
        return 0;
    }

    int32_t family()
    {
        switch (addr4.sin_family) {
        case AF_UNIX:
            return net_base::C_AF_UNIX;
        case AF_INET:
            return net_base::C_AF_INET;
        case AF_INET6:
            return net_base::C_AF_INET6;
        }
        return 0;
    }

    int32_t port()
    {
        if (addrun.sun_family == AF_UNIX)
            return 0;

        return ntohs(addr4.sin_port);
    }

    void init(int32_t family)
    {
        memset(this, 0, sizeof(inetAddr));

        switch (family) {
        case net_base::C_AF_UNIX:
            addrun.sun_family = AF_UNIX;
            break;
        case net_base::C_AF_INET:
            addr4.sin_family = AF_INET;
            break;
        case net_base::C_AF_INET6:
            addr6.sin6_family = AF_INET6;
            break;
        };
    }

    void init(sockaddr* addr)
    {
        addr4.sin_family = addr->sa_family;
        memmove(this, addr, size());
    }

    void setPort(int32_t port)
    {
        if (addrun.sun_family != AF_UNIX)
            addr4.sin_port = htons(port);
    }

    int32_t addr(exlib::string s)
    {
        switch (addr4.sin_family) {
        case AF_UNIX:
            if (s.empty())
                s = "";
            strncpy(addrun.sun_path, s.c_str(), sizeof(addrun.sun_path));
            return 0;
        case AF_INET:
            if (s.empty())
                s = "0.0.0.0";
            return uv_ip4_addr(s.c_str(), port(), &addr4);
        case AF_INET6:
            if (s.empty())
                s = "::";
            return uv_ip6_addr(s.c_str(), port(), &addr6);
        }

        return 0;
    }

    exlib::string str()
    {
        char tmp[INET6_ADDRSTRLEN];

        switch (addr4.sin_family) {
        case AF_UNIX:
            return addrun.sun_path;
        case AF_INET:
            uv_ip4_name(&addr4, tmp, INET6_ADDRSTRLEN);
            break;
        case AF_INET6:
            uv_ip6_name(&addr6, tmp, INET6_ADDRSTRLEN);
            break;
        }

        return tmp;
    }
};

inline int32_t get_port(const char* str)
{
    int32_t n = 0;

    if (!str)
        return 0;

    if (*str < '1' || *str > '9')
        return -1;

    n = *str++ - '0';
    while (*str >= '0' && *str <= '9')
        n = n * 10 + *str++ - '0';

    if (*str)
        return -1;

    return n;
}

} /* namespace fibjs */
