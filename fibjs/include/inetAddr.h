/*
 * inetAddr.h
 *
 *  Created on: Jul 10, 2012
 *      Author: lion
 */

#include "ifs/net.h"
#include <string.h>

#ifndef INETADDR_H_
#define INETADDR_H_

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
        return addr6.sin6_family == PF_INET6 ? net_base::_AF_INET6 : net_base::_AF_INET;
    }

    int32_t port()
    {
        return ntohs(addr4.sin_port);
    }

    void init(int32_t family)
    {
        memset(this, 0, sizeof(inetAddr));

        if (family == net_base::_AF_INET)
            addr4.sin_family = PF_INET;
        else
            addr6.sin6_family = PF_INET6;
    }

    void setPort(int32_t port)
    {
        addr4.sin_port = htons(port);
    }

    int32_t addr(exlib::string s);
    exlib::string str();
};

inline int32_t inet_pton4(const char* src, void* dst)
{
    static const char digits[] = "0123456789";
    int32_t saw_digit, octets, ch;
    unsigned char tmp[sizeof(struct in_addr)], *tp;

    saw_digit = 0;
    octets = 0;
    *(tp = tmp) = 0;
    while ((ch = *src++) != '\0') {
        const char* pch;

        if ((pch = strchr(digits, ch)) != NULL) {
            uint32_t nw = *tp * 10 + (uint32_t)(pch - digits);

            if (saw_digit && *tp == 0)
                return -1;
            if (nw > 255)
                return -1;
            *tp = nw;
            if (!saw_digit) {
                if (++octets > 4)
                    return -1;
                saw_digit = 1;
            }
        } else if (ch == '.' && saw_digit) {
            if (octets == 4)
                return -1;
            *++tp = 0;
            saw_digit = 0;
        } else
            return -1;
    }
    if (octets < 4)
        return -1;

    memcpy(dst, tmp, sizeof(struct in_addr));

    return 0;
}

inline int32_t inet_pton6(const char* src, void* dst)
{
    static const char xdigits_l[] = "0123456789abcdef",
                      xdigits_u[] = "0123456789ABCDEF";
    unsigned char tmp[sizeof(struct in6_addr)], *tp, *endp, *colonp;
    const char *xdigits, *curtok;
    int32_t ch, seen_xdigits;
    uint32_t val;

    memset((tp = tmp), '\0', sizeof tmp);
    endp = tp + sizeof tmp;
    colonp = NULL;
    /* Leading :: requires some special handling. */
    if (*src == ':')
        if (*++src != ':')
            return -1;
    curtok = src;
    seen_xdigits = 0;
    val = 0;
    while ((ch = *src++) != '\0') {
        const char* pch;

        if ((pch = strchr((xdigits = xdigits_l), ch)) == NULL)
            pch = strchr((xdigits = xdigits_u), ch);
        if (pch != NULL) {
            val <<= 4;
            val |= (pch - xdigits);
            if (++seen_xdigits > 4)
                return -1;
            continue;
        }
        if (ch == ':') {
            curtok = src;
            if (!seen_xdigits) {
                if (colonp)
                    return -1;
                colonp = tp;
                continue;
            } else if (*src == '\0') {
                return -1;
            }
            if (tp + sizeof(uint16_t) > endp)
                return -1;
            *tp++ = (unsigned char)(val >> 8) & 0xff;
            *tp++ = (unsigned char)val & 0xff;
            seen_xdigits = 0;
            val = 0;
            continue;
        }
        if (ch == '.' && ((tp + sizeof(struct in_addr)) <= endp)) {
            int32_t err = inet_pton4(curtok, tp);
            if (err == 0) {
                tp += sizeof(struct in_addr);
                seen_xdigits = 0;
                break; /*%< '\\0' was seen by inet_pton4(). */
            }
        }
        return -1;
    }
    if (seen_xdigits) {
        if (tp + sizeof(uint16_t) > endp)
            return -1;
        *tp++ = (unsigned char)(val >> 8) & 0xff;
        *tp++ = (unsigned char)val & 0xff;
    }
    if (colonp != NULL) {
        /*
        * Since some memmove()'s erroneously fail to handle
        * overlapping regions, we'll do the shift by hand.
        */
        const int32_t n = (int32_t)(tp - colonp);
        int32_t i;

        if (tp == endp)
            return -1;
        for (i = 1; i <= n; i++) {
            endp[-i] = colonp[n - i];
            colonp[n - i] = 0;
        }
        tp = endp;
    }
    if (tp != endp)
        return -1;
    memcpy(dst, tmp, sizeof tmp);
    return 0;
}

} /* namespace fibjs */
#endif /* INETADDR_H_ */
