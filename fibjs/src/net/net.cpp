/*
 * net.cpp
 *
 *  Created on: Apr 22, 2012
 *      Author: lion
 */

#include "object.h"
#include "ifs/ssl.h"
#include "ifs/os.h"
#include "Socket.h"
#include "Smtp.h"
#include "Url.h"

#define INET6_ADDRSTRLEN 46

namespace fibjs {

DECLARE_MODULE(net);

inline int inet_pton4(const char *src, unsigned char *dst) {
  static const char digits[] = "0123456789";
  int saw_digit, octets, ch;
  unsigned char tmp[sizeof(struct in_addr)], *tp;

  saw_digit = 0;
  octets = 0;
  *(tp = tmp) = 0;
  while ((ch = *src++) != '\0') {
    const char *pch;

    if ((pch = strchr(digits, ch)) != NULL) {
      unsigned int nw = *tp * 10 + (pch - digits);

      if (saw_digit && *tp == 0)
        return CALL_E_INVALID_CALL;
      if (nw > 255)
        return CALL_E_INVALID_CALL;
      *tp = nw;
      if (!saw_digit) {
        if (++octets > 4)
          return CALL_E_INVALID_CALL;
        saw_digit = 1;
      }
    } else if (ch == '.' && saw_digit) {
      if (octets == 4)
        return CALL_E_INVALID_CALL;
      *++tp = 0;
      saw_digit = 0;
    } else
      return CALL_E_INVALID_CALL;
  }
  if (octets < 4)
    return CALL_E_INVALID_CALL;
  memcpy(dst, tmp, sizeof(struct in_addr));
  return 0;
}

result_t net_base::info(v8::Local<v8::Object>& retVal)
{
    return os_base::networkInterfaces(retVal);
}

result_t net_base::resolve(exlib::string name, int32_t family,
    exlib::string& retVal, AsyncEvent* ac)
{
    if (family != net_base::_AF_INET && family != net_base::_AF_INET6)
        return CHECK_ERROR(CALL_E_INVALIDARG);

    if (!ac)
        return CHECK_ERROR(CALL_E_LONGSYNC);

    inetAddr addr_info;

    addr_info.init(family);

    addrinfo hints = { 0, AF_UNSPEC, SOCK_STREAM, IPPROTO_TCP, 0, 0, 0, 0 };
    addrinfo* result = NULL;
    addrinfo* ptr = NULL;

    if (getaddrinfo(name.c_str(), NULL, &hints, &result))
        return CHECK_ERROR(SocketError());

    for (ptr = result; ptr != NULL; ptr = ptr->ai_next)
        if (ptr->ai_family == addr_info.addr4.sin_family) {
            memcpy(&addr_info, ptr->ai_addr, addr_info.size());
            break;
        }

    freeaddrinfo(result);

    if (ptr == NULL) {
#ifdef _WIN32
        return -WSAHOST_NOT_FOUND;
#else
        return -ETIME;
#endif
    }

    retVal = addr_info.str();

    return 0;
}

result_t net_base::ip(exlib::string name, exlib::string& retVal,
    AsyncEvent* ac)
{
    return resolve(name, net_base::_AF_INET, retVal, ac);
}

result_t net_base::ipv6(exlib::string name, exlib::string& retVal,
    AsyncEvent* ac)
{
    return resolve(name, net_base::_AF_INET6, retVal, ac);
}

result_t net_base::connect(exlib::string host, int32_t port, int32_t timeout, int32_t family,
    obj_ptr<Stream_base>& retVal, AsyncEvent* ac)
{
    if (family != net_base::_AF_INET && family != net_base::_AF_INET6)
        return CHECK_ERROR(CALL_E_INVALIDARG);

    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    result_t hr;
    obj_ptr<Socket_base> socket;

    hr = Socket_base::_new(family, net_base::_SOCK_STREAM, socket);
    if (hr < 0)
        return hr;

    socket->set_timeout(timeout);

    retVal = socket;
    return socket->connect(host, port, ac);
}

result_t net_base::connect(exlib::string url, int32_t timeout, obj_ptr<Stream_base>& retVal,
    AsyncEvent* ac)
{
    if (!qstrcmp(url.c_str(), "ssl:", 4))
        return ssl_base::connect(url, timeout, retVal, ac);

    if (qstrcmp(url.c_str(), "tcp:", 4))
        return CHECK_ERROR(CALL_E_INVALIDARG);

    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    obj_ptr<Url> u = new Url();

    result_t hr = u->parse(url);
    if (hr < 0)
        return hr;

    if (u->m_port.length() == 0)
        return CHECK_ERROR(CALL_E_INVALIDARG);

    int32_t nPort = atoi(u->m_port.c_str());
    return connect(u->m_hostname.c_str(), nPort, timeout,
        u->m_ipv6 ? net_base::_AF_INET6 : net_base::_AF_INET,
        retVal, ac);
}

result_t net_base::openSmtp(exlib::string url, int32_t timeout,
    obj_ptr<Smtp_base>& retVal, AsyncEvent* ac)
{
    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    result_t hr;

    hr = Smtp_base::_new(retVal);
    if (hr < 0)
        return hr;

    retVal->set_timeout(timeout);

    return retVal->connect(url, ac);
}

result_t net_base::isIP(v8::Local<v8::Value> ip, int32_t& retVal)
{
    retVal = 0;
    bool is = false;

    isIPv4(ip, is);

    if (is) {
        retVal = 4;
        return 0;
    }

    isIPv6(ip, is);

    if(is)
        retVal = 6;

    return 0;
}

result_t net_base::isIPv4(v8::Local<v8::Value> ip, bool& retVal)
{
    result_t hr;
    exlib::string cip;

    hr = GetArgumentValue(ip, cip);
    if (hr < 0)
        return CHECK_ERROR(hr);

    retVal = true;
    const char *src = cip.c_str();
    unsigned char tmp[sizeof(struct in6_addr)];
    hr = inet_pton4(src, tmp);
    if(hr != 0)
        retVal = false;
    return 0;
}

result_t net_base::isIPv6(v8::Local<v8::Value> ip, bool& retVal)
{
    result_t hr;
    exlib::string cip;

    hr = GetArgumentValue(ip, cip);
    if (hr < 0)
        return CHECK_ERROR(hr);

    retVal = true;

    const char *s = cip.c_str();

    int len;
    char _tmp[INET6_ADDRSTRLEN], *src, *p;
    src = (char*) s;
    p = strchr(s, '%');
    if (p != NULL) {
        src = _tmp;
        len = p - s;
        if (len > INET6_ADDRSTRLEN - 1)
        {
            retVal = false;
            return 0;
        }
        memcpy(src, s, len);
        src[len] = '\0';
    }
    
    static const char xdigits_l[] = "0123456789abcdef",
                    xdigits_u[] = "0123456789ABCDEF";
    unsigned char tmp[sizeof(struct in6_addr)], *tp, *endp, *colonp;
    const char *xdigits, *curtok;
    int ch, seen_xdigits;
    unsigned int val;

    memset((tp = tmp), '\0', sizeof tmp);
    endp = tp + sizeof tmp;
    colonp = NULL;
    /* Leading :: requires some special handling. */
    if (*src == ':' && *++src != ':')
    {
        retVal = false;
        return 0;
    }
    curtok = src;
    seen_xdigits = 0;
    val = 0;
    while ((ch = *src++) != '\0') {
        const char *pch;

        if ((pch = strchr((xdigits = xdigits_l), ch)) == NULL)
            pch = strchr((xdigits = xdigits_u), ch);
        if (pch != NULL) {
            val <<= 4;
            val |= (pch - xdigits);
            if (++seen_xdigits > 4)
            {
                retVal = false;
                return 0;
            }
            continue;
        }
        if (ch == ':') {
            curtok = src;
            if (!seen_xdigits) {
                if (colonp)
                {
                    retVal = false;
                    return 0;
                }
                colonp = tp;
                continue;
            } else if (*src == '\0') {
                retVal = false;
                return 0;
            }
            if (tp + sizeof(uint16_t) > endp)
            {
                retVal = false;
                return 0;
            }
            *tp++ = (unsigned char) (val >> 8) & 0xff;
            *tp++ = (unsigned char) val & 0xff;
            seen_xdigits = 0;
            val = 0;
            continue;
        }
        if (ch == '.' && ((tp + sizeof(struct in_addr)) <= endp)) {
            int err = inet_pton4(curtok, tp);
            if (err == 0) {
                tp += sizeof(struct in_addr);
                seen_xdigits = 0;
                break;  /*%< '\\0' was seen by inet_pton4(). */
            }
        }
        retVal = false;
        return 0;
    }
    if (seen_xdigits) {
        if (tp + sizeof(uint16_t) > endp)
        {
            retVal = false;
            return 0;
        }
        *tp++ = (unsigned char) (val >> 8) & 0xff;
        *tp++ = (unsigned char) val & 0xff;
    }
    if (colonp != NULL) {
        /*
        * Since some memmove()'s erroneously fail to handle
        * overlapping regions, we'll do the shift by hand.
        */
        const int n = tp - colonp;
        int i;

        if (tp == endp)
        {
            retVal = false;
            return 0;
        }
        for (i = 1; i <= n; i++) {
            endp[- i] = colonp[n - i];
            colonp[n - i] = 0;
        }
        tp = endp;
    }

    if (tp != endp)
        retVal = false;

    return 0;
}
}
