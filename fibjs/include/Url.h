/*
 * Url.h
 *
 *  Created on: Jul 14, 2012
 *      Author: lion
 */

#pragma once

#include "ifs/UrlObject.h"
#include "utf8.h"
#include "HttpCollection.h"
#include "URLSearchParams.h"
#include <ada.h>

namespace fibjs {

class Url : public UrlObject_base {
public:
    Url()
    {
        extMemory(1024);
    }

    Url(const Url& src)
    {
        m_parseQuery = src.m_parseQuery;
        m_url = src.m_url;
        extMemory(1024);
    }

public:
    // object_base
    virtual result_t toString(exlib::string& retVal);
    virtual result_t toJSON(exlib::string key, v8::Local<v8::Value>& retVal);

public:
    // UrlObject_base
    virtual result_t resolve(exlib::string to, obj_ptr<UrlObject_base>& retVal);
    virtual result_t get_href(exlib::string& retVal);
    virtual result_t set_href(exlib::string newVal);
    virtual result_t get_protocol(exlib::string& retVal);
    virtual result_t set_protocol(exlib::string newVal);
    virtual result_t get_slashes(bool& retVal);
    virtual result_t set_slashes(bool newVal);
    virtual result_t get_origin(exlib::string& retVal);
    virtual result_t get_auth(exlib::string& retVal);
    virtual result_t get_username(exlib::string& retVal);
    virtual result_t set_username(exlib::string newVal);
    virtual result_t get_password(exlib::string& retVal);
    virtual result_t set_password(exlib::string newVal);
    virtual result_t get__host(exlib::string& retVal);
    virtual result_t set__host(exlib::string newVal);
    virtual result_t get_hostname(exlib::string& retVal);
    virtual result_t set_hostname(exlib::string newVal);
    virtual result_t get_port(exlib::string& retVal);
    virtual result_t set_port(exlib::string newVal);
    virtual result_t get_path(exlib::string& retVal);
    virtual result_t get_pathname(exlib::string& retVal);
    virtual result_t set_pathname(exlib::string newVal);
    virtual result_t get_search(exlib::string& retVal);
    virtual result_t set_search(exlib::string newVal);
    virtual result_t get_query(v8::Local<v8::Value>& retVal);
    virtual result_t set_query(v8::Local<v8::Value> newVal);
    virtual result_t get_hash(exlib::string& retVal);
    virtual result_t set_hash(exlib::string newVal);
    virtual result_t get_searchParams(obj_ptr<URLSearchParams_base>& retVal);

public:
    result_t legacy_parse(exlib::string url, bool parseQueryString);
    result_t format(v8::Local<v8::Object> args);
    result_t parse(exlib::string url, exlib::string base = "");
    result_t parse_search_params();

public:
    exlib::string href()
    {
        exlib::string str;
        get_href(str);
        return str;
    }

    exlib::string protocol() const
    {
        return m_url ? m_url->get_protocol() : std::string_view();
    }

    exlib::string username() const
    {
        return m_url ? m_url->get_username() : std::string_view();
    }

    exlib::string password() const
    {
        return m_url ? m_url->get_password() : std::string_view();
    }

    exlib::string host() const
    {
        return m_url ? m_url->get_host() : std::string_view();
    }

    exlib::string hostname() const
    {
        return m_url ? m_url->get_hostname() : std::string_view();
    }

    exlib::string port() const
    {
        return m_url ? m_url->get_port() : std::string_view();
    }

    exlib::string pathname() const
    {
        return m_url ? m_url->get_pathname() : std::string_view();
    }

    exlib::string search() const
    {
        return m_url ? m_url->get_search() : std::string_view();
    }

    exlib::string hash() const
    {
        return m_url ? m_url->get_hash() : std::string_view();
    }

