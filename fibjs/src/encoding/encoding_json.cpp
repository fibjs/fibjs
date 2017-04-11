/*
 * encoding_json.cpp
 *
 *  Created on: Aug 24, 2015
 *      Author: lion
 */

#include "object.h"
#include "ifs/encoding.h"
#include "qstring.h"
#include "Buffer.h"
#include "utf8.h"
#include <stdlib.h>

namespace fibjs {

DECLARE_MODULE(json);

struct _from {
    const char* name;
    void (*from)(Isolate*, v8::Local<v8::Value>, v8::Local<v8::Object>&);
} s_from[] = {
    { "Buffer", Buffer::fromJSON },
    { NULL }
};

inline result_t _jsonEncode(v8::Local<v8::Value> data,
    exlib::string& retVal)
{
    Isolate* isolate = Isolate::current();
    v8::Local<v8::Object> _json;

    if (isolate->m_json.IsEmpty()) {
        v8::Local<v8::Object> glob = isolate->context()->Global();
        _json = glob->Get(isolate->NewFromUtf8("JSON"))->ToObject();
        isolate->m_json.Reset(isolate->m_isolate, _json);

        isolate->m_stringify.Reset(isolate->m_isolate,
            v8::Local<v8::Function>::Cast(_json->Get(isolate->NewFromUtf8("stringify"))));
    } else
        _json = v8::Local<v8::Object>::New(isolate->m_isolate, isolate->m_json);

    TryCatch try_catch;
    v8::Local<v8::Value> str = v8::Local<v8::Function>::New(isolate->m_isolate, isolate->m_stringify)->Call(_json, 1, &data);
    if (try_catch.HasCaught())
        return CHECK_ERROR(Runtime::setError(*v8::String::Utf8Value(try_catch.Exception())));

    v8::String::Utf8Value v(str);
    retVal.assign(*v, v.length());

    return 0;
}

result_t json_base::encode(v8::Local<v8::Value> data,
    exlib::string& retVal)
{
    return _jsonEncode(data, retVal);
}

inline bool IsInRange(int32_t value, int32_t lower_limit, int32_t higher_limit)
{
    return static_cast<uint32_t>(value - lower_limit) <= static_cast<uint32_t>(higher_limit - lower_limit);
}

inline bool IsDecimalDigit(char c)
{
    return IsInRange(c, '0', '9');
}

inline int32_t AsciiAlphaToLower(char c)
{
    return c | 0x20;
}

inline result_t _jsonDecode(exlib::string data,
    v8::Local<v8::Value>& retVal)
{
    class json_parser {
    public:
        json_parser(exlib::string source)
            : isolate(Isolate::current())
            , source_(source.c_str())
            , source_length_((int32_t)source.length())
            , position_(-1)
        {
        }

        inline void Advance()
        {
            position_++;
            if (position_ >= source_length_)
                c0_ = 0;
            else
                c0_ = source_[position_];
        }

        inline void AdvanceSkipWhitespace()
        {
            do {
                Advance();
            } while (c0_ == ' ' || c0_ == '\t' || c0_ == '\n' || c0_ == '\r');
        }

        inline void SkipWhitespace()
        {
            while (c0_ == ' ' || c0_ == '\t' || c0_ == '\n' || c0_ == '\r') {
                Advance();
            }
        }

        inline char AdvanceGetChar()
        {
            Advance();
            return c0_;
        }

        inline bool MatchSkipWhiteSpace(char c)
        {
            if (c0_ == c) {
                AdvanceSkipWhitespace();
                return true;
            }
            return false;
        }

        result_t ParseJsonNumber(v8::Local<v8::Value>& retVal)
        {
            bool negative = false;
            int32_t beg_pos = position_;

            if (c0_ == '-') {
                Advance();
                negative = true;
            }

            if (c0_ == '0') {
                Advance();
                if (IsDecimalDigit(c0_))
                    return ReportUnexpectedCharacter();
            } else {
                int32_t i = 0;
                int32_t digits = 0;
                if (c0_ < '1' || c0_ > '9')
                    return ReportUnexpectedCharacter();

                do {
                    i = i * 10 + c0_ - '0';
                    digits++;
                    Advance();
                } while (IsDecimalDigit(c0_));

                if (c0_ != '.' && c0_ != 'e' && c0_ != 'E' && digits < 10) {
                    SkipWhitespace();
                    retVal = v8::Int32::New(isolate->m_isolate, negative ? -i : i);
                    return 0;
                }
            }

            if (c0_ == '.') {
                Advance();
                if (!IsDecimalDigit(c0_))
                    return ReportUnexpectedCharacter();

                do {
                    Advance();
                } while (IsDecimalDigit(c0_));
            }

            if (AsciiAlphaToLower(c0_) == 'e') {
                Advance();
                if (c0_ == '-' || c0_ == '+')
                    Advance();
                if (!IsDecimalDigit(c0_))
                    return ReportUnexpectedCharacter();

                do {
                    Advance();
                } while (IsDecimalDigit(c0_));
            }

            int32_t length = position_ - beg_pos;
            double number;
            exlib::string chars(source_ + beg_pos, length);

            number = atof(chars.c_str());
            SkipWhitespace();
            retVal = v8::Number::New(isolate->m_isolate, number);
            return 0;
        }

        result_t ParseJsonString(v8::Local<v8::Value>& retVal)
        {
            exlib::wstring str;

            Advance();
            while (c0_ != '"') {
                if (c0_ == 0 || c0_ == '\r' || c0_ == '\n')
                    return ReportUnexpectedCharacter();

                if (c0_ != '\\') {
                    int32_t beg_pos = position_;

                    while (c0_ != '"' && c0_ != '\\') {
                        Advance();
                        if (c0_ == 0 || c0_ == '\r' || c0_ == '\n')
                            return ReportUnexpectedCharacter();
                    }
                    str.append(utf8to16String(source_ + beg_pos, position_ - beg_pos));
                } else {
                    Advance();
                    switch (c0_) {
                    case '"':
                    case '\\':
                    case '/':
                        str.append(1, c0_);
                        break;
                    case 'b':
                        str.append(1, '\x08');
                        break;
                    case 'f':
                        str.append(1, '\x0c');
                        break;
                    case 'n':
                        str.append(1, '\x0a');
                        break;
                    case 'r':
                        str.append(1, '\x0d');
                        break;
                    case 't':
                        str.append(1, '\x09');
                        break;
                    case 'u': {
                        uint16_t value = 0;
                        for (int32_t i = 0; i < 4; i++) {
                            Advance();
                            if (!qisxdigit(c0_))
                                return ReportUnexpectedCharacter();

                            value = value * 16 + qhex(c0_);
                        }

                        str.append(1, value);
                        break;
                    }
                    default:
                        return ReportUnexpectedCharacter();
                    }
                    Advance();
                }
            }

            AdvanceSkipWhitespace();

            retVal = v8::String::NewFromTwoByte(isolate->m_isolate,
                (const uint16_t*)str.c_str(),
                v8::String::kNormalString,
                (int32_t)str.length());
            return 0;
        }

        result_t ParseJsonArray(v8::Local<v8::Value>& retVal)
        {
            v8::Local<v8::Array> elements = v8::Array::New(isolate->m_isolate);
            int32_t cnt = 0;
            result_t hr;

            AdvanceSkipWhitespace();

            if (c0_ != ']') {
                do {
                    v8::Local<v8::Value> element;
                    hr = ParseJsonValue(element);
                    if (hr < 0)
                        return hr;
                    elements->Set(cnt++, element);
                } while (MatchSkipWhiteSpace(','));

                if (c0_ != ']')
                    return ReportUnexpectedCharacter();
            }

            AdvanceSkipWhitespace();
            retVal = elements;
            return 0;
        }

        result_t ParseJsonObject(v8::Local<v8::Value>& retVal)
        {
            v8::Local<v8::Object> json_object = v8::Object::New(isolate->m_isolate);
            result_t hr;

            AdvanceSkipWhitespace();
            if (c0_ != '}') {
                do {
                    if (c0_ != '"')
                        return ReportUnexpectedCharacter();

                    v8::Local<v8::Value> key;
                    v8::Local<v8::Value> value;

                    hr = ParseJsonString(key);
                    if (hr < 0)
                        return hr;

                    if (c0_ != ':')
                        return ReportUnexpectedCharacter();

                    AdvanceSkipWhitespace();

                    hr = ParseJsonValue(value);
                    if (hr < 0)
                        return hr;

                    json_object->Set(key, value);
                } while (MatchSkipWhiteSpace(','));

                if (c0_ != '}')
                    return ReportUnexpectedCharacter();
            }

            AdvanceSkipWhitespace();

            v8::Local<v8::Value> type = json_object->Get(isolate->NewFromUtf8("type"));
            v8::Local<v8::Value> data = json_object->Get(isolate->NewFromUtf8("data"));

            if (!type.IsEmpty() && !data.IsEmpty()) {
                v8::String::Utf8Value str(type);

                if (*str) {
                    int32_t i;

                    for (i = 0; s_from[i].name; i++)
                        if (!qstrcmp(*str, s_from[i].name)) {
                            s_from[i].from(isolate, data, json_object);
                            break;
                        }
                }
            }

            retVal = json_object;
            return 0;
        }

        result_t ParseJsonValue(v8::Local<v8::Value>& retVal)
        {
            if (c0_ == '"')
                return ParseJsonString(retVal);

            if ((c0_ >= '0' && c0_ <= '9') || c0_ == '-')
                return ParseJsonNumber(retVal);

            if (c0_ == '{')
                return ParseJsonObject(retVal);

            if (c0_ == '[')
                return ParseJsonArray(retVal);

            if (c0_ == 'f') {
                if (AdvanceGetChar() == 'a' && AdvanceGetChar() == 'l' && AdvanceGetChar() == 's' && AdvanceGetChar() == 'e') {
                    AdvanceSkipWhitespace();
                    retVal = v8::False(isolate->m_isolate);

                    return 0;
                }
                return ReportUnexpectedCharacter();
            }

            if (c0_ == 't') {
                if (AdvanceGetChar() == 'r' && AdvanceGetChar() == 'u' && AdvanceGetChar() == 'e') {
                    AdvanceSkipWhitespace();
                    retVal = v8::True(isolate->m_isolate);

                    return 0;
                }
                return ReportUnexpectedCharacter();
            }

            if (c0_ == 'n') {
                if (AdvanceGetChar() == 'u' && AdvanceGetChar() == 'l' && AdvanceGetChar() == 'l') {
                    AdvanceSkipWhitespace();
                    retVal = v8::Null(isolate->m_isolate);

                    return 0;
                }
                return ReportUnexpectedCharacter();
            }

            return ReportUnexpectedCharacter();
        }

        result_t ReportUnexpectedCharacter()
        {
            exlib::string s = "Unexpected token ";
            s.append(1, c0_);
            return CHECK_ERROR(Runtime::setError(s));
        }

        result_t ParseJson(v8::Local<v8::Value>& retVal)
        {
            AdvanceSkipWhitespace();
            return ParseJsonValue(retVal);
        }

    private:
        Isolate* isolate;
        const char* source_;
        int32_t source_length_;
        int32_t position_;
        char c0_;
    };

    json_parser jp(data);
    return jp.ParseJson(retVal);
}

result_t json_base::decode(exlib::string data,
    v8::Local<v8::Value>& retVal)
{
    return _jsonDecode(data, retVal);
}

result_t encoding_base::jsstr(exlib::string str, bool json, exlib::string& retVal)
{
    const char* p;
    char* p1;
    int32_t len;
    char ch;
    exlib::string s;

    if (str.empty())
        return 0;

    const char* ptr = str.c_str();
    const char* ptr_end = ptr + str.length();

    for (len = 0, p = ptr; p < ptr_end; p++, len++) {
        ch = *p;
        if (ch == '\\' || ch == '\r' || ch == '\n' || ch == '\t' || ch == '\"'
            || (!json && ch == '\''))
            len++;
    }

    s.resize(len);

    for (p1 = &s[0], p = ptr; p < ptr_end; p++) {
        ch = *p;
        switch (ch) {
        case '\\':
            *p1++ = '\\';
            *p1++ = '\\';
            break;
        case '\r':
            *p1++ = '\\';
            *p1++ = 'r';
            break;
        case '\n':
            *p1++ = '\\';
            *p1++ = 'n';
            break;
        case '\t':
            *p1++ = '\\';
            *p1++ = 't';
            break;
        case '\"':
            *p1++ = '\\';
            *p1++ = '\"';
            break;
        case '\'':
            if (!json) {
                *p1++ = '\\';
                *p1++ = '\'';
                break;
            }
        default:
            *p1++ = ch;
            break;
        }
    }

    retVal = s;
    return 0;
}
}
