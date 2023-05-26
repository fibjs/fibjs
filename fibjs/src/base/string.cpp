/*
 * string.cpp
 *
 *  Created on: Apr 25, 2021
 *      Author: lion
 */

#include "object.h"
#include "Isolate.h"
#include <limits.h>

namespace fibjs {

exlib::atomic g_ExtStringCount;

class ExtStringW : public v8::String::ExternalStringResource {
public:
    ExtStringW(v8::Isolate* _isolate, exlib::wstring _buffer)
        : m_isolate(_isolate)
        , m_buffer(_buffer)
    {
        g_ExtStringCount.inc();
        m_isolate->AdjustAmountOfExternalAllocatedMemory(m_buffer.length() * 2);
    }

    ~ExtStringW()
    {
        m_isolate->AdjustAmountOfExternalAllocatedMemory(-(int64_t)m_buffer.length() * 2);
        g_ExtStringCount.dec();
    }

public:
    virtual const uint16_t* data() const
    {
        return (const uint16_t*)m_buffer.c_str();
    }

    virtual size_t length() const
    {
        return m_buffer.length();
    }

    exlib::string str()
    {
        return utf16to8String(m_buffer);
    }

private:
    v8::Isolate* m_isolate;
    exlib::wstring m_buffer;
};

class ExtString : public v8::String::ExternalOneByteStringResource {
public:
    ExtString(v8::Isolate* _isolate, exlib::string _buffer)
        : m_isolate(_isolate)
        , m_buffer(_buffer)
    {
        g_ExtStringCount.inc();
        m_isolate->AdjustAmountOfExternalAllocatedMemory(m_buffer.length());
    }

    ~ExtString()
    {
        m_isolate->AdjustAmountOfExternalAllocatedMemory(-(int64_t)m_buffer.length());
        g_ExtStringCount.dec();
    }

public:
    virtual const char* data() const
    {
        return m_buffer.c_str();
    }

    virtual size_t length() const
    {
        return m_buffer.length();
    }

    exlib::string str()
    {
        return m_buffer;
    }

private:
    v8::Isolate* m_isolate;
    exlib::string m_buffer;
};

inline bool is_safe_string(const char* s, size_t len)
{
    const uint64_t* w = (const uint64_t*)s;
    size_t lenw = len / sizeof(uint64_t);

    for (size_t i = 0; i < lenw; i++)
        if (w[i] & 0x8080808080808080)
            return false;

    for (size_t i = lenw * sizeof(uint64_t); i < len; i++)
        if (s[i] & 0x80)
            return false;

    return true;
}

#define SMALL_STRING 1024

v8::Local<v8::String> NewString(v8::Isolate* isolate, exlib::string str)
{
    size_t length = str.length();

    if (length > INT_MAX) {
        ThrowResult(CALL_E_OVERFLOW);
        return v8::Local<v8::String>();
    }

    if (length < SMALL_STRING)
        return NewString(isolate, str.c_str(), length);

    v8::Local<v8::String> v;

    if (is_safe_string(str.c_str(), length))
        v = v8::String::NewExternalOneByte(isolate, new ExtString(isolate, str)).FromMaybe(v8::Local<v8::String>());
    else
        v = v8::String::NewExternalTwoByte(isolate, new ExtStringW(isolate, utf8to16String(str))).FromMaybe(v8::Local<v8::String>());

    return v;
}

v8::Local<v8::String> NewString(v8::Isolate* isolate, const char* data, ssize_t length)
{
    if (length == -1)
        length = (ssize_t)qstrlen(data);

    if (length > INT_MAX) {
        ThrowResult(CALL_E_OVERFLOW);
        return v8::Local<v8::String>();
    }

    if (length >= SMALL_STRING)
        return NewString(isolate, exlib::string(data, length));

    v8::Local<v8::String> v;

    if (is_safe_string(data, length))
        v = v8::String::NewFromOneByte(isolate, (const uint8_t*)data, v8::NewStringType::kNormal, (uint32_t)length).FromMaybe(v8::Local<v8::String>());
    else
        v = v8::String::NewExternalTwoByte(isolate, new ExtStringW(isolate, utf8to16String(data, length))).FromMaybe(v8::Local<v8::String>());

    return v;
}

exlib::string ToString(v8::Isolate* isolate, v8::Local<v8::String> str)
{
    exlib::string n;

    if (str->IsExternalOneByte())
        return ((ExtString*)str->GetExternalOneByteStringResource())->str();
    else if (str->IsExternal())
        return ((ExtStringW*)str->GetExternalStringResource())->str();

    int32_t bufUtf8Len = str->Utf8Length(isolate);
    n.resize(bufUtf8Len);
    int flags = v8::String::HINT_MANY_WRITES_EXPECTED | v8::String::NO_NULL_TERMINATION;

    str->WriteUtf8(isolate, n.c_buffer(), bufUtf8Len, NULL, flags);
    return n;
}

exlib::string ToString(v8::Isolate* isolate, v8::Local<v8::Value> v)
{
    exlib::string n;
    v8::Local<v8::String> str = v->ToString(isolate->GetCurrentContext()).FromMaybe(v8::Local<v8::String>());
    if (str.IsEmpty())
        return n;

    return ToString(isolate, str);
}

result_t GetArgumentValue(Isolate* isolate, v8::Local<v8::Value> v, exlib::string& n, bool bStrict)
{
    if (v.IsEmpty())
        return CALL_E_TYPEMISMATCH;

    v8::Local<v8::String> str;

    if (v->IsString())
        str = v8::Local<v8::String>::Cast(v);
    else if (v->IsStringObject())
        str = v8::Local<v8::StringObject>::Cast(v)->ValueOf();
    else if (!bStrict)
        str = v->ToString(isolate->context()).FromMaybe(v8::Local<v8::String>());
    else
        return CALL_E_TYPEMISMATCH;

    if (str.IsEmpty())
        return CALL_E_JAVASCRIPT;

    n = ToString(isolate->m_isolate, str);
    return 0;
}

} // namespace fibjs