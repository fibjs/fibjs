/*
 * HttpHeaders.h
 *
 * The two header field checks of RFC 7230 section 3.2, in the shape Node.js
 * exposes them as http.validateHeaderName / http.validateHeaderValue.
 *
 * They live in a header of their own because they are meant to be reused: the
 * module-level functions are the only callers today, and the writing paths
 * (setHeader, Headers#set, the response constructors) deliberately do not
 * validate, so making them stricter later is a matter of calling these from
 * there instead of writing a second copy of the rules.
 */

#pragma once

#include "object.h"

namespace fibjs {

// One character of a token, the set a header field name is built from:
// "!#$%&'*+-.^_`|~" plus digits and letters. Everything else - space, the
// separators ()<>@,;:\"/[]?={} and every byte above 0x7f - ends the name.
inline bool is_http_token_char(uint8_t c)
{
    if ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
        return true;

    switch (c) {
    case '!':
    case '#':
    case '$':
    case '%':
    case '&':
    case '\'':
    case '*':
    case '+':
    case '-':
    case '.':
    case '^':
    case '_':
    case '`':
    case '|':
    case '~':
        return true;
    }

    return false;
}

// A header field body cannot carry NUL, CR or LF: the first would end the
// value as far as any parser is concerned, and the other two would end the
// field itself and let the rest be read as another header. Bytes above 0x7f
// are rejected too - Node.js reports them the same way, and the field has no
// charset of its own.
inline bool is_http_header_value_char(uint8_t c)
{
    return c != 0 && c != '\r' && c != '\n' && c < 0x80;
}

inline bool is_http_token(const char* s, size_t len)
{
    if (len == 0)
        return false;

    for (size_t i = 0; i < len; i++)
        if (!is_http_token_char((uint8_t)s[i]))
            return false;

    return true;
}

inline bool is_http_header_value(const char* s, size_t len)
{
    for (size_t i = 0; i < len; i++)
        if (!is_http_header_value_char((uint8_t)s[i]))
            return false;

    return true;
}

// The string form of a value, the way Node.js prints it in the message: a
// non-string is converted, so `null` and `1` appear as "null" and "1".
inline exlib::string http_header_string(v8::Local<v8::Value> v, Isolate* isolate)
{
    v8::Local<v8::String> s = v->ToString(isolate->context()).FromMaybe(v8::Local<v8::String>());

    if (s.IsEmpty())
        return exlib::string();

    v8::String::Utf8Value utf8(isolate->m_isolate, s);
    return exlib::string(*utf8, utf8.length());
}

inline result_t check_header_name(v8::Local<v8::Value> name, Isolate* isolate)
{
    // A name is never converted into: Node.js rejects a non-string whatever it
    // would stringify to, so `null` and `1` fail even though "null" and "1"
    // are both valid tokens. The string form only shapes the message.
    exlib::string s = http_header_string(name, isolate);

    if (name->IsString() && is_http_token(s.c_str(), s.length()))
        return result_t();

    exlib::string msg = "Header name must be a valid HTTP token [\"";
    msg.append(s);
    msg.append("\"]");

    return result_t::make(CALL_E_INVALID_DATA)
        .with_type_name(errtype::kTypeError)
        .with_code("ERR_INVALID_HTTP_TOKEN")
        .with_message(msg);
}

inline result_t check_header_value(exlib::string& name, v8::Local<v8::Value> value, Isolate* isolate)
{
    // undefined is the one value the collection would write as the literal
    // text "undefined", so it is the one that fails instead of converting.
    if (value->IsUndefined()) {
        exlib::string msg = "Invalid value \"undefined\" for header \"";
        msg.append(name);
        msg.append("\"");

        return result_t::make(CALL_E_INVALID_DATA)
            .with_type_name(errtype::kTypeError)
            .with_code("ERR_HTTP_INVALID_HEADER_VALUE")
            .with_message(msg);
    }

    exlib::string s = http_header_string(value, isolate);

    if (is_http_header_value(s.c_str(), s.length()))
        return result_t();

    exlib::string msg = "Invalid character in header content [\"";
    msg.append(name);
    msg.append("\"]");

    return result_t::make(CALL_E_INVALID_DATA)
        .with_type_name(errtype::kTypeError)
        .with_code("ERR_INVALID_CHAR")
        .with_message(msg);
}

}
