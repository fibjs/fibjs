/*
 * Url.cpp
 *
 *  Created on: Jul 14, 2012
 *      Author: lion
 */

#include "object.h"
#include "Url.h"
#include "ifs/encoding.h"
#include "ifs/url.h"

namespace fibjs
{

DECLARE_MODULE(url);

result_t url_base::format(v8::Local<v8::Object> args, exlib::string& retVal)
{
    obj_ptr<Url> u = new Url();

    result_t hr = u->format(args);
    if (hr < 0)
        return hr;

    return u->get_href(retVal);
}

result_t url_base::parse(exlib::string url, obj_ptr<UrlObject_base>& retVal)
{
    obj_ptr<Url> u = new Url();

    result_t hr = u->parse(url);
    if (hr < 0)
        return hr;

    retVal = u;

    return 0;
}

static const char *pathTable =
    " !  $%& ()*+,-./0123456789:; =  @ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_ abcdefghijklmnopqrstuvwxyz{|}~ ";
static const char *queryTable =
    " !  $%& ()*+,-./0123456789:; = ?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_ abcdefghijklmnopqrstuvwxyz{|}~ ";
static const char *hashTable =
    " ! #$%& ()*+,-./0123456789:; = ?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_ abcdefghijklmnopqrstuvwxyz{|}~ ";

result_t UrlObject_base::_new(exlib::string url, obj_ptr<UrlObject_base> &retVal, v8::Local<v8::Object> This)
{
    obj_ptr<Url> u = new Url();

    result_t hr = u->parse(url);
    if (hr < 0)
        return hr;

    retVal = u;

    return 0;
}

result_t UrlObject_base::_new(v8::Local<v8::Object> args, obj_ptr<UrlObject_base> &retVal,
                              v8::Local<v8::Object> This)
{
    obj_ptr<Url> u = new Url();

    result_t hr = u->format(args);
    if (hr < 0)
        return hr;

    retVal = u;

    return 0;
}

Url::Url(const Url &u)
{
    m_protocol = u.m_protocol;
    m_slashes = u.m_slashes;
    m_defslashes = u.m_defslashes;
    m_username = u.m_username;
    m_password = u.m_password;
    m_host = u.m_host;
    m_hostname = u.m_hostname;
    m_port = u.m_port;
    m_pathname = u.m_pathname;
    m_query = u.m_query;
    m_hash = u.m_hash;
    m_ipv6 = u.m_ipv6;
}

void Url::clear()
{
    m_protocol.clear();
    m_slashes = true;
    m_defslashes = true;
    m_username.clear();
    m_password.clear();
    m_host.clear();
    m_hostname.clear();
    m_port.clear();
    m_pathname.clear();
    m_query.clear();
    m_hash.clear();
    m_ipv6 = false;
}

void Url::parseProtocol(const char *&url)
{
    const char *p = url;
    char ch;

    while ((ch = *p)
            && (qisascii(ch) || qisdigit(ch) || ch == '.' || ch == '+'
                || ch == '-'))
        p++;

    if (ch == ':')
    {
        p++;

        exlib::string str(url, p - url);
        put_protocol(str);
        url = p;
    }
}

void Url::parseAuth(const char *&url)
{
    const char *p1 = url;
    const char *p2 = NULL;
    char ch;

    while ((ch = *p1)
            && (ch != '/' && ch != '@' && ch != '?' && ch != '#' && ch != ':'))
        p1++;

    if (ch == ':')
    {
        p2 = p1 + 1;

        while ((ch = *p2) && (ch != '/' && ch != '@' && ch != '?' && ch != '#'))
            p2++;
    }

    if (ch == '@')
    {
        m_username.assign(url, p1 - url);
        encoding_base::decodeURI(m_username, m_username);
        if (p2)
        {
            m_password.assign(p1 + 1, p2 - p1 - 1);
            encoding_base::decodeURI(m_password, m_password);
        }

        url = p2 ? p2 + 1 : p1 + 1;
    }
}

void Url::parseHost(const char *&url, exlib::string &hostname, exlib::string &port)
{
    const char *p1 = url;
    const char *p2 = NULL;
    char ch;

    if (*p1 == '[')
    {
        p1++;
        while ((ch = *p1) && (qisxdigit(ch) || ch == ':' || ch == '.'))
            p1++;
        if (ch == ']')
            ch = *++p1;
        else
            url++;
    }
    else
    {
        while ((ch = *p1)
                && (qisascii(ch) || qisdigit(ch) || ch == '.' || ch == '_'
                    || ch == '-'))
            p1++;
    }

    if (ch == ':')
    {
        p2 = p1 + 1;

        while ((ch = *p2) && qisdigit(ch))
            p2++;
    }

    if (*url == '[')
        hostname.assign(url + 1, p1 - url - 2);
    else
        hostname.assign(url, p1 - url);

    if (hostname.length() > 0)
        qstrlwr(&hostname[0]);
    if (p2)
        port.assign(p1 + 1, p2 - p1 - 1);
    else
        port.clear();

    url = p2 ? p2 : p1;
}

void Url::parseHost(const char *&url)
{
    const char *p0 = url;

    while (true)
    {
        parseHost(url, m_hostname, m_port);
        m_ipv6 = qstrchr(m_hostname.c_str(), ':') != NULL;

        if (*url != ',')
            break;

        url++;
    }

    if (url > p0)
    {
        if (*(url - 1) == ':')
            m_host.assign(p0, url - p0 - 1);
        else
            m_host.assign(p0, url - p0);
        qstrlwr(&m_host[0]);
    }
}

void Url::parsePath(const char *&url)
{
    const char *p = url;
    char ch;

    while ((ch = *p) && ch != '?' && ch != '#')
        p++;

    if (isUrlSlash(*url) || m_hostname.length() == 0 || !m_defslashes)
        m_pathname.assign(url, p - url);
    else
    {
        m_pathname.assign(1, URL_SLASH);
        m_pathname.append(url, p - url);
    }

    if (m_protocol.compare("javascript:"))
        Url::encodeURI(m_pathname, m_pathname, pathTable);
    url = p;
}

void Url::parseQuery(const char *&url)
{
    if (*url != '?')
        return;

    const char *p = ++url;
    char ch;

    while ((ch = *p) && ch != '#')
        p++;

    m_query.assign(url, p - url);
    Url::encodeURI(m_query, m_query, queryTable);
    url = p;
}

void Url::parseHash(const char *&url)
{
    if (*url != '#')
        return;

    Url::encodeURI(url, -1, m_hash, hashTable);
}

result_t Url::parse(exlib::string url)
{
    bool bHost;
    clear();

    const char* c_str = url.c_str();

    parseProtocol(c_str);

    bHost = !m_slashes;
    m_slashes = (isUrlSlash(*c_str) && isUrlSlash(c_str[1]));
    if (m_slashes)
        c_str += 2;

    if (!m_protocol.compare("javascript:"))
    {
        m_slashes = false;
        bHost = false;
    }
    else if (m_slashes || bHost)
    {
        parseAuth(c_str);
        parseHost(c_str);
    }

    parsePath(c_str);
    parseQuery(c_str);
    parseHash(c_str);

    return 0;
}

exlib::string getValue(Isolate* isolate, v8::Local<v8::Object> &args, const char *key)
{
    exlib::string s;

    v8::Local<v8::Value> v = args->Get(isolate->NewFromUtf8(key));

    if (!v.IsEmpty() && v->IsString())
        s = *v8::String::Utf8Value(v);

    return s;
}

result_t Url::format(v8::Local<v8::Object> args)
{
    clear();

    Isolate* isolate = holder();

    put_protocol(getValue(isolate, args, "protocol"));
    m_username = getValue(isolate, args, "username");
    m_password = getValue(isolate, args, "password");

    m_host = getValue(isolate, args, "host");
    m_hostname = getValue(isolate, args, "hostname");
    m_port = getValue(isolate, args, "port");

    m_pathname = getValue(isolate, args, "pathname");
    if (m_pathname.length() > 0 && !isUrlSlash(m_pathname[0])
            && m_hostname.length() > 0)
        m_pathname = URL_SLASH + m_pathname;

    m_query = getValue(isolate, args, "query");

    m_hash = getValue(isolate, args, "hash");
    if (m_hash.length() > 0 && m_hash[0] != '#')
        m_hash = '#' + m_hash;

    if (m_slashes && m_protocol.compare("file:") && m_hostname.length() == 0)
        m_slashes = false;

    v8::Local<v8::Value> v = args->Get(holder()->NewFromUtf8("slashes"));

    if (!IsEmpty(v))
        m_slashes = v->BooleanValue();

    m_ipv6 = qstrchr(m_hostname.c_str(), ':') != NULL;

    if (!m_hostname.empty() && m_host.empty())
    {
        if (m_ipv6)
            m_host.append(1, '[');

        m_host.append(m_hostname);

        if (m_ipv6)
            m_host.append(1, ']');

        if (m_port.length() > 0)
        {
            m_host.append(1, ':');
            m_host.append(m_port);
        }
    }

    return 0;
}

result_t Url::resolve(exlib::string url, obj_ptr<UrlObject_base> &retVal)
{
    obj_ptr<Url> u = new Url();

    result_t hr = u->parse(url);
    if (hr < 0)
        return hr;

    obj_ptr<Url> base = new Url(*this);

    if (u->m_hostname.length() > 0 || u->m_slashes
            || (u->m_protocol.length() > 0 && u->m_protocol.compare(m_protocol)))
    {
        if (u->m_protocol.length())
            base->m_protocol = u->m_protocol;
        base->m_slashes = u->m_slashes;
        base->m_defslashes = u->m_defslashes;
        base->m_username = u->m_username;
        base->m_password = u->m_password;
        base->m_host = u->m_host;
        base->m_hostname = u->m_hostname;
        base->m_port = u->m_port;
        base->m_pathname = u->m_pathname;
        base->m_query = u->m_query;
        base->m_hash = u->m_hash;
        base->m_ipv6 = u->m_ipv6;

        base->normalize();
    }
    else if (u->m_pathname.length())
    {
        if (isUrlSlash(u->m_pathname[0]))
            base->m_pathname = u->m_pathname;
        else
        {
            if (!isUrlSlash(m_pathname[m_pathname.length() - 1]))
                base->m_pathname.append("/../", 4);
            base->m_pathname.append(u->m_pathname);
        }

        base->normalize();

        base->m_query = u->m_query;
        base->m_hash = u->m_hash;
    }
    else if (u->m_query.length())
    {
        base->m_query = u->m_query;
        base->m_hash = u->m_hash;
    }
    else if (u->m_hash.length())
        base->m_hash = u->m_hash;

    retVal = base;

    return 0;
}

result_t Url::normalize()
{
    if (m_pathname.length() == 0)
        return 0;

    exlib::string str;
    const char *p1 = m_pathname.c_str();
    char *pstr;
    int32_t pos = 0;
    int32_t root = 0;
    bool bRoot = false;

    str.resize(m_pathname.length());
    pstr = &str[0];

    if (isUrlSlash(p1[0]))
    {
        pstr[pos++] = URL_SLASH;
        p1++;
        bRoot = true;
    }

    root = pos;

    while (*p1)
    {
        if (isUrlSlash(p1[0]))
        {
            p1++;
        }
        else if (p1[0] == '.' && (!p1[1] || isUrlSlash(p1[1])))
        {
            p1 += p1[1] ? 2 : 1;
        }
        else if ((p1[0] == '.') && (p1[1] == '.')
                 && (!p1[2] || isUrlSlash(p1[2])))
        {
            if (pos > root)
            {
                if ((pstr[pos - 2] == '.') && (pstr[pos - 3] == '.')
                        && ((root == pos - 3) || (pstr[pos - 4] == URL_SLASH)))
                {
                    pstr[pos++] = '.';
                    pstr[pos++] = '.';
                    pstr[pos++] = URL_SLASH;
                }
                else
                {
                    pos--;
                    while (pos > root && !isUrlSlash(pstr[pos - 1]))
                        pos--;
                }
            }
            else if (!bRoot)
            {
                pstr[pos++] = '.';
                pstr[pos++] = '.';
                pstr[pos++] = URL_SLASH;
            }

            p1 += p1[2] ? 3 : 2;
        }
        else
        {
            while (*p1 && !isUrlSlash(*p1))
                pstr[pos++] = *p1++;
            if (*p1)
            {
                p1++;
                pstr[pos++] = URL_SLASH;
            }
        }
    }

    str.resize(pos);

    m_pathname = str;

    return 0;
}

result_t Url::toString(exlib::string &retVal)
{
    return get_href(retVal);
}

result_t Url::get_href(exlib::string &retVal)
{
    if (m_protocol.length() > 0)
        retVal.append(m_protocol);

    if (m_slashes)
        retVal.append("//", 2);

    if (m_username.length() > 0)
    {
        get_auth(retVal);
        retVal.append(1, '@');
    }

    get_host(retVal);
    get_path(retVal);
    retVal.append(m_hash);

    return 0;
}

void Url::put_protocol(exlib::string str)
{
    static const char *s_slashed[] =
    {
        "http:", "https:", "ftp:", "gopher:", "file:", "http:", "https:", "ftp:",
        "gopher:", "file:"
    };
    int32_t i;

    m_protocol = str;
    m_defslashes = false;
    if (m_protocol.length() > 0)
    {
        qstrlwr(&m_protocol[0]);

        if (m_protocol[m_protocol.length() - 1] != ':')
            m_protocol.append(1, ':');

        for (i = 0; i < (int32_t) (sizeof(s_slashed) / sizeof(const char *)); i++)
            if (!m_protocol.compare(s_slashed[i]))
            {
                m_defslashes = true;
                break;
            }
    }
    else
        m_defslashes = true;

    m_slashes = m_defslashes;
}

result_t Url::get_protocol(exlib::string &retVal)
{
    retVal = m_protocol;
    return 0;
}

result_t Url::get_slashes(int32_t &retVal)
{
    retVal = m_slashes;
    return 0;
}

result_t Url::get_auth(exlib::string &retVal)
{
    exlib::string str;

    encoding_base::encodeURIComponent(m_username, str);
    retVal.append(str);
    if (m_password.length() > 0)
    {
        retVal.append(1, ':');
        encoding_base::encodeURIComponent(m_password, str);
        retVal.append(str);
    }

    return 0;
}

result_t Url::get_username(exlib::string &retVal)
{
    retVal = m_username;
    return 0;
}

result_t Url::get_password(exlib::string &retVal)
{
    retVal = m_password;
    return 0;
}

result_t Url::get_host(exlib::string &retVal)
{
    retVal.append(m_host);
    return 0;
}

result_t Url::get_hostname(exlib::string &retVal)
{
    retVal = m_hostname;
    return 0;
}

result_t Url::get_port(exlib::string &retVal)
{
    retVal = m_port;
    return 0;
}

result_t Url::get_path(exlib::string &retVal)
{
    retVal.append(m_pathname);
    get_search(retVal);

    return 0;
}

result_t Url::get_pathname(exlib::string &retVal)
{
    retVal = m_pathname;
    return 0;
}

result_t Url::get_search(exlib::string &retVal)
{
    if (m_query.length() > 0)
    {
        retVal.append(1, '?');
        retVal.append(m_query);
    }

    return 0;
}

result_t Url::get_query(exlib::string &retVal)
{
    retVal = m_query;
    return 0;
}

result_t Url::get_hash(exlib::string &retVal)
{
    retVal = m_hash;
    return 0;
}

} /* namespace fibjs */
