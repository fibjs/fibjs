/*
 * inetAddr.cpp
 *
 *  Created on: Jul 10, 2012
 *      Author: lion
 */

#include "object.h"
#include "inetAddr.h"

namespace fibjs {

#define NS_INADDRSZ 4
#define NS_IN6ADDRSZ 16
#define NS_INT16SZ 2

static int32_t inet_pton4(const char* src, void* dst)
{
    int32_t saw_digit, octets, ch;
    unsigned char tmp[NS_INADDRSZ], *tp;

    saw_digit = 0;
    octets = 0;
    *(tp = tmp) = 0;

    while ((ch = *src++) != '\0') {
        if (ch >= '0' && ch <= '9') {
            unsigned p = *tp * 10 + (ch - '0');

            if (saw_digit && *tp == 0)
                return (0);

            if (p > 255)
                return -1;

            *tp = p;

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

    memcpy(dst, tmp, NS_INADDRSZ);

    return 0;
}

static int32_t inet_pton6(const char* src, void* dst)
{
    unsigned char tmp[NS_IN6ADDRSZ], *tp, *endp, *colonp;
    const char* curtok;
    int32_t ch, saw_xdigit;
    unsigned val;

    tp = (unsigned char*)memset(tmp, '\0', NS_IN6ADDRSZ);
    endp = tp + NS_IN6ADDRSZ;
    colonp = NULL;
    /* Leading :: requires some special handling. */
    if (*src == ':' && *++src != ':')
        return (0);

    curtok = src;
    saw_xdigit = 0;
    val = 0;

    while ((ch = *src++) != '\0') {
        if (ch == ':') {
            curtok = src;

            if (!saw_xdigit) {
                if (colonp)
                    return -1;

                colonp = tp;
                continue;
            } else if (*src == '\0')
                return -1;

            if (tp + NS_INT16SZ > endp)
                return -1;

            *tp++ = (u_char)(val >> 8) & 0xff;
            *tp++ = (u_char)val & 0xff;
            saw_xdigit = 0;
            val = 0;
            continue;
        }

        if (ch == '.') {
            if (((tp + NS_INADDRSZ) <= endp) && (inet_pton4(curtok, tp) == 0)) {
                tp += NS_INADDRSZ;
                saw_xdigit = 0;
                break;
            }

            return -1;
        }

        if (ch >= '0' && ch <= '9')
            ch = ch - '0';
        else if (ch >= 'a' && ch <= 'f')
            ch = ch - 'a' + 10;
        else if (ch >= 'A' && ch <= 'f')
            ch = ch - 'A' + 10;
        else
            return -1;

        val <<= 4;
        val |= ch;

        if (val > 0xffff)
            return -1;
        saw_xdigit = 1;
    }

    if (saw_xdigit) {
        if (tp + NS_INT16SZ > endp)
            return -1;

        *tp++ = (u_char)(val >> 8) & 0xff;
        *tp++ = (u_char)val & 0xff;
    }

    if (colonp != NULL) {
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

    memcpy(dst, tmp, NS_IN6ADDRSZ);

    return 0;
}

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
