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
#include "union_helpers.h"

namespace fibjs {

DECLARE_MODULE(url);

static const char* pathToTable = " !  $ & ()*+,-./0123456789:; =  @ABCDEFGHIJKLMNOPQRSTUVWXYZ[ ]^_ abcdefghijklmnopqrstuvwxyz{|}~ ";
static ada::url_aggregator s_base;

result_t url_base::format(v8::Local<v8::Object> args, exlib::string& retVal)
{
    obj_ptr<Url> u = new Url();

    result_t hr = u->format(args);
    if (hr.failed())
        return hr;

    return u->get_href(retVal);
}

result_t url_base::format(exlib::string href, exlib::string& retVal)
{
    ada::result<ada::url_aggregator> _url = ada::parse(href, &s_base);
    if (!_url || !_url->validate())
        return result_t::make(CALL_E_INVALID_DATA).with_type_name(errtype::kTypeError).with_code("ERR_INVALID_URL").with_message("url: Invalid URL '" + href + "'.");

    retVal = _url->get_href();

    return result_t();
}

result_t url_base::parse(exlib::string url, bool parseQueryString,
    bool slashesDenoteHost, obj_ptr<UrlObject_base>& retVal)
{
    obj_ptr<Url> u = new Url();

    result_t hr = u->legacy_parse(url, parseQueryString);
    if (hr.failed())
        return hr;

    retVal = u;

    return result_t();
}

result_t url_base::resolve(exlib::string _from, exlib::string to,
    exlib::string& retVal)
{
    ada::result<ada::url_aggregator> _base;

    if (!_from.empty()) {
        _base = ada::parse(_from.c_str(), &s_base);
        if (!_base) {
            return result_t::make(CALL_E_INVALID_DATA).with_type_name(errtype::kTypeError).with_code("ERR_INVALID_URL").with_message("url: Invalid URL '" + _from + "'.");
        }
    }

    ada::result<ada::url_aggregator> _url = ada::parse(to.c_str(), _base ? &_base.value() : nullptr);
    if (!_url || !_url->validate()) {
        return result_t::make(CALL_E_INVALID_DATA).with_type_name(errtype::kTypeError).with_code("ERR_INVALID_URL").with_message("url: Invalid URL '" + to + "'.");
    }

    retVal = _url->get_href();

    return result_t();
}

result_t url_base::fileURLToPath(Union_fileURLToPath_url url, v8::Local<v8::Object> options, exlib::string& retVal)
{
    // a string is parsed into a URL object first
    if (std::holds_alternative<exlib::string>(url)) {
        obj_ptr<Url> parsed = new Url();

        result_t hr = parsed->parse(std::get<exlib::string>(url));
        if (hr.failed())
            return hr;

        return fileURLToPath(parsed, options, retVal);
    }

    // a UrlObject or a URL components object: both resolve to a UrlObject
    obj_ptr<UrlObject_base> obj;
    result_t obj_hr = urlobject_from_union(url, obj);
    if (obj_hr.failed())
        return obj_hr;

    obj_ptr<Url> u = (Url*)obj.get();

    if (!u->m_url || u->m_url->type != ada::scheme::FILE)
        return result_t::make(CALL_E_INVALID_DATA).with_type_name(errtype::kTypeError).with_code("ERR_INVALID_URL_SCHEME").with_message("url: '" + exlib::string(u->m_url->get_href()) + "' is not a file URL.");

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

            return result_t::make(CALL_E_INVALID_DATA).with_type_name(errtype::kTypeError).with_code("ERR_INVALID_FILE_URL_PATH").with_message("url: File URL path must not include encoded / characters");
        }

        std::string_view hostname = u->m_url->get_hostname();
        std::string decoded_pathname = ada::unicode::percent_decode(
            std::string_view(pathname_escaped_slash), first_percent);

        if (hostname.size() > 0) {
            if (hostname.size() == 1) {
                char letter = hostname[0] | 0x20;
                if (letter >= 'a' && letter <= 'z') {
                    retVal = exlib::string(1, hostname[0]) + ":" + decoded_pathname;
                    return result_t();
                }
            }
            retVal = "\\\\" + ada::idna::to_unicode(hostname) + decoded_pathname;
            return result_t();
        }

        char letter = decoded_pathname[1] | 0x20;
        char sep = decoded_pathname[2];

        if (letter < 'a' || letter > 'z' || sep != ':')
            return result_t::make(CALL_E_INVALID_DATA).with_type_name(errtype::kTypeError).with_code("ERR_INVALID_FILE_URL_PATH").with_message("url: File URL path must be absolute");

