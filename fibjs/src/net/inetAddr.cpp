/*
 * inetAddr.cpp
 *
 *  Created on: Jul 10, 2012
 *      Author: lion
 */

#include "object.h"
#include "inetAddr.h"

namespace fibjs {

#define MAX_IPv4_STR_LEN 16
#define MAX_IPv6_STR_LEN 64

static const char* hexchars = "0123456789abcdef";

static const char* inet_ntop4(const struct in_addr* addr, char* buf,
    socklen_t len)
{
    const unsigned char* ap = (const unsigned char*)&addr->s_addr;
    char tmp[MAX_IPv4_STR_LEN]; /* max length of ipv4 addr string */
    int32_t fulllen;

    /*
     * snprintf returns number of bytes printed (not including NULL) or
     * number of bytes that would have been printed if more than would
     * fit
     */
    fulllen = sprintf(tmp, "%d.%d.%d.%d", ap[0], ap[1], ap[2], ap[3]);
    if (fulllen >= (int32_t)len) {
        return NULL;
    }

    memcpy(buf, tmp, fulllen + 1);

    return buf;
}

static const char* inet_ntop6(const struct in6_addr* addr, char* dst,
    socklen_t size)
{
    char hexa[8][5], tmp[MAX_IPv6_STR_LEN];
    int32_t zr[8];
    size_t len;
    int32_t i, j, k, skip;
    uint8_t x8, hx8;
    uint16_t x16;
    struct in_addr a4;

    if (addr == NULL)
        return NULL;

    memset(tmp, 0, sizeof(tmp));

    /*  check for mapped or compat addresses */
    i = IN6_IS_ADDR_V4MAPPED(addr);
    j = IN6_IS_ADDR_V4COMPAT(addr);
    if ((i != 0) || (j != 0)) {
        char tmp2[16]; /* max length of ipv4 addr string */
        uint32_t* paddr4 = (uint32_t*)&addr->s6_addr;
        a4.s_addr = paddr4[3];
        len = sprintf(tmp, "::%s%s", (i != 0) ? "ffff:" : "",
            inet_ntop4(&a4, tmp2, sizeof(tmp2)));
        if ((socklen_t)len >= size)
            return NULL;
        memcpy(dst, tmp, len + 1);
        return dst;
    }

    k = 0;
    for (i = 0; i < 16; i += 2) {
        j = 0;
        skip = 1;

        memset(hexa[k], 0, 5);

        x8 = addr->s6_addr[i];

        hx8 = x8 >> 4;
        if (hx8 != 0) {
            skip = 0;
            hexa[k][j++] = hexchars[hx8];
        }

        hx8 = x8 & 0x0f;
        if ((skip == 0) || ((skip == 1) && (hx8 != 0))) {
            skip = 0;
            hexa[k][j++] = hexchars[hx8];
        }

        x8 = addr->s6_addr[i + 1];

        hx8 = x8 >> 4;
        if ((skip == 0) || ((skip == 1) && (hx8 != 0))) {
            hexa[k][j++] = hexchars[hx8];
        }

        hx8 = x8 & 0x0f;
        hexa[k][j++] = hexchars[hx8];

        k++;
    }

    /* find runs of zeros for :: convention */
    j = 0;
    for (i = 7; i >= 0; i--) {
        zr[i] = j;
        x16 = ((uint16_t*)&addr->s6_addr)[i];
        if (x16 == 0)
            j++;
        else
            j = 0;
        zr[i] = j;
    }

    /* find longest run of zeros */
    k = -1;
    j = 0;
    for (i = 0; i < 8; i++) {
        if (zr[i] > j) {
            k = i;
            j = zr[i];
        }
    }

    for (i = 0; i < 8; i++) {
        if (i != k)
            zr[i] = 0;
    }

    len = 0;
    for (i = 0; i < 8; i++) {
        if (zr[i] != 0) {
            /* check for leading zero */
            if (i == 0)
                tmp[len++] = ':';
            tmp[len++] = ':';
            i += (zr[i] - 1);
            continue;
        }
        for (j = 0; hexa[i][j] != '\0'; j++)
            tmp[len++] = hexa[i][j];
        if (i != 7)
            tmp[len++] = ':';
    }

    /* trailing NULL */
    len++;

    if ((socklen_t)len > size)
        return NULL;

    memcpy(dst, tmp, len);
    return dst;
}

int32_t inetAddr::addr(exlib::string s)
{
    if (!*s.c_str())
        return 0;

    return addr6.sin6_family == PF_INET6 ? inet_pton6(s.c_str(), &addr6.sin6_addr) : inet_pton4(s.c_str(), &addr4.sin_addr.s_addr);
}

exlib::string inetAddr::str()
{
    char tmp[MAX_IPv6_STR_LEN];

    if (addr6.sin6_family == PF_INET6)
        inet_ntop6(&addr6.sin6_addr, tmp, MAX_IPv6_STR_LEN);
    else
        inet_ntop4(&addr4.sin_addr, tmp, MAX_IPv6_STR_LEN);

    return tmp;
}

} /* namespace fibjs */
