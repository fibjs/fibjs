/*
 * inetAddr.h
 *
 *  Created on: Jul 10, 2012
 *      Author: lion
 */

#pragma once

#include "ifs/net.h"
#include <string.h>
#include <uv/include/uv.h>

namespace fibjs {

union inetAddr {
    struct sockaddr_in addr4;
    struct sockaddr_in6 addr6;

    int32_t size()
    {
        return addr6.sin6_family == PF_INET6 ? (int32_t)sizeof(addr6) : (int32_t)sizeof(addr4);
    }

    int32_t family()
    {
        return addr6.sin6_family == PF_INET6 ? net_base::C_AF_INET6 : net_base::C_AF_INET;
    }

    int32_t port()
    {
        return ntohs(addr4.sin_port);
    }

    void init(int32_t family)
    {
        memset(this, 0, sizeof(inetAddr));

        if (family == net_base::C_AF_INET)
            addr4.sin_family = PF_INET;
        else
            addr6.sin6_family = PF_INET6;
    }

    void init(sockaddr* addr)
    {
        if (addr->sa_family == PF_INET)
            memmove(this, addr, sizeof(addr4));
        else
            memmove(this, addr, sizeof(addr6));
    }

    void setPort(int32_t port)
    {
        addr4.sin_port = htons(port);
    }

    int32_t addr(exlib::string s)
    {
        if (s.empty())
            s = addr6.sin6_family == PF_INET6 ? "::" : "0.0.0.0";

        if (addr6.sin6_family == PF_INET6)
            return uv_ip6_addr(s.c_str(), port(), &addr6);
        else
            return uv_ip4_addr(s.c_str(), port(), &addr4);
    }

    exlib::string str()
    {
        char tmp[INET6_ADDRSTRLEN];

        if (addr6.sin6_family == PF_INET6)
            uv_ip6_name(&addr6, tmp, INET6_ADDRSTRLEN);
        else
            uv_ip4_name(&addr4, tmp, INET6_ADDRSTRLEN);

        return tmp;
    }
};

} /* namespace fibjs */
