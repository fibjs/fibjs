/*
 * Url.cpp
 *
 *  Created on: Jul 14, 2012
 *      Author: lion
 */

#include "object.h"
#include "Url.h"
#include "parse.h"
#include "path.h"
#include "ifs/encoding.h"
#include "ifs/url.h"
#include "ifs/punycode.h"
#include "ifs/querystring.h"

namespace fibjs {

DECLARE_MODULE(url);

static const char* pathTable = " !  $%& ()*+,-./0123456789:; =  @ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_ abcdefghijklmnopqrstuvwxyz{|}~ ";
static const char* queryTable = " !  $%& ()*+,-./0123456789:; = ?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_ abcdefghijklmnopqrstuvwxyz{|}~ ";
static const char* hashTable = " ! #$%& ()*+,-./0123456789:; = ?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_ abcdefghijklmnopqrstuvwxyz{|}~ ";

#ifdef _WIN32
static const char* pathToTable = " !  $ & ()*+,-./0123456789:; =  @ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_ abcdefghijklmnopqrstuvwxyz{|}~ ";
#else
static const char* pathToTable = " !  $ & ()*+,-./0123456789:; =  @ABCDEFGHIJKLMNOPQRSTUVWXYZ[ ]^_ abcdefghijklmnopqrstuvwxyz{|}~ ";
#endif

result_t url_base::format(v8::Local<v8::Object> args, exlib::string& retVal)
{
    obj_ptr<Url> u = new Url();

    result_t hr = u->format(args);
    if (hr < 0)
        return hr;

    return u->get_href(retVal);
}

result_t url_base::parse(exlib::string url, bool parseQueryString,
    bool slashesDenoteHost, obj_ptr<UrlObject_base>& retVal)
{
    obj_ptr<Url> u = new Url();

    result_t hr = u->parse(url, parseQueryString, slashesDenoteHost);
    if (hr < 0)
        return hr;

    retVal = u;

    return 0;
}

result_t url_base::resolve(exlib::string _from, exlib::string to,
    exlib::string& retVal)
{
    obj_ptr<Url> u = new Url();
    result_t hr = u->parse(_from, false, false);
    if (hr < 0)
        return hr;

    obj_ptr<UrlObject_base> u1;
    hr = u->resolve(to, u1);
    if (hr < 0)
        return hr;

    return u1->toString(retVal);
}

#ifdef _WIN32
result_t url_base::fileURLToPath(UrlObject_base* url, exlib::string& retVal)
{
    obj_ptr<Url> u = (Url*)url;

    if (u->m_protocol != "file:")
        return CALL_E_INVALID_DATA;

    const char* p = u->m_pathname.c_str();

    for (size_t i = 0; i < u->m_pathname.length(); i++)
        if (p[i] == '%') {
            if (p[i + 1] == '2' && (p[i + 2] == 'f' || p[i + 2] == 'F'))
                return CALL_E_INVALID_DATA;
            if (p[i + 1] == '5' && (p[i + 2] == 'c' || p[i + 2] == 'C'))
                return CALL_E_INVALID_DATA;
        }

    exlib::string pathname;
    encoding_base::decodeURI(u->m_pathname, pathname);

    char* p1 = pathname.c_buffer();
    for (size_t i = 0; i < pathname.length(); i++)
        if (isPathSlash(p1[i]))
            p1[i] = PATH_SLASH;

    if (u->m_hostname.length() > 0)
        retVal = exlib::string("\\\\") + u->m_hostname + pathname;
    else {
        p = pathname.c_str();
        if (!qisascii(p[1]) || p[2] != ':')
            return CALL_E_INVALID_DATA;

        retVal = pathname.substr(1);
    }

    return 0;
}
#else
result_t url_base::fileURLToPath(UrlObject_base* url, exlib::string& retVal)
{
    obj_ptr<Url> u = (Url*)url;

    if (u->m_protocol != "file:")
        return CALL_E_INVALID_DATA;

    if (u->m_hostname.length() > 0)
        return CALL_E_INVALID_DATA;

    const char* p = u->m_pathname.c_str();

    for (size_t i = 0; i < u->m_pathname.length(); i++)
        if (p[i] == '%') {
            if (p[i + 1] == '2' && (p[i + 2] == 'f' || p[i + 2] == 'F'))
                return CALL_E_INVALID_DATA;
        }

    encoding_base::decodeURI(u->m_pathname, retVal);

    return 0;
}
#endif

result_t url_base::fileURLToPath(exlib::string url, exlib::string& retVal)
{
    obj_ptr<Url> u = new Url();

    result_t hr = u->parse(url, false, false);
    if (hr < 0)
        return hr;

    return fileURLToPath(u, retVal);
}

result_t url_base::pathToFileURL(exlib::string path, obj_ptr<UrlObject_base>& retVal)
{
    obj_ptr<Url> u = new Url();
    exlib::string resolved(path);

    u->m_protocol.assign("file:", 5);
    u->m_slashes = true;

#ifdef _WIN32
    const char* p1 = resolved.c_str();
    if (p1[0] == PATH_SLASH && p1[1] == PATH_SLASH) {
        p1 += 2;

        const char* p2 = p1;
        while (*p2 && *p2 != PATH_SLASH)
            p2++;

        if (p2 == p1 || *p2 != PATH_SLASH)
            return CALL_E_INVALID_DATA;

        exlib::string domain(p1, p2 - p1);

        u->m_host = domain;
        resolved = resolved.substr(p2 - p1 + 2);
    } else
        _resolve_win32(resolved);

    char* p = resolved.c_buffer();
    for (size_t i = 0; i < resolved.length(); i++)
        if (isPathSlash(p[i]))
            p[i] = URL_SLASH;
#else
    _resolve(resolved);
#endif

    if (isPathSlash(path.c_str()[path.length() - 1])
        && resolved.c_str()[resolved.length() - 1] != URL_SLASH)
        resolved.append(1, URL_SLASH);

    Url::encodeURI(resolved, u->m_pathname, pathToTable);

    retVal = u;

    return 0;
}

result_t UrlObject_base::_new(exlib::string url, bool parseQueryString,
    bool slashesDenoteHost,
    obj_ptr<UrlObject_base>& retVal,
    v8::Local<v8::Object> This)
{
    obj_ptr<Url> u = new Url();

    result_t hr = u->parse(url, parseQueryString, slashesDenoteHost);
    if (hr < 0)
        return hr;

    retVal = u;

    return 0;
}

result_t UrlObject_base::_new(v8::Local<v8::Object> args, obj_ptr<UrlObject_base>& retVal,
    v8::Local<v8::Object> This)
{
    obj_ptr<Url> u = new Url();

    result_t hr = u->format(args);
    if (hr < 0)
        return hr;

    retVal = u;

    return 0;
}

Url::Url(const Url& u)
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

void Url::parseProtocol(const char*& url)
{
    const char* p = url;
    char ch;

    while ((ch = *p)
        && (qisascii(ch) || qisdigit(ch) || ch == '.' || ch == '+'
            || ch == '-'))
        p++;

    if (ch == ':') {
        p++;

        exlib::string str(url, p - url);
        set_protocol(str);
        url = p;
    }
}

void Url::parseAuth(const char*& url)
{
    const char* p1 = url;
    const char* p2 = NULL;
    char ch;

    while ((ch = *p1)
        && (ch != URL_SLASH && ch != '@' && ch != '?' && ch != '#' && ch != ':'))
        p1++;

    if (ch == ':') {
        p2 = p1 + 1;

        while ((ch = *p2) && (ch != URL_SLASH && ch != '@' && ch != '?' && ch != '#'))
            p2++;
    }

    if (ch == '@') {
        m_username.assign(url, p1 - url);
        encoding_base::decodeURI(m_username, m_username);
        if (p2) {
            m_password.assign(p1 + 1, p2 - p1 - 1);
            encoding_base::decodeURI(m_password, m_password);
        }

        url = p2 ? p2 + 1 : p1 + 1;
    }
}

void Url::parseHost(const char*& url, exlib::string& hostname, exlib::string& port)
{
    const char* p1 = url;
    const char* p2 = NULL;
    char ch;

    if (*p1 == '[') {
        p1++;
        while ((ch = *p1) && (qisxdigit(ch) || ch == ':' || ch == '.'))
            p1++;
        if (ch == ']')
            ch = *++p1;
        else
            url++;
    } else if (*p1 == '%' && p1[1] == '2' && (p1[2] == 'f' || p1[2] == 'F')) {
        while ((ch = *p1)
            && (qisascii(ch) || qisdigit(ch) || ch == '.' || ch == '_' || ch == '-' || ch == '%' || ch < 0))
            p1++;
    } else {
        while ((ch = *p1)
            && (qisascii(ch) || qisdigit(ch) || ch == '.' || ch == '_' || ch == '-' || ch < 0))
            p1++;
    }

    if (ch == ':') {
        p2 = p1 + 1;

        while ((ch = *p2) && qisdigit(ch))
            p2++;
    }

    if (*url == '[')
        hostname.assign(url + 1, p1 - url - 2);
    else if (*url == '%') {
        hostname.assign(url, p1 - url);
        encoding_base::decodeURI(hostname, hostname);
    } else
        hostname.assign(url, p1 - url);

    if (hostname.length() > 0) {
        qstrlwr(hostname.c_buffer());
        punycode_base::toASCII(hostname, hostname);
    }

    if (p2)
        port.assign(p1 + 1, p2 - p1 - 1);
    else
        port.clear();

    url = p2 ? p2 : p1;
}

void Url::parseHost(const char*& url)
{
    const char* p0 = url;

    while (true) {
        parseHost(url, m_hostname, m_port);
        m_ipv6 = qstrchr(m_hostname.c_str(), ':') != NULL;

        if (*url != ',')
            break;

        url++;
    }

    if (url > p0) {
        if (m_hostname.length() > 0) {
            if (m_ipv6) {
                m_host.append("[");
                m_host.append(m_hostname);
                m_host.append("]");
            } else
                m_host.append(m_hostname);
        }

        if (m_port.length() > 0) {
            m_host.append(":");
            m_host.append(m_port);
        }
    }
}

void Url::parsePath(const char*& url)
{
    const char* p = url;
    char ch;

    while ((ch = *p) && ch != '?' && ch != '#')
        p++;

    if (isUrlSlash(*url) || m_hostname.length() == 0 || !m_defslashes)
        m_pathname.assign(url, p - url);
    else {
        m_pathname.assign(1, URL_SLASH);
        m_pathname.append(url, p - url);
    }

    if (m_protocol.compare("javascript:"))
        Url::encodeURI(m_pathname, m_pathname, pathTable);
    url = p;
}

void Url::parseQuery(const char*& url)
{
    if (*url != '?')
        return;

    const char* p = ++url;
    char ch;

    while ((ch = *p) && ch != '#')
        p++;

    m_query.assign(url, p - url);
    Url::encodeURI(m_query, m_query, queryTable);
    url = p;
}

void Url::parseHash(const char*& url)
{
    if (*url != '#')
        return;

    Url::encodeURI(url, -1, m_hash, hashTable);
}

void Url::trimUrl(exlib::string url, exlib::string& retVal)
{
    exlib::string rest;
    int32_t start = -1;
    int32_t end = -1;
    int32_t lastPos = 0;
    int32_t i;
    bool isWs;
    unsigned char* _url = (unsigned char*)url.c_buffer();

    bool inWs = false;

    for (i = 0; i < (int32_t)url.length(); i++) {
        isWs = _url[i] == 32 || _url[i] == 9 || _url[i] == 13 || _url[i] == 10 || _url[i] == 12;

        if (_url[i] == 0xc2 && _url[i + 1] == 0xa0) {
            isWs = true;
            i++;
        }
        if (_url[i] == 239 && _url[i + 1] == 187 && _url[i + 2] == 191) {
            isWs = true;
            i += 2;
        }

        if (start == -1) {
            if (isWs)
                continue;
            lastPos = start = i;
        } else {
            if (inWs) {
                if (!isWs) {
                    end = -1;
                    inWs = false;
                }
            } else if (isWs) {
                end = i;
                inWs = true;
            }
        }
        if (_url[i] == 92 && i - lastPos > 0)
            _url[i] = URL_SLASH;
    }

    if (start != -1) {
        if (lastPos == start) {
            if (end == -1) {
                if (start == 0)
                    rest = url;
                else
                    rest = url.substr(start);
            } else {
                rest = url.substr(start, end - start);
            }
        } else if (end == -1 && lastPos < (int32_t)url.length()) {
            // We converted some backslashes and have only part of the entire string
            rest = url.substr(lastPos);
        } else if (end != -1 && lastPos < end) {
            // We converted some backslashes and have only part of the entire string
            rest = url.substr(lastPos, end);
        }
    }
    retVal = rest;
}

bool Url::checkHost(const char* str)
{
    _parser parser(str);
    int32_t p, p1;

    if (parser.get() != URL_SLASH)
        return false;
    parser.skip();
    if (parser.get() != URL_SLASH)
        return false;
    parser.skip();

    p = parser.pos;
    if (parser.get() == '@' || parser.get() == URL_SLASH)
        return false;
    parser.skip();
    parser.skipUntil('@');
    if (parser.get() != '@')
        return false;

    p1 = parser.pos;
    if (p1 + 1 == parser.sz)
        return false;
    parser.pos = p;
    parser.skipUntil(URL_SLASH);
    if (parser.get() == URL_SLASH && parser.pos <= p1 + 1)
        return false;

    parser.pos = p1 + 1;
    if (parser.get() == '@')
        return false;

    return true;
}

result_t Url::parse(exlib::string url, bool parseQueryString, bool slashesDenoteHost)
{
    bool bHost;
    clear();
    m_slashes = false;

    trimUrl(url, url);
    const char* c_str = url.c_str();
    bool hasHash = qstrchr(c_str, '#') != NULL;

    if (!slashesDenoteHost && !hasHash && isUrlSlash(*c_str)) {
        parsePath(c_str);
        parseQuery(c_str);
        parseHash(c_str);

        if (parseQueryString) {
            m_queryParsed = new HttpCollection();
            m_queryParsed->parse(m_query);
        }

        return 0;
    }

    parseProtocol(c_str);

    bHost = checkHost(c_str);

    if (slashesDenoteHost || m_protocol.length() > 0 || bHost)
        m_slashes = ((isUrlSlash(*c_str) && isUrlSlash(c_str[1])) && (m_protocol.length() <= 0 || m_protocol.compare("javascript:")));

    if (m_protocol.compare("javascript:") && m_slashes) {
        c_str += 2;
        parseAuth(c_str);
        parseHost(c_str);
    }

    parsePath(c_str);
    parseQuery(c_str);
    parseHash(c_str);

    if (parseQueryString) {
        m_queryParsed = new HttpCollection();
        m_queryParsed->parse(m_query);
    }

    return 0;
}

result_t Url::format(v8::Local<v8::Object> args)
{
    clear();

    Isolate* isolate = holder();
    v8::Local<v8::Context> context = isolate->context();

    exlib::string str;
    JSValue v;

    if (GetConfigValue(isolate, args, "protocol", str, true) >= 0)
        set_protocol(str);

    if (GetConfigValue(isolate, args, "username", str, true) >= 0)
        set_username(str);
    if (GetConfigValue(isolate, args, "password", str, true) >= 0)
        set_password(str);

    if (GetConfigValue(isolate, args, "host", str, true) >= 0)
        set__host(str);

    if (GetConfigValue(isolate, args, "hostname", str, true) >= 0)
        set_hostname(str);
    if (GetConfigValue(isolate, args, "port", str) >= 0)
        set_port(str);

    if (GetConfigValue(isolate, args, "pathname", str, true) >= 0)
        set_pathname(str);

    v = args->Get(context, holder()->NewString("query"));
    if (!IsEmpty(v))
        set_query(v);

    if (GetConfigValue(isolate, args, "hash", str, true) >= 0)
        set_hash(str);

    if (m_slashes && m_protocol.compare("file:") && m_hostname.length() == 0)
        m_slashes = false;

    GetConfigValue(isolate, args, "slashes", m_slashes, true);

    return 0;
}

result_t Url::resolve(obj_ptr<Url>& u, obj_ptr<UrlObject_base>& retVal)
{
    obj_ptr<Url> base = new Url(*this);

    if (u->m_hostname.length() > 0 || u->m_slashes
        || (u->m_protocol.length() > 0 && u->m_protocol.compare(m_protocol))) {
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
    } else if (u->m_pathname.length()) {
        if (isUrlSlash(u->m_pathname[0]))
            base->m_pathname = u->m_pathname;
        else {
            if (!isUrlSlash(base->m_pathname[base->m_pathname.length() - 1]))
                base->m_pathname.append("/../", 4);
            base->m_pathname.append(u->m_pathname);
        }

        base->normalize();

        base->m_query = u->m_query;
        base->m_hash = u->m_hash;
    } else if (u->m_query.length()) {
        base->m_query = u->m_query;
        base->m_hash = u->m_hash;
    } else if (u->m_hash.length())
        base->m_hash = u->m_hash;

    retVal = base;

    return 0;
}

result_t Url::resolve(exlib::string url, obj_ptr<UrlObject_base>& retVal)
{
    obj_ptr<Url> u = new Url();

    result_t hr = u->parse(url);
    if (hr < 0)
        return hr;

    return resolve(u, retVal);
}

result_t Url::normalize()
{
    if (m_pathname.length() == 0)
        return 0;

    exlib::string str;
    const char* p1 = m_pathname.c_str();
    char* pstr;
    int32_t pos = 0;
    int32_t root = 0;
    bool bRoot = false;

    str.resize(m_pathname.length());
    pstr = str.c_buffer();

    if (isUrlSlash(p1[0])) {
        pstr[pos++] = URL_SLASH;
        p1++;
        bRoot = true;
    }

    root = pos;

    while (*p1) {
        if (isUrlSlash(p1[0])) {
            p1++;
        } else if (p1[0] == '.' && (!p1[1] || isUrlSlash(p1[1]))) {
            p1 += p1[1] ? 2 : 1;
        } else if ((p1[0] == '.') && (p1[1] == '.')
            && (!p1[2] || isUrlSlash(p1[2]))) {
            if (pos > root) {
                if ((pstr[pos - 2] == '.') && (pstr[pos - 3] == '.')
                    && ((root == pos - 3) || (pstr[pos - 4] == URL_SLASH))) {
                    pstr[pos++] = '.';
                    pstr[pos++] = '.';
                    pstr[pos++] = URL_SLASH;
                } else {
                    pos--;
                    while (pos > root && !isUrlSlash(pstr[pos - 1]))
                        pos--;
                }
            } else if (!bRoot) {
                pstr[pos++] = '.';
                pstr[pos++] = '.';
                pstr[pos++] = URL_SLASH;
            }

            p1 += p1[2] ? 3 : 2;
        } else {
            while (*p1 && !isUrlSlash(*p1))
                pstr[pos++] = *p1++;
            if (*p1) {
                p1++;
                pstr[pos++] = URL_SLASH;
            }
        }
    }

    str.resize(pos);

    m_pathname = str;

    return 0;
}

result_t Url::toString(exlib::string& retVal)
{
    return get_href(retVal);
}

result_t Url::get_href(exlib::string& retVal)
{
    retVal.clear();

    if (m_protocol.length() > 0)
        retVal.append(m_protocol);

    if (m_slashes)
        retVal.append(2, URL_SLASH);

    exlib::string str;

    if (m_username.length() > 0) {
        get_auth(str);
        retVal.append(str);
        retVal.append(1, '@');
    }

    get__host(str);
    if (str[0] == URL_SLASH)
        encoding_base::encodeURIComponent(str, str);
    retVal.append(str);

    get_path(str);

#ifdef _WIN32
    if (m_slashes && qisascii(str[0]) && str[1] == ':')
        retVal.append(1, URL_SLASH);
#endif

    retVal.append(str);

    retVal.append(m_hash);

    return 0;
}

result_t Url::set_href(exlib::string newVal)
{
    return parse(newVal, m_queryParsed != NULL, false);
}

result_t Url::get_protocol(exlib::string& retVal)
{
    retVal = m_protocol;
    return 0;
}

result_t Url::set_protocol(exlib::string newVal)
{
    static const char* s_slashed[] = {
        "http:", "https:", "ftp:", "gopher:", "file:", "http:", "https:", "ftp:",
        "gopher:", "file:"
    };
    int32_t i;

    m_protocol = newVal;
    m_defslashes = false;
    if (m_protocol.length() > 0) {
        qstrlwr(m_protocol.c_buffer());

        if (m_protocol[m_protocol.length() - 1] != ':')
            m_protocol.append(1, ':');

        for (i = 0; i < (int32_t)(sizeof(s_slashed) / sizeof(const char*)); i++)
            if (!m_protocol.compare(s_slashed[i])) {
                m_defslashes = true;
                break;
            }
    } else
        m_defslashes = true;

    m_slashes = m_defslashes;
    return 0;
}

result_t Url::get_slashes(bool& retVal)
{
    retVal = m_slashes;
    return 0;
}

result_t Url::set_slashes(bool newVal)
{
    m_slashes = newVal;
    return 0;
}

result_t Url::get_auth(exlib::string& retVal)
{
    exlib::string str;

    encoding_base::encodeURIComponent(m_username, str);
    retVal = str;
    if (m_password.length() > 0) {
        retVal.append(1, ':');
        encoding_base::encodeURIComponent(m_password, str);
        retVal.append(str);
    }

    return 0;
}

result_t Url::set_auth(exlib::string newVal)
{
    // m_auth = newVal;
    return 0;
}

result_t Url::get_username(exlib::string& retVal)
{
    retVal = m_username;
    return 0;
}

result_t Url::set_username(exlib::string newVal)
{
    m_username = newVal;
    return 0;
}

result_t Url::get_password(exlib::string& retVal)
{
    retVal = m_password;
    return 0;
}

result_t Url::set_password(exlib::string newVal)
{
    m_password = newVal;
    return 0;
}

result_t Url::get__host(exlib::string& retVal)
{
    retVal = m_host;
    return 0;
}

result_t Url::set__host(exlib::string newVal)
{
    const char* url = newVal.c_str();
    parseHost(url);
    return 0;
}

result_t Url::get_hostname(exlib::string& retVal)
{
    retVal = m_hostname;
    return 0;
}

void Url::build_host()
{
    if (!m_hostname.empty()) {
        m_host.clear();

        if (m_ipv6)
            m_host.append(1, '[');

        m_host.append(m_hostname);

        if (m_ipv6)
            m_host.append(1, ']');

        if (m_port.length() > 0) {
            m_host.append(1, ':');
            m_host.append(m_port);
        }
    }
}

result_t Url::set_hostname(exlib::string newVal)
{
    m_hostname = newVal;
    m_ipv6 = qstrchr(m_hostname.c_str(), ':') != NULL;
    build_host();

    return 0;
}

result_t Url::get_port(exlib::string& retVal)
{
    retVal = m_port;
    return 0;
}

result_t Url::set_port(exlib::string newVal)
{
    m_port = newVal;
    build_host();

    return 0;
}

result_t Url::get_path(exlib::string& retVal)
{
    exlib::string str;

    get_search(str);
    retVal = m_pathname + str;

    return 0;
}

result_t Url::set_path(exlib::string newVal)
{
    return 0;
}

result_t Url::get_pathname(exlib::string& retVal)
{
    retVal = m_pathname;
    return 0;
}

result_t Url::set_pathname(exlib::string newVal)
{
    m_pathname = newVal;
    if (m_pathname.length() > 0 && !isUrlSlash(m_pathname[0])
        && m_hostname.length() > 0)
        m_pathname = URL_SLASH + m_pathname;

    return 0;
}

result_t Url::get_search(exlib::string& retVal)
{
    if (m_query.length() > 0) {
        retVal.assign(1, '?');
        retVal.append(m_query);
    }

    return 0;
}

result_t Url::set_search(exlib::string newVal)
{
    return 0;
}

result_t Url::get_query(v8::Local<v8::Value>& retVal)
{
    if (m_queryParsed)
        retVal = m_queryParsed->wrap();
    else
        retVal = GetReturnValue(holder(), m_query);

    return 0;
}

result_t Url::get_searchParams(obj_ptr<HttpCollection_base>& retVal)
{
    if (!m_queryParsed) {
        m_queryParsed = new HttpCollection();
        m_queryParsed->parse(m_query);
    }

    retVal = m_queryParsed;

    return 0;
}

result_t Url::set_query(v8::Local<v8::Value> newVal)
{
    result_t hr;

    if (!newVal.IsEmpty()) {
        if (newVal->IsString() || newVal->IsStringObject()) {
            Isolate* isolate = holder();
            m_query = isolate->toString(newVal);

            if (m_queryParsed) {
                m_queryParsed = new HttpCollection();
                hr = m_queryParsed->parse(m_query);
                if (hr < 0)
                    return hr;
            }
        } else {
            obj_ptr<HttpCollection> queryParsed = (HttpCollection*)HttpCollection_base::getInstance(newVal);
            if (!queryParsed) {
                if (newVal->IsObject()) {
                    hr = querystring_base::stringify(v8::Local<v8::Object>::Cast(newVal), "&", "=", v8::Local<v8::Object>(), m_query);
                    if (hr < 0)
                        return hr;

                    if (m_queryParsed) {
                        m_queryParsed = new HttpCollection();
                        hr = m_queryParsed->parse(m_query);
                        if (hr < 0)
                            return hr;
                    }
                } else
                    return CHECK_ERROR(CALL_E_INVALIDARG);
            }
            m_queryParsed = queryParsed;
        }
    }

    return 0;
}

result_t Url::get_hash(exlib::string& retVal)
{
    retVal = m_hash;
    return 0;
}

result_t Url::set_hash(exlib::string newVal)
{
    m_hash = newVal;
    if (m_hash.length() > 0 && m_hash[0] != '#')
        m_hash = '#' + m_hash;

    return 0;
}

} /* namespace fibjs */
