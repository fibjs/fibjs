/*
 * url.cpp
 *
 *  Created on: Nov 9, 2024
 *      Author: lion
 */

#include "object.h"
#include "Url.h"
#include "path.h"
#include "ifs/url.h"
#include "Buffer.h"

namespace fibjs {

DECLARE_MODULE(url);

static const char* pathToTable = " !  $ & ()*+,-./0123456789:; =  @ABCDEFGHIJKLMNOPQRSTUVWXYZ[ ]^_ abcdefghijklmnopqrstuvwxyz{|}~ ";
static ada::url_aggregator s_base;

result_t url_base::format(v8::Local<v8::Object> args, exlib::string& retVal)
{
    obj_ptr<Url> u = new Url();

    result_t hr = u->format(args);
    if (hr < 0)
        return hr;

    return u->get_href(retVal);
}

result_t url_base::format(exlib::string href, exlib::string& retVal)
{
    ada::result<ada::url_aggregator> _url = ada::parse(href, &s_base);
    if (!_url || !_url->validate())
        return Runtime::setError("url: Invalid URL '" + href + "'.");

    retVal = _url->get_href();

    return 0;
}

result_t url_base::parse(exlib::string url, bool parseQueryString,
    bool slashesDenoteHost, obj_ptr<UrlObject_base>& retVal)
{
    obj_ptr<Url> u = new Url();

    result_t hr = u->legacy_parse(url, parseQueryString);
    if (hr < 0)
        return hr;

    retVal = u;

    return 0;
}

result_t url_base::resolve(exlib::string _from, exlib::string to,
    exlib::string& retVal)
{
    ada::result<ada::url_aggregator> _base;

    if (!_from.empty()) {
        _base = ada::parse(_from.c_str(), &s_base);
        if (!_base) {
            return Runtime::setError("url: Invalid URL '" + _from + "'.");
        }
    }

    ada::result<ada::url_aggregator> _url = ada::parse(to.c_str(), _base ? &_base.value() : nullptr);
    if (!_url || !_url->validate()) {
        return Runtime::setError("url: Invalid URL '" + to + "'.");
    }

    retVal = _url->get_href();

    return 0;
}

result_t url_base::fileURLToPath(UrlObject_base* url, v8::Local<v8::Object> options, exlib::string& retVal)
{
    obj_ptr<Url> u = (Url*)url;

    if (!u->m_url || u->m_url->type != ada::scheme::FILE)
        return Runtime::setError("url: '" + exlib::string(u->m_url->get_href()) + "' is not a file URL.");

#ifdef _WIN32
    bool isWindows = true;
#else
    bool isWindows = false;
#endif

    if (!options.IsEmpty())
        GetConfigValue(options, "windows", isWindows, true);

    std::string_view pathname = u->m_url->get_pathname();

    if (isWindows) {

        size_t first_percent = std::string::npos;
        size_t pathname_size = pathname.size();
        std::string pathname_escaped_slash;

        for (size_t i = 0; i < pathname_size; i++) {
            if (pathname[i] == URL_SLASH) {
                pathname_escaped_slash += PATH_SLASH_WIN32;
            } else {
                pathname_escaped_slash += pathname[i];
            }

            if (pathname[i] != '%')
                continue;

            if (first_percent == std::string::npos) {
                first_percent = i;
            }

            if ((i + 2) >= pathname_size)
                continue;

            char third = pathname[i + 2] | 0x20;

            bool is_slash = pathname[i + 1] == '2' && third == 102;
            bool is_forward_slash = pathname[i + 1] == '5' && third == 99;

            if (!is_slash && !is_forward_slash)
                continue;

            return Runtime::setError("url: File URL path must not include encoded / characters");
        }

        std::string_view hostname = u->m_url->get_hostname();
        std::string decoded_pathname = ada::unicode::percent_decode(
            std::string_view(pathname_escaped_slash), first_percent);

        if (hostname.size() > 0) {
            if (hostname.size() == 1) {
                char letter = hostname[0] | 0x20;
                if (letter >= 'a' && letter <= 'z') {
                    retVal = exlib::string(1, hostname[0]) + ":" + decoded_pathname;
                    return 0;
                }
            }
            retVal = "\\\\" + ada::idna::to_unicode(hostname) + decoded_pathname;
            return 0;
        }

        char letter = decoded_pathname[1] | 0x20;
        char sep = decoded_pathname[2];

        if (letter < 'a' || letter > 'z' || sep != ':')
            return Runtime::setError("url: File URL path must be absolute");

        retVal = decoded_pathname.substr(1);
    } else {
        std::string_view hostname = u->m_url->get_hostname();

        if (hostname.size() > 0)
            return Runtime::setError("url: the hostname of file URL '" + exlib::string(u->m_url->get_href()) + "' is not empty.");

        size_t first_percent = std::string::npos;
        for (size_t i = 0; (i + 2) < pathname.size(); i++) {
            if (pathname[i] != '%')
                continue;

            if (first_percent == std::string::npos) {
                first_percent = i;
            }

            if (pathname[i + 1] == '2' && (pathname[i + 2] | 0x20) == 102)
                return Runtime::setError("url: file URL path must not include encoded / characters");
        }

        retVal = ada::unicode::percent_decode(pathname, first_percent);
    }

    return 0;
}

result_t url_base::fileURLToPath(exlib::string url, v8::Local<v8::Object> options, exlib::string& retVal)
{
    obj_ptr<Url> u = new Url();

    result_t hr = u->parse(url);
    if (hr < 0)
        return hr;

    return fileURLToPath(u, options, retVal);
}

result_t url_base::pathToFileURL(exlib::string path, v8::Local<v8::Object> options, obj_ptr<UrlObject_base>& retVal)
{
    exlib::string resolved(path);

#ifdef _WIN32
    bool isWindows = true;
#else
    bool isWindows = false;
#endif

    if (!options.IsEmpty())
        GetConfigValue(options, "windows", isWindows, true);

    exlib::string domain;

    if (isWindows) {
        const char* p1 = resolved.c_str();
        if (p1[0] == PATH_SLASH_WIN32 && p1[1] == PATH_SLASH_WIN32) {
            p1 += 2;

            const char* p2 = p1;
            while (*p2 && *p2 != PATH_SLASH_WIN32)
                p2++;

            if (p2 == p1 || *p2 != PATH_SLASH_WIN32)
                return CALL_E_INVALID_DATA;

            domain.assign(p1, p2 - p1);

            resolved = resolved.substr(p2 - p1 + 2);
        } else {
            _resolve_win32(resolved);
            resolved = PATH_SLASH_WIN32 + resolved;
        }

        char* p = resolved.data();
        for (size_t i = 0; i < resolved.length(); i++)
            if (p[i] == PATH_SLASH_WIN32)
                p[i] = URL_SLASH;

        if (isWin32PathSlash(path[path.length() - 1])
            && resolved[resolved.length() - 1] != URL_SLASH)
            resolved.append(1, URL_SLASH);
    } else {
        _resolve(resolved);

        if (isPosixPathSlash(path[path.length() - 1])
            && resolved[resolved.length() - 1] != URL_SLASH)
            resolved.append(1, URL_SLASH);
    }

    exlib::string pathname;
    Url::encodeURI(resolved, pathname, pathToTable);

    if (isWindows)
        pathname = ada::idna::to_ascii(domain) + pathname;

    return parse("file://" + pathname, false, false, retVal);
}

result_t url_base::domainToASCII(exlib::string domain, exlib::string& retVal)
{
    retVal = ada::idna::to_ascii(domain);
    return 0;
}

result_t url_base::domainToUnicode(exlib::string domain, exlib::string& retVal)
{
    retVal = ada::idna::to_unicode(domain);
    return 0;
}

result_t url_base::format(UrlObject_base* urlObject, v8::Local<v8::Object> options, exlib::string& retVal)
{
    // Temporary minimal implementation for compilation
    return urlObject->get_href(retVal);
}

} /* namespace fibjs */
