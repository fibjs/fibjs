/*
 * Url.h
 *
 *  Created on: Jul 14, 2012
 *      Author: lion
 */

#include "ifs/Url.h"
#include "utf8.h"

#ifndef URL_H_
#define URL_H_

namespace fibjs
{

class Url: public Url_base
{
public:
    Url()
    {
        extMemory(1024);
    }

    Url(const Url &u);

public:
    // object_base
    result_t toString(exlib::string &retVal);

public:
    // Url_base
    virtual result_t parse(const char *url);
    virtual result_t format(v8::Local<v8::Object> args);
    virtual result_t resolve(const char *to, obj_ptr<Url_base> &retVal);
    virtual result_t normalize();
    virtual result_t get_href(exlib::string &retVal);
    virtual result_t get_protocol(exlib::string &retVal);
    virtual result_t get_slashes(int32_t &retVal);
    virtual result_t get_auth(exlib::string &retVal);
    virtual result_t get_username(exlib::string &retVal);
    virtual result_t get_password(exlib::string &retVal);
    virtual result_t get_host(exlib::string &retVal);
    virtual result_t get_hostname(exlib::string &retVal);
    virtual result_t get_port(exlib::string &retVal);
    virtual result_t get_path(exlib::string &retVal);
    virtual result_t get_pathname(exlib::string &retVal);
    virtual result_t get_search(exlib::string &retVal);
    virtual result_t get_query(exlib::string &retVal);
    virtual result_t get_hash(exlib::string &retVal);

public:
    static void parseHost(const char *&url, exlib::string &hostname, exlib::string &port);

private:
    void clear();

    void parseProtocol(const char *&url);
    void put_protocol(exlib::string str);

    void parseAuth(const char *&url);
    void parseHost(const char *&url);
    void parsePath(const char *&url);
    void parseQuery(const char *&url);
    void parseHash(const char *&url);

public:
    inline static void decodeURI(const char *url, int32_t sz, exlib::string &retVal, bool space = false)
    {
        if (sz < 0)
            sz = (int32_t) qstrlen(url);

        if (sz == 0)
            return;

        int32_t len, l;
        const char *src;
        unsigned char ch;
        char *bstr;
        exlib::string str;

        for (len = 0, src = url, l = sz; l > 0; src++, len++, l--)
        {
            ch = (unsigned char) * src;
            if (ch == '%' && l > 2 && qisxdigit(src[1]) && qisxdigit(src[2]))
            {
                src += 2;
                l -= 2;
            }
            else if ((ch == '%' || ch == '\\') && l > 5
                     && (src[1] == 'u' || src[1] == 'U') && qisxdigit(src[2])
                     && qisxdigit(src[3]) && qisxdigit(src[4]) && qisxdigit(src[5]))
            {
                exlib::wchar wch = (qhex(src[2]) << 12) + (qhex(src[3]) << 8)
                                   + (qhex(src[4]) << 4) + qhex(src[5]);

                len += utf8_strlen(&wch, 1) - 1;

                src += 5;
                l -= 5;
            }
        }

        str.resize(len);
        bstr = &str[0];

        for (len = 0, src = url, l = sz; l > 0; src++, len++, l--)
        {
            ch = (unsigned char) * src;

            if (ch == '%' && l > 2 && qisxdigit(src[1]) && qisxdigit(src[2]))
            {
                *bstr++ = (qhex(src[1]) << 4) + qhex(src[2]);
                src += 2;
                l -= 2;
            }
            else if ((ch == '%' || ch == '\\') && l > 5
                     && (src[1] == 'u' || src[1] == 'U') && qisxdigit(src[2])
                     && qisxdigit(src[3]) && qisxdigit(src[4]) && qisxdigit(src[5]))
            {
                exlib::wchar wch = (qhex(src[2]) << 12) + (qhex(src[3]) << 8)
                                   + (qhex(src[4]) << 4) + qhex(src[5]);

                bstr += utf8_wcstombs(&wch, 1, bstr, 5);

                src += 5;
                l -= 5;
            }
            else if (space && ch == '+')
                *bstr++ = ' ';
            else
                *bstr++ = ch;
        }

        retVal = str;
    }

    inline static void encodeURI(const char *url, int32_t sz, exlib::string &retVal,
                                 const char *tab)
    {
        static const char *hex = "0123456789ABCDEF";

        if (sz < 0)
            sz = (int32_t) qstrlen(url);

        if (sz == 0)
            return;

        int32_t len, l;
        const char *src;
        unsigned char ch;
        char *bstr;
        exlib::string str;

        for (len = 0, src = url, l = sz; l > 0; len++, l--)
        {
            ch = (unsigned char) * src++;
            if (ch < 0x20 || ch >= 0x80 || tab[ch - 0x20] == ' ')
                len += 2;
        }

        str.resize(len);
        bstr = &str[0];

        for (src = url, l = sz; l > 0; l--)
        {
            ch = (unsigned char) * src++;
            if (ch >= 0x20 && ch < 0x80 && tab[ch - 0x20] != ' ')
                *bstr++ = ch;
            else
            {
                *bstr++ = '%';

                *bstr++ = hex[(ch >> 4) & 15];
                *bstr++ = hex[ch & 15];
            }
        }

        retVal = str;
    }

public:
    exlib::string m_protocol;
    bool m_slashes;
    bool m_defslashes;
    exlib::string m_username;
    exlib::string m_password;
    exlib::string m_host;
    exlib::string m_hostname;
    exlib::string m_port;
    exlib::string m_pathname;
    exlib::string m_query;
    exlib::string m_hash;
    bool m_ipv6;
};

} /* namespace fibjs */
#endif /* URL_H_ */
