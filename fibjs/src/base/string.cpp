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

    str->WriteUtf8(isolate, n.data(), bufUtf8Len, NULL, flags);
    return n;
}

exlib::string ToString(v8::Isolate* isolate, v8::Local<v8::Value> v)
{
    exlib::string n;
    v8::Local<v8::String> str;

    if (v->IsDate())
        str = v.As<v8::Date>()->ToISOString();
    else
        str = v->ToString(isolate->GetCurrentContext()).FromMaybe(v8::Local<v8::String>());
    if (str.IsEmpty())
        return n;

    return ToString(isolate, str);
}

// an object converts to a string only through a toString() of its own: the
// generic Object.prototype / Array.prototype rendering ("[object Object]",
// "1,2") is a tag rather than a conversion, and a native object carries none
// (its generic toString reports an error, see object_base::toString)
static bool has_own_toString(Isolate* isolate, v8::Local<v8::Value> v)
{
    v8::Local<v8::Object> o = v.As<v8::Object>();
    v8::Local<v8::Context> context = isolate->context();
    v8::Local<v8::String> key = isolate->NewString("toString");

    v8::Local<v8::Value> fn;
    if (!o->Get(context, key).ToLocal(&fn) || !fn->IsFunction())
        return false;

    return o->HasOwnProperty(context, key).FromMaybe(false);
}

result_t GetArgumentValue(Isolate* isolate, v8::Local<v8::Value> v, exlib::string& n, bool bStrict)
{
    if (v.IsEmpty())
        return CALL_E_TYPEMISMATCH;

    v8::Local<v8::String> str;

    if (v->IsString())
        str = v.As<v8::String>();
    else if (v->IsStringObject())
        str = v.As<v8::StringObject>()->ValueOf();
    else {
        // the first pass takes real strings only. The second pass renders a
        // value through a toString() of its own: a JS object that brings one
        // (`{toString: () => "x"}`) and a native object whose class implements
        // one (a URL renders its href, a Buffer its bytes, an element its
        // markup) convert through it, which is what a string argument means in
        // JavaScript.
        //
        // everything else is not a string: primitives (1 is not "1", null is
        // not "null"), objects that only inherit the generic Object.prototype /
        // Array.prototype rendering ("[object Object]", "1,2"), and native
        // objects whose class implements no toString -- the generic
        // object_base::toString reports an error instead of turning into a
        // plausible looking "[object Name]", so those have no string form
        if (bStrict || !v->IsObject())
            return CALL_E_TYPEMISMATCH;

        // a read that fails inside the object -- a getter that throws -- is the
        // JavaScript error of that getter, not a type error of the argument
        v8::TryCatch trycatch(isolate->m_isolate);

        if (v->IsDate())
            str = v.As<v8::Date>()->ToISOString();
        else if (IsJSBuffer(v) || has_own_toString(isolate, v))
            // a buffer renders its bytes as utf8, which is what handing a
            // buffer to a string parameter has always meant here
            str = v->ToString(isolate->context()).FromMaybe(v8::Local<v8::String>());
        else if (IsNativeObject(v)) {
            str = v->ToString(isolate->context()).FromMaybe(v8::Local<v8::String>());

            if (trycatch.HasCaught() || str.IsEmpty())
                return CALL_E_TYPEMISMATCH;
        } else {
            if (trycatch.HasCaught()) {
                trycatch.ReThrow();

                return CALL_E_JAVASCRIPT;
            }

            return CALL_E_TYPEMISMATCH;
        }
    }

    if (str.IsEmpty())
        return CALL_E_JAVASCRIPT;

    n = ToString(isolate->m_isolate, str);
    return 0;
}

result_t GetDOMStringValue(v8::Local<v8::Value> v, exlib::string& retVal)
{
    Isolate* isolate = v->IsObject() ? Isolate::current(v.As<v8::Object>()) : Isolate::current();

    // the value's own string form, like String() in JavaScript. A conversion
    // that throws is either a symbol (the engine error stands) or a native
    // object whose class implements no toString() -- the generic one reports
    // an error (object_base::toString); the DOM renders the object tag for the
    // latter, which is what Object.prototype.toString gives: String(file) is
    // "[object File]"
    v8::TryCatch trycatch(isolate->m_isolate);
    v8::Local<v8::String> str = v->ToString(isolate->context()).FromMaybe(v8::Local<v8::String>());

    if (trycatch.HasCaught()) {
        object_base* o = object_base::getInstance(v);

        if (!o) {
            // the conversion of a value that is not a native object threw (a
            // symbol, or a toString() of its own): the engine error stands
            trycatch.ReThrow();

            return CALL_E_JAVASCRIPT;
        }

        retVal = "[object ";
        retVal.append(o->Classinfo().name());
        retVal.append("]");
        return 0;
    }

    if (str.IsEmpty())
        return CALL_E_JAVASCRIPT;

    retVal = ToString(isolate->m_isolate, str);
    return 0;
}

} // namespace fibjs