    bool isIPv6() const
    {
        return m_url ? m_url->host_type == ada::url_host_type::IPV6 : false;
    }

public:
    inline static void decodeURI(const char* url, ssize_t sz, exlib::string& retVal, bool space = false)
    {
        if (sz < 0)
            sz = qstrlen(url);

        if (sz == 0) {
            retVal.resize(0);
            return;
        }

        ssize_t len, l;
        const char* src;
        unsigned char ch;
        char* bstr;
        exlib::string str;

        for (len = 0, src = url, l = sz; l > 0; src++, len++, l--) {
            ch = (unsigned char)*src;
            if (ch == '%' && l > 2 && qisxdigit(src[1]) && qisxdigit(src[2])) {
                src += 2;
                l -= 2;
            } else if ((ch == '%' || ch == '\\') && l > 5
                && (src[1] == 'u' || src[1] == 'U') && qisxdigit(src[2])
                && qisxdigit(src[3]) && qisxdigit(src[4]) && qisxdigit(src[5])) {
                char16_t wch = (qhex(src[2]) << 12) + (qhex(src[3]) << 8)
                    + (qhex(src[4]) << 4) + qhex(src[5]);

                len += utf8_strlen(&wch, 1) - 1;

                src += 5;
                l -= 5;
            }
        }

        str.resize(len);
        bstr = str.data();

        for (len = 0, src = url, l = sz; l > 0; src++, len++, l--) {
            ch = (unsigned char)*src;

            if (ch == '%' && l > 2 && qisxdigit(src[1]) && qisxdigit(src[2])) {
                *bstr++ = (qhex(src[1]) << 4) + qhex(src[2]);
                src += 2;
                l -= 2;
            } else if ((ch == '%' || ch == '\\') && l > 5
                && (src[1] == 'u' || src[1] == 'U') && qisxdigit(src[2])
                && qisxdigit(src[3]) && qisxdigit(src[4]) && qisxdigit(src[5])) {
                char16_t wch = (qhex(src[2]) << 12) + (qhex(src[3]) << 8)
                    + (qhex(src[4]) << 4) + qhex(src[5]);

                bstr += utf_convert(&wch, 1, bstr, 5);

                src += 5;
                l -= 5;
            } else if (space && ch == '+')
                *bstr++ = ' ';
            else
                *bstr++ = ch;
        }

        retVal = str;
    }

    inline static void decodeURI(exlib::string url, exlib::string& retVal, bool space = false)
    {
        decodeURI(url.c_str(), url.length(), retVal, space);
    }

    inline static void encodeURI(const char* url, ssize_t sz, exlib::string& retVal,
        const char* tab, bool space = false)
    {
        static const char* hex = "0123456789ABCDEF";

        if (sz < 0)
            sz = qstrlen(url);

        if (sz == 0) {
            retVal.resize(0);
            return;
        }

        ssize_t len, l;
        const char* src;
        unsigned char ch;
        char* bstr;
        exlib::string str;

        for (len = 0, src = url, l = sz; l > 0; len++, l--) {
            ch = (unsigned char)*src++;
            if (ch < 0x20 || ch >= 0x80 || tab[ch - 0x20] == ' ') {
                if (space && ch == ' ') {
                    // Space will be encoded as +, no extra length needed
                } else {
                    len += 2;
                }
            }
        }

        str.resize(len);
        bstr = str.data();

        for (src = url, l = sz; l > 0; l--) {
            ch = (unsigned char)*src++;
            if (ch >= 0x20 && ch < 0x80 && tab[ch - 0x20] != ' ') {
                // Special handling for space in form encoded mode
                if (space && ch == ' ') {
                    *bstr++ = '+';
                } else {
                    *bstr++ = ch;
                }
            } else {
                // Special handling for space in form encoded mode
                if (space && ch == ' ') {
                    *bstr++ = '+';
                } else {
                    *bstr++ = '%';
                    *bstr++ = hex[(ch >> 4) & 15];
                    *bstr++ = hex[ch & 15];
                }
            }
        }

        retVal = str;
    }

    inline static void encodeURI(exlib::string url, exlib::string& retVal,
        const char* tab, bool space = false)
    {
        encodeURI(url.c_str(), url.length(), retVal, tab, space);
    }

public:
    static bool isIPv4(exlib::string ip)
    {
        sockaddr_in dst;
        return uv_ip4_addr(ip.c_str(), 0, &dst) == 0;
    }

    static bool isIPv6(exlib::string ip)
    {
        sockaddr_in6 dst;
        return uv_ip6_addr(ip.c_str(), 0, &dst) == 0;
    }

public:
    bool m_parseQuery = false;
    bool m_slashes = false;
    bool m_isLegacy = false;
    bool m_hasAuthSeparator = false; // Track if @ symbol was found
    exlib::string m_originalAuth; // Store original auth for legacy mode
    ada::result<ada::url_aggregator> m_url;
    obj_ptr<URLSearchParams> m_searchParams;
};

} /* namespace fibjs */
