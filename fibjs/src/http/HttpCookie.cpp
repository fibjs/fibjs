/*
 * HttpCookie.cpp
 *
 *  Created on: Aug 23, 2014
 *      Author: lion
 */

#include "HttpCookie.h"
#include "parse.h"
#include "Url.h"

namespace fibjs
{

result_t HttpCookie_base::_new(v8::Local<v8::Object> opts,
                               obj_ptr<HttpCookie_base> &retVal,
                               v8::Local<v8::Object> This)
{
    obj_ptr<HttpCookie> cookie = new HttpCookie();
    cookie->fill(opts, true);

    retVal = cookie;
    return 0;
}

result_t HttpCookie_base::_new(const char *name, const char *value,
                               v8::Local<v8::Object> opts,
                               obj_ptr<HttpCookie_base> &retVal,
                               v8::Local<v8::Object> This)
{
    obj_ptr<HttpCookie> cookie = new HttpCookie();

    cookie->set_name(name);
    cookie->set_value(value);

    cookie->fill(opts, false);

    retVal = cookie;
    return 0;
}

result_t HttpCookie::parse(const char *header)
{
    _parser p(header);
    qstring key, value;
    qstring tmp;

    p.skipSpace();
    p.getWord(tmp, '=');
    if (!p.want('=') || tmp.empty())
        return CHECK_ERROR(Runtime::setError("HttpCookie: bad cookie format."));
    Url::decodeURI(tmp.c_str(), (int32_t)tmp.length(), m_name);

    p.getWord(tmp, ';');
    Url::decodeURI(tmp.c_str(), (int32_t)tmp.length(), m_value);

    while (p.want(';'))
    {
        p.skipSpace();
        p.getWord(key, ';', '=');

        if (p.want('='))
        {
            p.getString(value, ';');

            if (!qstricmp(key.c_str(), "expires"))
                m_expires.parse(value.c_str());
            else if (!qstricmp(key.c_str(), "domain"))
                m_domain = value;
            else if (!qstricmp(key.c_str(), "path"))
                m_path = value;
        }
        else
        {
            if (!qstricmp(key.c_str(), "secure"))
                m_secure = true;
            else if (!qstricmp(key.c_str(), "HttpOnly"))
                m_httpOnly = true;
        }
    }

    return 0;
}

result_t HttpCookie::fill(v8::Local<v8::Object> opts, bool bBase)
{
    Isolate* isolate = holder();

    if (bBase)
    {
        GetConfigValue(isolate->m_isolate, opts, "name", m_name);
        GetConfigValue(isolate->m_isolate, opts, "value", m_value);
    }

    GetConfigValue(isolate->m_isolate, opts, "expires", m_expires);
    GetConfigValue(isolate->m_isolate, opts, "domain", m_domain);
    GetConfigValue(isolate->m_isolate, opts, "path", m_path);
    GetConfigValue(isolate->m_isolate, opts, "secure", m_secure);
    GetConfigValue(isolate->m_isolate, opts, "httpOnly", m_httpOnly);

    return 0;
}

result_t HttpCookie::match(const char *url, bool &retVal)
{
    obj_ptr<Url> u = new Url();

    result_t hr = u->parse(url);
    if (hr < 0)
        return hr;

    retVal = false;

    if (!m_domain.empty())
    {
        const char *p1, *p2;
        size_t sz = m_domain.length();

        p1 = m_domain.c_str();
        p2 = u->m_host.c_str();

        while (*p1 == '.')
        {
            p1 ++;
            sz --;
        }

        if (*p1)
        {
            if (!qstrchr(p1, '.'))
                return 0;

            if (sz > u->m_host.length())
                return 0;

            if (sz == u->m_host.length())
            {
                if (qstrcmp(p1, p2))
                    return 0;
            }
            else
            {
                p2 += u->m_host.length() - sz - 1;
                if (*p2 != '.' || qstrcmp(p1, p2 + 1))
                    return 0;
            }
        }
    }

    if (!m_path.empty())
    {
        const char *p1, *p2;
        size_t sz = m_path.length();

        p1 = m_path.c_str();
        p2 = u->m_pathname.c_str();

        while (sz && p1[sz - 1] == '/')
            sz --;

        if (sz)
        {
            if (sz > u->m_pathname.length())
                return 0;

            if (sz == u->m_pathname.length())
            {
                if (qstrcmp(p1, p2, (int32_t)sz))
                    return 0;
            }
            else if (p2[sz] != '/' || qstrcmp(p1, p2, (int32_t)sz))
                return 0;
        }
    }

    retVal = true;

    return 0;
}

result_t HttpCookie::get_name(qstring &retVal)
{
    retVal = m_name;
    return 0;
}

result_t HttpCookie::set_name(const char *newVal)
{
    m_name = newVal;
    return 0;
}

result_t HttpCookie::get_value(qstring &retVal)
{
    retVal = m_value;
    return 0;
}

result_t HttpCookie::set_value(const char *newVal)
{
    m_value = newVal;
    return 0;
}

result_t HttpCookie::get_domain(qstring &retVal)
{
    retVal = m_domain;
    return 0;
}

result_t HttpCookie::set_domain(const char *newVal)
{
    m_domain = newVal;
    return 0;
}

result_t HttpCookie::get_path(qstring &retVal)
{
    retVal = m_path;
    return 0;
}

result_t HttpCookie::set_path(const char *newVal)
{
    m_path = newVal;
    return 0;
}

result_t HttpCookie::get_expires(date_t &retVal)
{
    retVal = m_expires;
    return 0;
}

result_t HttpCookie::set_expires(date_t newVal)
{
    m_expires = newVal;
    return 0;
}

result_t HttpCookie::get_httpOnly(bool &retVal)
{
    retVal = m_httpOnly;
    return 0;
}

result_t HttpCookie::set_httpOnly(bool newVal)
{
    m_httpOnly = newVal;
    return 0;
}

result_t HttpCookie::get_secure(bool &retVal)
{
    retVal = m_secure;
    return 0;
}

result_t HttpCookie::set_secure(bool newVal)
{
    m_secure = newVal;
    return 0;
}

static const char *CookieNameTable =
    " ! #$%&'()*+ -./0123456789: < >?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[ ]^_`abcdefghijklmnopqrstuvwxyz{|}~ ";
static const char *CookieTable =
    " ! #$%&'()*+ -./0123456789: <=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[ ]^_`abcdefghijklmnopqrstuvwxyz{|}~ ";

result_t HttpCookie::toString(qstring &retVal)
{
    qstring str;
    qstring tmp;

    Url::encodeURI(m_name.c_str(), (int32_t)m_name.length(), tmp, CookieNameTable);
    str = tmp;
    str += '=';

    Url::encodeURI(m_value.c_str(), (int32_t)m_value.length(), tmp, CookieTable);
    str.append(tmp);

    if (!m_expires.empty())
    {
        m_expires.toGMTString(tmp);

        str.append("; expires=", 10);
        str.append(tmp);
    }

    if (!m_domain.empty())
    {
        str.append("; domain=", 9);
        str.append(m_domain);
    }

    if (!m_path.empty())
    {
        str.append("; path=", 7);
        str.append(m_path);
    }

    if (m_secure)
        str.append("; secure", 8);

    if (m_httpOnly)
        str.append("; HttpOnly", 10);

    retVal = str;
    return 0;
}

}
