/*
 * Variant.cpp
 *
 *  Created on: Jul 31, 2012
 *      Author: lion
 */

#include <math.h>
#include "object.h"
#include "ifs/json.h"
#include <stdio.h>
#include <stdlib.h>

namespace fibjs {

class UNBIND_DATA {
public:
    exlib::string k;
    Variant v;
};

Variant& Variant::operator=(v8::Local<v8::Value> v)
{
    clear();

    if (v.IsEmpty() || v->IsUndefined())
        set_type(VT_Undefined);
    else if (v->IsNull())
        set_type(VT_Null);
    else if (v->IsDate()) {
        set_type(VT_Date);
        dateVal() = v;
    } else if (v->IsBoolean() || v->IsBooleanObject()) {
        set_type(VT_Boolean);
        m_Val.boolVal = v->BooleanValue();
    } else if (v->IsNumber() || v->IsNumberObject()) {

        double n = v->NumberValue();
        int64_t num = (int64_t)n;

        if (n == (double)num) {
            if (num >= -2147483648ll && num <= 2147483647ll) {
                set_type(VT_Integer);
                m_Val.intVal = (int32_t)num;
            } else {
                set_type(VT_Long);
                m_Val.longVal = num;
            }
        } else {
            set_type(VT_Number);
            m_Val.dblVal = n;
        }
    } else if (v->IsString() || v->IsStringObject()) {
        v8::String::Utf8Value s(v);
        exlib::string str(*s, s.length());
        return operator=(str);
    } else {
        object_base* obj = object_base::getInstance(v);

        if (obj)
            return operator=(obj);
        else {
            set_type(VT_JSValue);

            if (isGlobal()) {
                new (m_Val.jsVal) v8::Global<v8::Value>();
                jsValEx().Reset(Isolate::current()->m_isolate, v);
            } else
                new (m_Val.jsVal) v8::Local<v8::Value>(v);

            return *this;
        }
    }

    return *this;
}

Variant::operator v8::Local<v8::Value>() const
{
    Isolate* isolate = Isolate::current();

    switch (type()) {
    case VT_Undefined:
        return v8::Undefined(isolate->m_isolate);
    case VT_Null:
    case VT_Type:
    case VT_Global:
        return v8::Null(isolate->m_isolate);
    case VT_Boolean:
        return m_Val.boolVal ? v8::True(isolate->m_isolate) : v8::False(isolate->m_isolate);
    case VT_Integer:
        return v8::Int32::New(isolate->m_isolate, m_Val.intVal);
    case VT_Long:
        return v8::Number::New(isolate->m_isolate, (double)m_Val.longVal);
    case VT_Number:
        return v8::Number::New(isolate->m_isolate, m_Val.dblVal);
    case VT_Date:
        return dateVal().value(isolate->m_isolate);
    case VT_Object: {
        object_base* obj = (object_base*)m_Val.objVal;
        if (obj == NULL)
            break;

        v8::Local<v8::Value> v;
        obj->valueOf(v);
        return v;
    }
    case VT_JSValue:
        if (isGlobal())
            return v8::Local<v8::Value>::New(isolate->m_isolate, jsValEx());
        else
            return jsVal();
    case VT_JSON: {
        v8::Local<v8::Value> v;

        json_base::decode(strVal(), v);
        return v;
    }
    case VT_String: {
        exlib::string& str = strVal();
        return isolate->NewFromUtf8(str);
    }
    case VT_UNBOUND_ARRAY: {
        v8::Local<v8::Array> a;
        int32_t len, i;
        UNBIND_DATA* data;

        len = m_Val.buffer.cnt;
        data = (UNBIND_DATA*)m_Val.buffer.data;
        a = v8::Array::New(isolate->m_isolate, len);

        for (i = 0; i < len; i++)
            a->Set(i, data[i].v.operator v8::Local<v8::Value>());

        return a;
    }
    case VT_UNBOUND_OBJECT: {
        v8::Local<v8::Object> o;
        int32_t len, i;
        UNBIND_DATA* data;

        len = m_Val.buffer.cnt;
        data = (UNBIND_DATA*)m_Val.buffer.data;
        o = v8::Object::New(isolate->m_isolate);

        for (i = 0; i < len; i++)
            o->Set(isolate->NewFromUtf8(data[i].k), data[i].v.operator v8::Local<v8::Value>());

        return o;
    }
    }

    return v8::Null(isolate->m_isolate);
}

inline void next(int32_t& len, int32_t& pos)
{
    pos++;
    if (len > 0)
        len--;
}

inline int64_t getInt(const char* s, int32_t& len, int32_t& pos)
{
    char ch;
    int64_t n = 0;

    while (len && (ch = s[pos]) && ch >= '0' && ch <= '9') {
        n = n * 10 + ch - '0';
        next(len, pos);
    }

    return n;
}

inline char pick(const char* s, int32_t& len, int32_t& pos)
{
    return len == 0 ? 0 : s[pos];
}

void Variant::parseNumber(const char* str, int32_t len)
{
    int64_t digit, frac = 0, exp = 0;
    double v, div = 1.0;
    bool bNeg, bExpNeg;
    int32_t pos = 0;
    char ch;

    bNeg = (pick(str, len, pos) == '-');
    if (bNeg)
        next(len, pos);

    digit = getInt(str, len, pos);

    ch = pick(str, len, pos);
    if (ch == '.') {
        next(len, pos);

        while ((ch = pick(str, len, pos)) && ch >= '0' && ch <= '9') {
            frac = frac * 10 + ch - '0';
            div *= 10;
            next(len, pos);
        }
    }

    if (ch == 'e' || ch == 'E') {
        next(len, pos);

        bExpNeg = false;
        ch = pick(str, len, pos);
        if (ch == '+')
            next(len, pos);
        else if (ch == '-') {
            bExpNeg = true;
            next(len, pos);
        }

        exp = getInt(str, len, pos);
        if (bExpNeg)
            exp = -exp;
    }

    if (div > 1 || exp != 0) {
        v = digit + (frac / div);
        if (bNeg)
            v = -v;

        if (exp != 0)
            v *= pow((double)10, (int32_t)exp);

        set_type(VT_Number);
        m_Val.dblVal = v;

        return;
    }

    if (bNeg)
        digit = -digit;

    if (digit >= -2147483648ll && digit <= 2147483647ll) {
        set_type(VT_Integer);
        m_Val.intVal = (int32_t)digit;
    } else {
        set_type(VT_Long);
        m_Val.longVal = digit;
    }
}

#define STRING_BUF_SIZE 1024

void Variant::toString(exlib::string& retVal) const
{
    switch (type()) {
    case VT_Undefined:
        retVal = "undefined";
        break;
    case VT_Null:
    case VT_Type:
    case VT_Global:
        retVal = "null";
        break;
    case VT_Boolean:
        retVal = m_Val.boolVal ? "true" : "false";
        break;
    case VT_Integer: {
        char str[STRING_BUF_SIZE];

        sprintf(str, "%d", m_Val.intVal);
        retVal = str;

        break;
    }
    case VT_Long: {
        char str[STRING_BUF_SIZE];

#ifdef _WIN32
        sprintf(str, "%lld", m_Val.longVal);
#else
        sprintf(str, "%lld", (long long)m_Val.longVal);
#endif

        retVal = str;

        break;
    }
    case VT_Number: {
        char str[STRING_BUF_SIZE];

        sprintf(str, "%.16g", m_Val.dblVal);
        retVal = str;

        break;
    }
    case VT_Date:
        dateVal().toGMTString(retVal);
        break;
    case VT_Object: {
        object_base* obj = (object_base*)m_Val.objVal;

        if (obj == NULL)
            break;

        obj->toString(retVal);

        break;
    }
    case VT_String:
        retVal = strVal();
        break;
    case VT_JSValue:
    case VT_UNBOUND_ARRAY:
    case VT_UNBOUND_OBJECT:
        retVal = "[Object]";
        break;
    case VT_JSON:
        retVal = strVal();
        break;
    }
}

void Variant::toJSON()
{
    if (type() != VT_JSON) {
        v8::Local<v8::Value> v = operator v8::Local<v8::Value>();
        exlib::string str;

        json_base::encode(v, str);

        clear();
        set_type(VT_JSON);
        new (m_Val.strVal) exlib::string(str);
    }
}

result_t Variant::unbind()
{
    result_t hr;

    switch (type()) {
    case VT_Object: {
        object_base* obj = (object_base*)m_Val.objVal;
        obj_ptr<object_base> obj1;

        if (obj == NULL)
            break;

        hr = obj->unbind(obj1);
        if (hr < 0)
            return hr;

        m_Val.objVal->Unref();
        m_Val.objVal = obj1;
        m_Val.objVal->Ref();

        break;
    }
    case VT_JSValue: {
        v8::Local<v8::Value> v = operator v8::Local<v8::Value>();
        v8::Local<v8::Object> o;
        v8::Local<v8::Array> a;
        int32_t len, i;
        UNBIND_DATA* data;

        if (GetArgumentValue(v, a, true) >= 0) {
            clear();
            set_type(VT_UNBOUND_ARRAY);

            len = a->Length();

            m_Val.buffer.cnt = len;
            if (len > 0) {
                m_Val.buffer.data = data = new UNBIND_DATA[len];

                for (i = 0; i < len; i++)
                    data[i].v = a->Get(i);
            }
        } else if (GetArgumentValue(v, o, true) >= 0) {
            clear();
            set_type(VT_UNBOUND_OBJECT);

            v8::Local<v8::Array> ks = o->GetPropertyNames();
            len = ks->Length();

            m_Val.buffer.cnt = len;
            if (len > 0) {
                m_Val.buffer.data = data = new UNBIND_DATA[len];
                for (i = 0; i < len; i++) {
                    v8::Local<v8::Value> k = ks->Get(i);

                    data[i].k = *v8::String::Utf8Value(k);
                    data[i].v = o->Get(k);
                }
            }
        } else
            return CHECK_ERROR(CALL_E_TYPEMISMATCH);

        for (i = 0; i < len; i++) {
            hr = data[i].v.unbind();
            if (hr < 0)
                return hr;
        }

        break;
    }
    default:
        break;
    }

    return 0;
}

void Variant::clearUnbind()
{
    if (m_Val.buffer.cnt > 0)
        delete[](UNBIND_DATA*) m_Val.buffer.data;
}
}
