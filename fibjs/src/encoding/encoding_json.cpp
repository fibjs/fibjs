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

#include "v8.h"
#include "v8/src/api/api-inl.h"
#include "v8/src/utils/utils.h"
#include "v8/src/api/api.h"
#include "v8/src/execution/isolate.h"
#include "v8/src/execution/frames.h"
#include "v8/src/execution/frames-inl.h"
#include "v8/src/base/vector.h"
#include "v8/include/v8-json.h"

#include "src/objects/string-inl.h"

using namespace v8;

namespace fibjs {

DECLARE_MODULE(json);

result_t json_base::encode(v8::Local<v8::Value> data, exlib::string& retVal)
{
    Isolate* isolate = Isolate::current();

    v8::Local<v8::String> str = v8::JSON::Stringify(isolate->context(), data).FromMaybe(v8::Local<v8::String>());
    if (str.IsEmpty())
        return CALL_E_JAVASCRIPT;

    return GetArgumentValue(isolate, str, retVal);
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

inline int32_t qstrcmp(const char16_t* s1, const char* s2, ssize_t sz = -1)
{
    int32_t n = 0;

    while (*s1 && !(n = *s1++ - (char16_t)*s2++))
        if ((sz > 0) && (!--sz))
            return 0;

    return n ? n : -*s2;
}

inline result_t _jsonDecode(exlib::string data,
    v8::Local<v8::Value>& retVal)
{
    class json_parser {
    public:
        json_parser(exlib::string& source)
            : isolate(Isolate::current())
            , v8_isolate((i::Isolate*)isolate->m_isolate)
            , zone_(v8_isolate->allocator(), ZONE_NAME)
            , object_constructor_(v8_isolate->native_context()->object_function(),
                  v8_isolate)
            , source_(source.c_str())
            , source_length_(source.length())
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

        result_t ReportUnexpectedCharacter()
        {
            exlib::string s = "Unexpected token ";
            s.append(1, c0_);
            return CHECK_ERROR(Runtime::setError(s));
        }

        result_t ParseJsonNumber(i::MaybeHandle<i::Object>& retVal)
        {
            bool negative = false;
            ssize_t beg_pos = position_;

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
                    retVal = factory()->NewNumberFromInt(negative ? -i : i);
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

            ssize_t length = position_ - beg_pos;
            double number;
            exlib::string chars(source_ + beg_pos, length);

            number = atof(chars.c_str());
            SkipWhitespace();
            retVal = factory()->NewNumber(number);
            return 0;
        }

        result_t ParseJsonString(i::MaybeHandle<i::Object>& retVal, exlib::wstring& str)
        {
            Advance();
            while (c0_ != '"') {
                if (c0_ == 0 || c0_ == '\r' || c0_ == '\n')
                    return ReportUnexpectedCharacter();

                if (c0_ != '\\') {
                    ssize_t beg_pos = position_;

                    while (c0_ != '"' && c0_ != '\\') {
                        Advance();
                        if (c0_ == 0 || c0_ == '\r' || c0_ == '\n')
                            return ReportUnexpectedCharacter();
                    }

                    const char* src = source_ + beg_pos;
                    ssize_t srclen = position_ - beg_pos;

                    ssize_t n = utf_convert(src, srclen, (char16_t*)NULL, 0);
                    ssize_t n1 = str.length();

                    str.resize(n + n1);
                    utf_convert(src, srclen, str.data() + n1, n);
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

            base::Vector<const uint16_t> data_((const uint16_t*)str.c_str(), str.length());
            retVal = factory()->NewStringFromTwoByte(data_, i::AllocationType::kYoung);
            return 0;
        }

        result_t ParseJsonArray(i::MaybeHandle<i::Object>& retVal)
        {
            i::ZoneVector<i::Handle<i::Object>> els(&zone_);
            result_t hr;

            AdvanceSkipWhitespace();

            if (c0_ != ']') {
                do {
                    i::MaybeHandle<i::Object> el;
                    i::Handle<i::Object> el1;

                    hr = ParseJsonValue(el);
                    if (hr < 0)
                        return hr;

                    el1 = el.ToHandleChecked();

                    els.push_back(el1);
                } while (MatchSkipWhiteSpace(','));

                if (c0_ != ']')
                    return ReportUnexpectedCharacter();
            }

            AdvanceSkipWhitespace();

            i::Handle<i::Object> json_array;
            int elements_size = static_cast<int>(els.size());

            i::Handle<i::FixedArray> elems = factory()->NewFixedArray(elements_size, i::AllocationType::kYoung);
            for (int i = 0; i < elements_size; i++)
                elems->set(i, *els[i]);
            retVal = factory()->NewJSArrayWithElements(elems);
            return 0;
        }

        result_t ParseJsonObject(i::MaybeHandle<i::Object>& retVal)
        {
            i::Handle<i::JSObject> json_object = factory()->NewJSObject(object_constructor_);
            result_t hr;
            i::Handle<i::String> type;
            i::Handle<i::Object> data;

            AdvanceSkipWhitespace();
            if (c0_ != '}') {
                do {
                    if (c0_ != '"')
                        return ReportUnexpectedCharacter();

                    exlib::wstring str;
                    i::MaybeHandle<i::Object> key;
                    i::MaybeHandle<i::Object> value;

                    hr = ParseJsonString(key, str);
                    if (hr < 0)
                        return hr;

                    if (c0_ != ':')
                        return ReportUnexpectedCharacter();

                    AdvanceSkipWhitespace();

                    hr = ParseJsonValue(value);
                    if (hr < 0)
                        return hr;

                    i::Handle<i::String> name = i::Object::ToString(v8_isolate, key.ToHandleChecked()).ToHandleChecked();

                    i::JSObject::DefinePropertyOrElementIgnoreAttributes(json_object,
                        name, value.ToHandleChecked())
                        .Check();
                } while (MatchSkipWhiteSpace(','));

                if (c0_ != '}')
                    return ReportUnexpectedCharacter();
            }

            AdvanceSkipWhitespace();

            retVal = json_object;
            return 0;
        }

        result_t ParseJsonValue(i::MaybeHandle<i::Object>& retVal)
        {
            if (c0_ == '"') {
                exlib::wstring str;
                return ParseJsonString(retVal, str);
            }

            if ((c0_ >= '0' && c0_ <= '9') || c0_ == '-')
                return ParseJsonNumber(retVal);

            if (c0_ == '{')
                return ParseJsonObject(retVal);

            if (c0_ == '[')
                return ParseJsonArray(retVal);

            if (c0_ == 'f') {
                if (AdvanceGetChar() == 'a' && AdvanceGetChar() == 'l' && AdvanceGetChar() == 's' && AdvanceGetChar() == 'e') {
                    AdvanceSkipWhitespace();
                    retVal = factory()->false_value();

                    return 0;
                }
                return ReportUnexpectedCharacter();
            }

            if (c0_ == 't') {
                if (AdvanceGetChar() == 'r' && AdvanceGetChar() == 'u' && AdvanceGetChar() == 'e') {
                    AdvanceSkipWhitespace();
                    retVal = factory()->true_value();

                    return 0;
                }
                return ReportUnexpectedCharacter();
            }

            if (c0_ == 'n') {
                if (AdvanceGetChar() == 'u' && AdvanceGetChar() == 'l' && AdvanceGetChar() == 'l') {
                    AdvanceSkipWhitespace();
                    retVal = factory()->null_value();

                    return 0;
                }
                return ReportUnexpectedCharacter();
            }

            return ReportUnexpectedCharacter();
        }

        result_t ParseJsonValue(v8::Local<v8::Value>& retVal)
        {
            i::MaybeHandle<i::Object> maybe;
            result_t hr = ParseJsonValue(maybe);
            if (hr < 0)
                return hr;

            v8::ToLocal(maybe, &retVal);
            return 0;
        }

        result_t ParseJson(v8::Local<v8::Value>& retVal)
        {
            AdvanceSkipWhitespace();
            return ParseJsonValue(retVal);
        }

        i::Factory* factory()
        {
            return v8_isolate->factory();
        }

    private:
        Isolate* isolate;
        i::Isolate* v8_isolate;
        i::Zone zone_;
        i::Handle<i::JSFunction> object_constructor_;
        const char* source_;
        ssize_t source_length_;
        ssize_t position_;
        char c0_;
    };

    json_parser jp(data);
    return jp.ParseJson(retVal);
}

result_t json_base::decode(exlib::string data, v8::Local<v8::Value>& retVal)
{
    if (data.length() < 1024 * 1024) {
        Isolate* isolate = Isolate::current();

        retVal = v8::JSON::Parse(isolate->context(), isolate->NewString(data)).FromMaybe(v8::Local<v8::Value>());
        return retVal.IsEmpty() ? CALL_E_JAVASCRIPT : 0;
    }

    return _jsonDecode(data, retVal);
}

result_t encoding_base::jsstr(exlib::string str, bool json, exlib::string& retVal)
{
    const char* p;
    char* p1;
    ssize_t len;
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

    for (p1 = s.data(), p = ptr; p < ptr_end; p++) {
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