        retVal = decoded_pathname.substr(1);
    } else {
        std::string_view hostname = u->m_url->get_hostname();

        if (hostname.size() > 0)
            return result_t::make(CALL_E_INVALID_DATA).with_type_name(errtype::kTypeError).with_code("ERR_INVALID_FILE_URL_HOST").with_message("url: the hostname of file URL '" + exlib::string(u->m_url->get_href()) + "' is not empty.");

        size_t first_percent = std::string::npos;
        for (size_t i = 0; (i + 2) < pathname.size(); i++) {
            if (pathname[i] != '%')
                continue;

            if (first_percent == std::string::npos) {
                first_percent = i;
            }

            if (pathname[i + 1] == '2' && (pathname[i + 2] | 0x20) == 102)
                return result_t::make(CALL_E_INVALID_DATA).with_type_name(errtype::kTypeError).with_code("ERR_INVALID_FILE_URL_PATH").with_message("url: file URL path must not include encoded / characters");
        }

        retVal = ada::unicode::percent_decode(pathname, first_percent);
    }

    return result_t();
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
                return result_t::invalid_data();

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
    return result_t();
}

result_t url_base::domainToUnicode(exlib::string domain, exlib::string& retVal)
{
    retVal = ada::idna::to_unicode(domain);
    return result_t();
}

result_t url_base::format(Union_format_urlObject urlObject, v8::Local<v8::Object> options, exlib::string& retVal)
{
    // A string input is already formatted; Node ignores the options in that case
    // and returns the text as it is, so only a URL/UrlObject input is rewritten.
    bool from_object = std::holds_alternative<obj_ptr<UrlObject_base>>(urlObject)
        || std::holds_alternative<v8::Local<v8::Object>>(urlObject);

    obj_ptr<UrlObject_base> u;
    result_t hr = urlobject_from_union(urlObject, u);
    if (hr.failed())
        return hr;

    exlib::string href;
    hr = u->get_href(href);
    if (hr.failed())
        return hr;

    if (options.IsEmpty() || !from_object)
        return u->get_href(retVal);

    // Node's WHATWG options: auth/fragment/search are "keep unless falsy" and
    // unicode switches the host between its ASCII and its Unicode form.
    if (!options->IsObject())
        return result_t::make(CALL_E_INVALID_DATA).with_type_name(errtype::kTypeError)
            .with_type_name(errtype::kTypeError)
            .with_code("ERR_INVALID_ARG_TYPE")
            .with_message("The \"options\" argument must be of type object.");

    Isolate* isolate = Isolate::current();
    v8::Local<v8::Context> context = isolate->context();
    v8::Local<v8::Value> option;

    auto option_enabled = [&](const char* name, bool dflt) -> bool {
        option = options->Get(context, isolate->NewString(name)).FromMaybe(v8::Local<v8::Value>());
        if (option.IsEmpty() || option->IsUndefined() || option->IsNull())
            return dflt;
        return option->BooleanValue(isolate->m_isolate);
    };

    bool keep_auth = option_enabled("auth", true);
    bool keep_fragment = option_enabled("fragment", true);
    bool keep_search = option_enabled("search", true);
    bool unicode = option_enabled("unicode", false);

    // Every option is honoured by rewriting the href, so the parts that are
    // dropped are exactly the ones Node drops and the rest keeps its original
    // spelling.
    exlib::string result(href);

    if (!keep_fragment) {
        size_t pos = result.find('#');
        if (pos != exlib::string::npos)
            result.resize(pos);
    }

    if (!keep_search) {
        size_t hash = result.find('#');
        size_t end = hash == exlib::string::npos ? result.length() : hash;
        size_t pos = result.find('?');
        if (pos != exlib::string::npos && pos < end) {
            size_t stop = end;
            result.erase(pos, stop - pos);
        }
    }

    if (!keep_auth) {
        // "scheme://user:pass@host/...": drop the userinfo section.
        size_t scheme = result.find("://");
        if (scheme != exlib::string::npos) {
            size_t at = result.find('@', scheme + 3);
            size_t slash = result.find('/', scheme + 3);
            if (at != exlib::string::npos && (slash == exlib::string::npos || at < slash))
                result.erase(scheme + 3, at - scheme - 2);
        }
    }

    if (unicode) {
        // Rewrite the host through the IDN decoder, keeping the rest as it is.
        size_t scheme = result.find("://");
        if (scheme != exlib::string::npos) {
            size_t start = scheme + 3;
            size_t at = result.find('@', start);
            if (at != exlib::string::npos) {
                size_t slash = result.find('/', start);
                if (slash != exlib::string::npos && at < slash)
                    start = at + 1;
            }

            size_t end = start;
            while (end < result.length() && result[end] != '/' && result[end] != '?' && result[end] != '#')
                end++;

            size_t colon = result.find(':', start);
            if (colon != exlib::string::npos && colon < end)
                end = colon;

            if (end > start) {
                exlib::string host(result.c_str() + start, end - start);
                exlib::string decoded = ada::idna::to_unicode(host);
                if (decoded != host)
                    result = exlib::string(result.c_str(), start) + decoded + result.substr(end);
            }
        }
    }

    retVal = result;
    return result_t();
}

} /* namespace fibjs */
