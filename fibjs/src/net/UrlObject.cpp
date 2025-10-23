/*
 * Url.cpp
 *
 *  Created on: Jul 14, 2012
 *      Author: lion
 */

#include "object.h"
#include "Url.h"
#include "ifs/encoding.h"
#include "URLSearchParams.h"

namespace fibjs {

static const char* pathTable = " !  $%& ()*+,-./0123456789:; =  @ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_ abcdefghijklmnopqrstuvwxyz{|}~ ";
static ada::url_aggregator s_base;

const struct {
    const char* protocol;
    int32_t length;
} slashedProtocol[] = {
    { "http:", 5 },
    { "https:", 6 },
    { "ftp:", 4 },
    { "gopher:", 7 },
    { "file:", 5 },
    { "ws:", 3 },
    { "wss:", 4 }
};

static int32_t is_non_slash_protocol(const char* p, bool slashes)
{
    if (qstricmp(p, "javascript:", 11) == 0)
        return 11;
    if (qstricmp(p, "mailto:", 7) == 0)
        return 7;
    if (qstricmp(p, "xmpp:", 5) == 0)
        return 5;

    if (!slashes) {
        const char* p1 = qstrchr(p, ':');
        if (p1)
            return p1 - p + 1;
    }

    return 0;
}

static bool is_slashed_protocol(const char* p)
{
    for (const auto& proto : slashedProtocol) {
        if (qstricmp(p, proto.protocol, proto.length) == 0)
            return true;
    }
    return false;
}

inline bool is_slash(char ch)
{
    return ch == '/' || ch == '\\';
}

result_t UrlObject_base::_new(exlib::string url, exlib::string base,
    obj_ptr<UrlObject_base>& retVal, v8::Local<v8::Object> This)
{
    return parse(url, base, retVal);
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

result_t UrlObject_base::canParse(exlib::string url, exlib::string base, bool& retVal)
{
    ada::result<ada::url_aggregator> _base;

    if (!base.empty()) {
        _base = ada::parse(base, &s_base);
        if (!_base || !_base->validate()) {
            retVal = false;
            return 0;
        }
    }

    ada::result<ada::url_aggregator> _url = ada::parse(url, _base ? &_base.value() : nullptr);
    retVal = _url && _url->validate();

    return 0;
}

result_t UrlObject_base::parse(exlib::string url, exlib::string base, obj_ptr<UrlObject_base>& retVal)
{
    obj_ptr<Url> u = new Url();

    result_t hr = u->parse(url, base);
    if (hr < 0)
        return hr;

    retVal = u;

    return 0;
}

result_t Url::parse(exlib::string url, exlib::string base)
{
    ada::result<ada::url_aggregator> _base;

    m_slashes = true;

    if (!base.empty()) {
        _base = ada::parse(base, &s_base);
        if (!_base || !_base->validate())
            return Runtime::setError("url: Invalid URL '" + base + "'.");
    }

    m_url = ada::parse(url, &_base.value());
    if (!m_url || !m_url->validate())
        return Runtime::setError("url: Invalid URL '" + url + "'.");

    return 0;
}

result_t Url::legacy_parse(exlib::string url, bool parseQueryString)
{
    m_isLegacy = true;

    const char* p = url.c_str();
    const char* p2 = p;

    while (*p2 == ' ' || *p2 == '\t' || *p2 == '\r' || *p2 == '\n')
        p2++;

    if (p2 != p) {
        url = url.substr(p2 - p);
        p = url.c_str();
    }

    // Extract original auth info before parsing
    m_originalAuth.clear();
    m_hasAuthSeparator = false;
    size_t at_pos = url.find('@');
    if (at_pos != exlib::string::npos) {
        m_hasAuthSeparator = true;
        size_t protocol_end = url.find("://");
        if (protocol_end != exlib::string::npos) {
            size_t auth_start = protocol_end + 3;
            if (auth_start < at_pos) {
                m_originalAuth = url.substr(auth_start, at_pos - auth_start);
            }
        }
    }

    // Validate percent encoding in the URL string
    const char* url_str = url.c_str();
    for (size_t i = 0; i < url.length(); i++) {
        if (url_str[i] == '%') {
            // Check if we have at least 2 more characters
            if (i + 2 >= url.length()) {
                return Runtime::setError("url: URI malformed");
            }

            // Check if the next two characters are valid hex digits
            char c1 = url_str[i + 1];
            char c2 = url_str[i + 2];

            bool c1_valid = (c1 >= '0' && c1 <= '9') || (c1 >= 'A' && c1 <= 'F') || (c1 >= 'a' && c1 <= 'f');
            bool c2_valid = (c2 >= '0' && c2 <= '9') || (c2 >= 'A' && c2 <= 'F') || (c2 >= 'a' && c2 <= 'f');

            if (!c1_valid || !c2_valid) {
                return Runtime::setError("url: URI malformed");
            }

            // Skip the two hex digits
            i += 2;
        }
    }

    exlib::string str = url;
    const char* p1 = qstrchr(p, ':');
    if (p1 && !is_slash(p1[1])) {
        exlib::string protocol = url.substr(0, (p1 - p) + 1);
        if (protocol != "javascript:")
            str = protocol + "//" + url.substr((p1 - p) + 1);
    } else
        m_slashes = true;

    m_parseQuery = parseQueryString;

    m_url = ada::parse(str, &s_base);
    if (!m_url || !m_url->validate())
        return Runtime::setError("url: Invalid URL '" + url + "'.");

    return 0;
}

result_t Url::format(v8::Local<v8::Object> args)
{
    Isolate* isolate = holder();
    v8::Local<v8::Context> context = isolate->context();

    bool isJavascript = false;
    exlib::string str;
    exlib::string url;
    exlib::string username;
    exlib::string password;
    JSValue v;

    if (GetConfigValue(isolate, args, "protocol", str, true) >= 0) {
        const char* p = str.c_str();
        int32_t len = str.length();

        if (len > 0) {
            isJavascript = str == "javascript:";
            if (is_slash(p[len - 1]) && is_slash(p[len - 2])) {
                str = str.substr(0, len - 2);
                p = str.c_str();
                len -= 2;
            }

            if (p[len - 1] != ':')
                url = str + ":";
            else
                url = str;
        }
    }

    bool slashes = is_slashed_protocol(url.c_str());
    if (GetConfigValue(isolate, args, "slashes", slashes) >= 0 && slashes)
        url += "//";
    m_slashes = slashes;

    if (GetConfigValue(isolate, args, "auth", str, true) >= 0) {
        size_t pos = str.find(':');
        if (pos != exlib::string::npos) {
            username = str.substr(0, pos);
            password = str.substr(pos + 1);
        } else
            username = str;
    }

    GetConfigValue(isolate, args, "username", username, true);
    GetConfigValue(isolate, args, "password", password, true);

    if (username.length() > 0 || password.length() > 0) {
        encoding_base::encodeURIComponent(username, false, username);
        encoding_base::encodeURIComponent(password, false, password);
        url += username;
        if (password.length() > 0)
            url += ":" + password;
        url += "@";
    }

    bool hasHost = false;
    if (GetConfigValue(isolate, args, "host", str, true) >= 0) {
        url += ada::idna::to_ascii(str);
        hasHost = true;
    } else if (GetConfigValue(isolate, args, "hostname", str, true) >= 0) {
        if (str.find(':') != exlib::string::npos && str[0] != '[')
            url += '[' + str + ']';
        else
            url += ada::idna::to_ascii(str);

        if (GetConfigValue(isolate, args, "port", str) >= 0)
            url += ":" + str;

        hasHost = true;
    }

    if (GetConfigValue(isolate, args, "pathname", str, true) >= 0) {
        if (hasHost && !isJavascript) {
            if (!is_slash(str[0]))
                url += "/";
            Url::encodeURI(str, str, pathTable);
        }
        url += str;
    }

    if (url.length() > 0) {
        ada::url_aggregator _base;
        m_url = ada::parse(url, &_base);
    } else
        m_url = ada::url_aggregator();

    v = args->Get(context, holder()->NewString("query"));
    if (!IsEmpty(v))
        set_query(v);

    if (GetConfigValue(isolate, args, "search", str, true) >= 0)
        set_search(str);

    if (GetConfigValue(isolate, args, "hash", str, true) >= 0)
        set_hash(str);

    return 0;
}

result_t Url::resolve(exlib::string to, obj_ptr<UrlObject_base>& retVal)
{
    obj_ptr<Url> u = new Url();

    u->m_url = ada::parse(to, m_url ? &m_url.value() : nullptr);
    if (!u->m_url)
        return Runtime::setError("url: Invalid URL '" + to + "'.");
    u->m_slashes = true;

    retVal = u;

    return 0;
}

result_t Url::get_href(exlib::string& retVal)
{
    if (m_url) {
        if (m_searchParams) {
            exlib::string searchStr;
            result_t hr = m_searchParams->toString(searchStr);
            if (hr < 0)
                return hr;

            m_url->set_search(searchStr);
        }

        retVal = m_url->get_href();

        const char* p = retVal.c_str();
        int32_t n = is_non_slash_protocol(p, m_slashes);
        if (n > 0 && is_slash(p[n]) && is_slash(p[n + 1]))
            retVal = retVal.substr(0, n) + retVal.substr(n + 2);
    }

    return 0;
}

result_t Url::set_href(exlib::string newVal)
{
    m_searchParams.Release();
    return parse(newVal);
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

result_t Url::get_protocol(exlib::string& retVal)
{
    if (m_url)
        retVal = m_url->get_protocol();

    return 0;
}

result_t Url::set_protocol(exlib::string newVal)
{
    if (m_url)
        m_url->set_protocol(newVal);

    return 0;
}

result_t Url::get_origin(exlib::string& retVal)
{
    if (m_url)
        retVal = m_url->get_origin();

    return 0;
}

result_t Url::get_auth(exlib::string& retVal)
{
    if (m_isLegacy) {
        // In legacy mode, check if we found an @ symbol during parsing
        if (!m_hasAuthSeparator) {
            return CALL_RETURN_NULL;
        }

        // If we have @ separator, decode and return the auth (even if empty)
        exlib::string decoded_auth = m_originalAuth;
        decodeURI(decoded_auth, decoded_auth);
        retVal = decoded_auth;

        return 0;
    }

    // For non-legacy mode (WHATWG API), use the parsed components
    if (m_url) {
        exlib::string username = m_url->get_username();
        exlib::string password = m_url->get_password();

        // Check if the original URL contained auth info by checking the href
        exlib::string href = m_url->get_href();
        size_t at_pos = href.find('@');

        // If no @ symbol found, return null like Node.js
        if (at_pos == exlib::string::npos) {
            return CALL_RETURN_NULL;
        }

        retVal = username;
        if (!password.empty()) {
            retVal.append(1, ':');
            retVal.append(password);
        }
    } else {
        return CALL_RETURN_NULL;
    }

    return 0;
}

result_t Url::get_username(exlib::string& retVal)
{
    if (m_url) {
        retVal = m_url->get_username();
        if (m_isLegacy)
            decodeURI(retVal, retVal);
    }

    return 0;
}

result_t Url::set_username(exlib::string newVal)
{
    if (m_url) {
        if (m_isLegacy)
            encoding_base::encodeURIComponent(newVal, false, newVal);
        m_url->set_username(newVal);
    }

    return 0;
}

result_t Url::get_password(exlib::string& retVal)
{
    if (m_url) {
        retVal = m_url->get_password();
        if (m_isLegacy)
            decodeURI(retVal, retVal);
    }

    return 0;
}

result_t Url::set_password(exlib::string newVal)
{
    if (m_url) {
        if (m_isLegacy)
            encoding_base::encodeURIComponent(newVal, false, newVal);
        m_url->set_password(newVal);
    }

    return 0;
}

result_t Url::get__host(exlib::string& retVal)
{
    if (m_url)
        retVal = m_url->get_host();

    return 0;
}

result_t Url::set__host(exlib::string newVal)
{
    if (m_url)
        m_url->set_host(newVal);

    return 0;
}

result_t Url::get_hostname(exlib::string& retVal)
{
    if (m_url)
        retVal = m_url->get_hostname();

    return 0;
}

result_t Url::set_hostname(exlib::string newVal)
{
    if (m_url)
        m_url->set_hostname(newVal);

    return 0;
}

result_t Url::get_port(exlib::string& retVal)
{
    if (m_url) {
        retVal = m_url->get_port();
    }

    return 0;
}

result_t Url::set_port(exlib::string newVal)
{
    if (m_url)
        m_url->set_port(newVal);

    return 0;
}

result_t Url::get_path(exlib::string& retVal)
{
    if (m_url) {
        retVal = m_url->get_pathname();
        retVal += m_url->get_search();
    }

    return 0;
}

result_t Url::get_pathname(exlib::string& retVal)
{
    if (m_url)
        retVal = m_url->get_pathname();

    return 0;
}

result_t Url::set_pathname(exlib::string newVal)
{
    if (m_url)
        m_url->set_pathname(newVal);

    return 0;
}

result_t Url::get_search(exlib::string& retVal)
{
    if (m_searchParams)
        return m_searchParams->toString(retVal);

    if (m_url)
        retVal = m_url->get_search();

    return 0;
}

result_t Url::set_search(exlib::string newVal)
{
    if (m_url) {
        m_searchParams.Release();
        if (newVal == "?")
            m_url->set_search("");
        else
            m_url->set_search(newVal);
    }

    return 0;
}

result_t Url::get_query(v8::Local<v8::Value>& retVal)
{
    if (!m_url)
        return CALL_RETURN_UNDEFINED;

    if (m_parseQuery) {
        parse_search_params();
        retVal = m_searchParams->wrap();
    } else {
        exlib::string search = m_url->get_search();
        if (search.length() > 0)
            retVal = holder()->NewString(search.data() + 1, search.length() - 1);
    }

    return 0;
}

result_t Url::set_query(v8::Local<v8::Value> newVal)
{
    if (newVal->IsString() || newVal->IsStringObject()) {
        exlib::string query;
        result_t hr = GetArgumentValue(holder(), newVal, query, true);
        if (hr < 0)
            return hr;

        return set_search(query);
    } else if (newVal->IsObject()) {
        ada::url_search_params search_params;

        v8::Local<v8::Object> obj = newVal.As<v8::Object>();
        v8::Local<v8::Array> keys = obj->GetPropertyNames(holder()->context()).ToLocalChecked();
        int32_t len = keys->Length();

        if (len == 0)
            return 0;

        for (uint32_t i = 0; i < len; i++) {
            v8::Local<v8::Value> key = keys->Get(holder()->context(), i).ToLocalChecked();
            v8::Local<v8::Value> value = obj->Get(holder()->context(), key).ToLocalChecked();

            exlib::string k, v;
            result_t hr = GetArgumentValue(holder(), key, k, false);
            if (hr < 0)
                return hr;

            hr = GetArgumentValue(holder(), value, v, false);
            if (hr < 0)
                return hr;

            search_params.append(k, v);
        }

        exlib::string search_params_str = search_params.to_string();

        set_search('?' + search_params_str);

        return 0;
    }

    return CALL_E_INVALID_DATA;
}

result_t Url::get_hash(exlib::string& retVal)
{
    if (m_url)
        retVal = m_url->get_hash();

    return 0;
}

result_t Url::set_hash(exlib::string newVal)
{
    if (m_url)
        m_url->set_hash(newVal);

    return 0;
}

result_t Url::get_searchParams(obj_ptr<URLSearchParams_base>& retVal)
{
    if (!m_url)
        return CALL_RETURN_UNDEFINED;

    parse_search_params();
    retVal = m_searchParams;

    return 0;
}

result_t Url::toString(exlib::string& retVal)
{
    return get_href(retVal);
}

result_t Url::toJSON(exlib::string key, v8::Local<v8::Value>& retVal)
{
    exlib::string href;
    result_t hr = get_href(href);
    if (hr < 0)
        return hr;

    Isolate* isolate = holder();
    retVal = isolate->NewString(href);
    return 0;
}

result_t Url::parse_search_params()
{
    if (!m_searchParams) {
        exlib::string search = m_url->get_search();
        if (search[0] == '?')
            search = search.substr(1); // Remove leading '?'

        m_searchParams = new URLSearchParams();
        result_t hr = m_searchParams->parse(search);
        if (hr < 0)
            return hr;
    }

    return 0;
}

} /* namespace fibjs */
