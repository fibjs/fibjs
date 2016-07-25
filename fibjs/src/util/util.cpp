/*
 * utils.cpp
 *
 *  Created on: Dec 10, 2013
 *      Author: lion
 */

#include "object.h"
#include "ifs/util.h"
#include "ifs/encoding.h"
#include "ifs/Int64.h"
#include <zlib/include/zlib.h>
#include <sqlite/sqlite3.h>
#include <ev/ev.h>
#include <gd/include/gd.h>
#include <jpeg/include/jpeglib.h>
#include <png/include/png.h>
#include <pcre/pcre.h>
#include <tiff/include/tiffvers.h>
#include <mongo/include/mongo.h>
#include <mbedtls/mbedtls/version.h>
#include <snappy/include/snappy.h>
#include <leveldb/db.h>
#include <expat/include/expat.h>
#include "QuickArray.h"
#include "StringBuffer.h"
#include <map>

#ifndef _WIN32
#include "gitinfo.h"
#endif

namespace fibjs
{

DECLARE_MODULE(util);

inline void newline(StringBuffer &strBuffer, int32_t padding)
{
    strBuffer.append('\n');
    if (padding > 0)
        strBuffer.append(exlib::string(padding, ' '));
}

class _item
{
public:
    _item()
    {
        pos = 0;
        len = 0;
        mode = 0;
    }

    _item(const _item &v)
    {
        keys = v.keys;
        obj = v.obj;
        pos = v.pos;
        len = v.len;
        mode = v.mode;
    }

public:
    v8::Local<v8::Value> val;
    v8::Local<v8::Array> keys;
    v8::Local<v8::Object> obj;
    int32_t pos;
    int32_t len;
    int32_t mode;
};

void string_format(StringBuffer &strBuffer, v8::Local<v8::Value> v)
{
    exlib::string s;
    json_base::encode(v, s);
    strBuffer.append(s);
}

exlib::string json_format(v8::Local<v8::Value> obj)
{
    StringBuffer strBuffer;

    Isolate* isolate = Isolate::current();
    QuickArray<_item> stk;
    QuickArray<v8::Local<v8::Object>> vals;
    v8::Local<v8::Value> v = obj;
    int32_t padding = 0;
    const int32_t tab_size = 2;
    _item *it = NULL;

    while (true)
    {
        if (v.IsEmpty())
            strBuffer.append("undefined");
        else if (v->IsUndefined() || v->IsNull() || v->IsDate() ||
                 v->IsBoolean() || v->IsBooleanObject())
            strBuffer.append(*v8::String::Utf8Value(v));
        else if (v->IsNumber() || v->IsNumberObject())
            strBuffer.append(*v8::String::Utf8Value(v->ToNumber()));
        else if (v->IsString() || v->IsStringObject())
            string_format(strBuffer, v);
        else if (v->IsRegExp())
        {
            v8::Local<v8::RegExp> re = v8::Local<v8::RegExp>::Cast(v);
            v8::Local<v8::String> src = re->GetSource();
            v8::RegExp::Flags flgs = re->GetFlags();

            strBuffer.append('/');
            strBuffer.append(*v8::String::Utf8Value(src));
            strBuffer.append('/');

            if (flgs & v8::RegExp::kIgnoreCase)
                strBuffer.append('i');
            if (flgs & v8::RegExp::kGlobal)
                strBuffer.append('g');
            if (flgs & v8::RegExp::kMultiline)
                strBuffer.append('m');
        }
        else if (v->IsObject())
        {
            do
            {
                v8::Local<v8::Object> obj = v->ToObject();
                v8::Local<v8::Array> keys = obj->GetPropertyNames();

                if (v->IsFunction() && keys->Length() == 0)
                {
                    strBuffer.append("[Function]");
                    break;
                }

                obj_ptr<Buffer_base> buf = Buffer_base::getInstance(v);
                if (buf)
                {
                    static char hexs[] = "0123456789abcdef";
                    exlib::string data;
                    exlib::string s;
                    int32_t len, i;

                    buf->toString(data);
                    len = (int32_t)data.length();

                    s.resize(len * 3 + 8);
                    memcpy(&s[0], "<Buffer", 7);

                    for (i = 0; i < len; i ++)
                    {
                        int32_t ch = (unsigned char)data[i];

                        s[i * 3 + 7] = ' ';
                        s[i * 3 + 8] = hexs[ch >> 4];
                        s[i * 3 + 9] = hexs[ch & 0xf];
                    }

                    s[i * 3 + 7] = '>';

                    strBuffer.append(s);
                    break;
                }

                obj_ptr<Int64_base> int64Val = Int64_base::getInstance(v);
                if (int64Val)
                {
                    exlib::string s;
                    int64Val->toString(10, s);
                    strBuffer.append(s);
                    break;
                }

                int32_t i, sz1 = (int32_t)vals.size();
                for (i = 0; i < sz1; i ++)
                    if (vals[i]->Equals(obj))
                        break;

                if (i < sz1)
                {
                    strBuffer.append("[Circular]");
                    break;
                }

                vals.append(obj);

                v8::Local<v8::Value> toArray = obj->Get(isolate->NewFromUtf8("toArray"));
                if (!IsEmpty(toArray) && toArray->IsFunction())
                {
                    v = v8::Local<v8::Function>::Cast(toArray)->Call(obj, 0, NULL);
                    obj = v->ToObject();
                }

                int32_t sz = (int32_t)stk.size();

                if (v->IsArray())
                {
                    v8::Local<v8::Array> array = v8::Local<v8::Array>::Cast(v);
                    int32_t len = array->Length();

                    if (len == 0)
                        strBuffer.append("[]");
                    else
                    {
                        if (len == 1 && v->StrictEquals(array->Get(0)))
                            strBuffer.append("[Circular]");
                        else
                        {
                            stk.resize(sz + 1);
                            it = &stk[sz];

                            it->val = v;

                            it->keys = array;
                            it->len = len;

                            strBuffer.append('[');
                            padding += tab_size;
                        }
                    }
                    break;
                }

                int32_t len = keys->Length();

                if (len == 0)
                    strBuffer.append("{}");
                else
                {
                    if (len == 1 && v->StrictEquals(obj->Get(keys->Get(0))))
                        strBuffer.append("[Circular]");
                    else
                    {
                        stk.resize(sz + 1);
                        it = &stk[sz];

                        it->val = v;

                        it->obj = obj;
                        it->keys = keys;
                        it->len = len;

                        strBuffer.append('{');
                        padding += tab_size;
                    }
                }
            }
            while (false);
        }

        if (it)
        {
            while (it && it->pos == it->len)
            {
                padding -= tab_size;
                newline(strBuffer, padding);
                strBuffer.append(it->obj.IsEmpty() ? ']' : '}');

                int32_t sz = (int32_t)stk.size();

                stk.resize(sz - 1);
                if (sz > 1)
                    it = &stk[sz - 2];
                else
                    it = NULL;
            }

            if (!it)
                break;

            if (it->pos)
                strBuffer.append(',');
            newline(strBuffer, padding);

            v = it->keys->Get(it->pos ++);

            if (!it->obj.IsEmpty())
            {
                TryCatch try_catch;

                string_format(strBuffer, v);
                strBuffer.append(": ");
                v = it->obj->Get(v);
            }
        }
        else
            break;
    }

    return strBuffer.str();
}

result_t _format(const char *fmt, size_t fmt_sz, const v8::FunctionCallbackInfo<v8::Value> &args,
                 exlib::string &retVal)
{
    const char *s1;
    char ch;
    int32_t argc = args.Length();
    int32_t idx = 1;

    if (fmt == NULL)
    {
        idx = 0;
        fmt = "";
    }
    else if (argc == 1)
    {
        retVal = fmt;
        return 0;
    }

    const char *s = fmt;
    const char *s_end = fmt + fmt_sz;

    while (s < s_end)
    {
        ch = 0;
        s1 = s;
        while ((s < s_end) && ((ch = *s) != '%'))
            s++;

        retVal.append(s1, s - s1);

        if (ch == '%')
        {
            s++;

            switch (ch = *s++)
            {
            case 's':
                if (idx < argc)
                {
                    v8::String::Utf8Value s(args[idx++]);
                    if (*s)
                        retVal.append(*s, s.length());
                }
                else
                    retVal.append("%s", 2);
                break;
            case 'd':
                if (idx < argc)
                {
                    v8::String::Utf8Value s(args[idx++]->ToNumber());
                    if (*s)
                        retVal.append(*s, s.length());
                }
                else
                    retVal.append("%d", 2);
                break;
            case 'j':
                if (idx < argc)
                {
                    exlib::string s;
                    s = json_format(args[idx++]);
                    retVal.append(s);
                }
                else
                    retVal.append("%j", 2);
                break;
            default:
                retVal.append(1, '%');
            case '%':
                retVal.append(1, ch);
                break;
            }
        }
        else
            break;
    }

    while (idx < argc)
    {
        if (!retVal.empty())
            retVal.append(1, ' ');

        bool v;

        util_base::isString(args[idx], v);

        if (v)
        {
            v8::String::Utf8Value s(args[idx++]);
            retVal.append(*s, s.length());
        }
        else
        {
            exlib::string s;
            s = json_format(args[idx++]);

            retVal.append(s);
        }
    }

    return 0;
}

result_t util_base::format(exlib::string fmt, const v8::FunctionCallbackInfo<v8::Value> &args,
                           exlib::string &retVal)
{
    return _format(fmt.c_str(), fmt.length(), args, retVal);
}

result_t util_base::format(const v8::FunctionCallbackInfo<v8::Value> &args, exlib::string &retVal)
{
    return _format(NULL, 0, args, retVal);
}

result_t util_base::isEmpty(v8::Local<v8::Value> v, bool &retVal)
{
    if (v->IsNull())
    {
        retVal = true;
        return 0;
    }

    if (v->IsString())
    {
        retVal = v8::Local<v8::String>::Cast(v)->Length() == 0;
        return 0;
    }

    if (v->IsStringObject())
    {
        retVal = v8::Local<v8::StringObject>::Cast(v)->ValueOf()->Length() == 0;
        return 0;
    }

    if (v->IsArray())
    {
        retVal = v8::Local<v8::Array>::Cast(v)->Length() == 0;
        return 0;
    }

    if (v->IsObject())
    {
        retVal = v->ToObject()->GetOwnPropertyNames()->Length() == 0;
        return 0;
    }

    retVal = true;

    return 0;
}

result_t util_base::isArray(v8::Local<v8::Value> v, bool &retVal)
{
    retVal = v->IsArray();
    return 0;
}

result_t util_base::isBoolean(v8::Local<v8::Value> v, bool &retVal)
{
    retVal = v->IsBoolean() || v->IsBooleanObject();
    return 0;
}

result_t util_base::isNull(v8::Local<v8::Value> v, bool &retVal)
{
    retVal = v->IsNull();
    return 0;
}

result_t util_base::isNullOrUndefined(v8::Local<v8::Value> v, bool &retVal)
{
    retVal = v->IsNull() || v->IsUndefined();
    return 0;
}

result_t util_base::isNumber(v8::Local<v8::Value> v, bool &retVal)
{
    retVal = v->IsNumber() || v->IsNumberObject();
    return 0;
}

result_t util_base::isString(v8::Local<v8::Value> v, bool &retVal)
{
    retVal = v->IsString() || v->IsStringObject();
    return 0;
}

result_t util_base::isUndefined(v8::Local<v8::Value> v, bool &retVal)
{
    retVal = v->IsUndefined();
    return 0;
}

result_t util_base::isRegExp(v8::Local<v8::Value> v, bool &retVal)
{
    retVal = v->IsRegExp();
    return 0;
}

result_t util_base::isObject(v8::Local<v8::Value> v, bool &retVal)
{
    retVal = v->IsObject();
    return 0;
}

result_t util_base::isDate(v8::Local<v8::Value> v, bool &retVal)
{
    retVal = v->IsDate();
    return 0;
}

result_t util_base::isFunction(v8::Local<v8::Value> v, bool &retVal)
{
    retVal = v->IsFunction();
    return 0;
}

result_t util_base::isBuffer(v8::Local<v8::Value> v, bool &retVal)
{
    retVal = !!Buffer_base::getInstance(v);
    return 0;
}

result_t util_base::has(v8::Local<v8::Value> v, exlib::string key, bool &retVal)
{
    if (v->IsUndefined() || v->IsNull())
    {
        retVal = false;
        return 0;
    }

    if (!v->IsObject())
        return CHECK_ERROR(CALL_E_TYPEMISMATCH);

    v8::Local<v8::Object> obj = v->ToObject();
    retVal = obj->HasOwnProperty(Isolate::current()->NewFromUtf8(key));
    return 0;
}

result_t util_base::keys(v8::Local<v8::Value> v, v8::Local<v8::Array> &retVal)
{
    if (v->IsObject())
    {
        v8::Local<v8::Object> obj = v->ToObject();

        retVal = obj->GetPropertyNames();
        if (obj->IsArray())
        {
            int32_t len = retVal->Length();
            int32_t i;

            for (i = 0; i < len; i ++)
                retVal->Set(i, retVal->Get(i)->ToString());
        }
    }
    else
        retVal = v8::Array::New(Isolate::current()->m_isolate);

    return 0;
}

result_t util_base::values(v8::Local<v8::Value> v, v8::Local<v8::Array> &retVal)
{
    Isolate* isolate = Isolate::current();
    if (v->IsObject())
    {
        v8::Local<v8::Object> obj = v->ToObject();
        v8::Local<v8::Array> keys = obj->GetPropertyNames();
        v8::Local<v8::Array> arr = v8::Array::New(isolate->m_isolate);

        int32_t len = keys->Length();
        int32_t i, n = 0;

        for (i = 0; i < len; i ++)
        {
            v8::Local<v8::Value> key = keys->Get(i);
            arr->Set(n++, obj->Get(key));
        }

        retVal = arr;
    }
    else
        retVal = v8::Array::New(isolate->m_isolate);

    return 0;
}

result_t util_base::clone(v8::Local<v8::Value> v, v8::Local<v8::Value> &retVal)
{
    if (!v->IsProxy() && v->IsObject() && !object_base::getInstance(v))
    {
        Isolate* isolate = Isolate::current();

        if (v->IsFunction() || v->IsArgumentsObject() || v->IsSymbolObject())
            retVal = v;
        else if (v->IsDate())
            retVal = v8::Date::New(isolate->m_isolate, v->NumberValue());
        else if (v->IsBooleanObject())
            retVal = v8::BooleanObject::New(v->BooleanValue());
        else if (v->IsNumberObject())
            retVal = v8::NumberObject::New(isolate->m_isolate, v->NumberValue());
        else if (v->IsStringObject())
            retVal = v8::StringObject::New(v->ToString());
        else if (v->IsRegExp())
        {
            v8::Local<v8::RegExp> re = v8::Local<v8::RegExp>::Cast(v);
            retVal = v8::RegExp::New(re->GetSource(), re->GetFlags());
        }
        else
            retVal = v8::Local<v8::Object>::Cast(v)->Clone();
    }
    else
        retVal = v;

    return 0;
}

result_t util_base::extend(v8::Local<v8::Value> v,
                           const v8::FunctionCallbackInfo<v8::Value> &args,
                           v8::Local<v8::Value> &retVal)
{
    if (v->IsUndefined() || v->IsNull())
    {
        retVal = v;
        return 0;
    }

    if (!v->IsObject())
        return CHECK_ERROR(CALL_E_TYPEMISMATCH);

    v8::Local<v8::Object> obj = v->ToObject();
    int32_t argc = args.Length();
    int32_t i, j;

    for (i = 1; i < argc; i ++)
    {
        v8::Local<v8::Value> val = args[i];

        if (val->IsUndefined() || val->IsNull())
            continue;

        if (!val->IsObject())
            return CHECK_ERROR(CALL_E_TYPEMISMATCH);

        v8::Local<v8::Object> obj1 = val->ToObject();
        v8::Local<v8::Array> keys = obj1->GetPropertyNames();
        int32_t len = keys->Length();

        for (j = 0; j < len; j ++)
        {
            v8::Local<v8::Value> key = keys->Get(j);
            obj->Set(key, obj1->Get(key));
        }
    }

    retVal = obj;

    return 0;
}

result_t util_base::pick(v8::Local<v8::Value> v,
                         const v8::FunctionCallbackInfo<v8::Value> &args,
                         v8::Local<v8::Object> &retVal)
{
    Isolate* isolate = Isolate::current();
    if (v->IsUndefined() || v->IsNull())
    {
        retVal = v8::Object::New(isolate->m_isolate);
        return 0;
    }

    if (!v->IsObject())
        return CHECK_ERROR(CALL_E_TYPEMISMATCH);

    v8::Local<v8::Object> obj = v->ToObject();
    v8::Local<v8::Object> obj1 = v8::Object::New(isolate->m_isolate);
    int32_t argc = args.Length();
    int32_t i, j;

    for (i = 1; i < argc; i ++)
    {
        if (args[i]->IsArray())
        {
            v8::Local<v8::Array> arr = v8::Local<v8::Array>::Cast(args[i]);
            int32_t len = arr->Length();

            for (j = 0; j < len; j ++)
            {
                v8::Local<v8::Value> k = arr->Get(j);

                if (obj->Has(k))
                    obj1->Set(k, obj->Get(k));
            }
        }
        else
        {
            v8::Local<v8::Value> k = args[i];

            if (obj->Has(k))
                obj1->Set(k, obj->Get(k));
        }
    }

    retVal = obj1;

    return 0;
}

result_t util_base::omit(v8::Local<v8::Value> v,
                         const v8::FunctionCallbackInfo<v8::Value> &args,
                         v8::Local<v8::Object> &retVal)
{
    Isolate* isolate = Isolate::current();
    if (v->IsUndefined() || v->IsNull())
    {
        retVal = v8::Object::New(isolate->m_isolate);
        return 0;
    }

    if (!v->IsObject())
        return CHECK_ERROR(CALL_E_TYPEMISMATCH);

    v8::Local<v8::Object> obj = v->ToObject();

    std::map<exlib::string, bool> _map;
    int32_t argc = args.Length();
    int32_t i, j;
    result_t hr;

    for (i = 1; i < argc; i ++)
    {
        if (args[i]->IsArray())
        {
            v8::Local<v8::Array> arr = v8::Local<v8::Array>::Cast(args[i]);
            int32_t len = arr->Length();

            for (j = 0; j < len; j ++)
            {
                exlib::string k;
                hr = GetArgumentValue(arr->Get(j), k);
                if (hr < 0)
                    return CHECK_ERROR(hr);

                _map.insert(std::pair<exlib::string, bool>(k, true));
            }
        }
        else
        {
            exlib::string k;
            hr = GetArgumentValue(args[i], k);
            if (hr < 0)
                return CHECK_ERROR(hr);

            _map.insert(std::pair<exlib::string, bool>(k, true));
        }
    }

    v8::Local<v8::Array> keys = obj->GetPropertyNames();
    int32_t len = keys->Length();
    v8::Local<v8::Object> obj1 = v8::Object::New(isolate->m_isolate);

    for (i = 0; i < len; i ++)
    {
        v8::Local<v8::Value> key = keys->Get(i);

        if (_map.find(*v8::String::Utf8Value(key)) == _map.end())
            obj1->Set(key, obj->Get(key));
    }

    retVal = obj1;

    return 0;
}

result_t util_base::intersection(const v8::FunctionCallbackInfo<v8::Value> &args,
                                 v8::Local<v8::Array> &retVal)
{
    v8::Local<v8::Array> arr = v8::Array::New(Isolate::current()->m_isolate);
    int32_t argc = args.Length();
    int32_t i, j, k, n = 0;

    if (argc > 0)
    {
        if (args[0]->IsUndefined() || args[0]->IsNull())
        {
            retVal = arr;
            return 0;
        }

        for (j = 0; j < argc; j ++)
            if (!args[j]->IsArray())
                return CHECK_ERROR(CALL_E_TYPEMISMATCH);

        v8::Local<v8::Array> base = v8::Local<v8::Array>::Cast(args[0]);
        int32_t len = base->Length();
        int32_t left = len;
        QuickArray<v8::Local<v8::Value> > erase;

        erase.resize(len);
        for (i = 0; i < len; i ++)
            erase[i] = base->Get(i);

        for (i = 1; left > 0 && i < argc; i ++)
        {
            v8::Local<v8::Array> other = v8::Local<v8::Array>::Cast(args[i]);
            int32_t len1 = other->Length();

            for (j = 0; left > 0 && j < len; j ++)
                if (!erase[j].IsEmpty())
                {
                    for (k = 0; k < len1; k ++)
                        if (erase[j]->Equals(other->Get(k)))
                            break;

                    if (k == len1)
                    {
                        erase[j] = v8::Local<v8::Value>();
                        left --;
                    }
                }
        }

        if (left)
            for (i = 0; i < len; i ++)
                if (!erase[i].IsEmpty())
                {
                    for (j = 0; j < i; j ++)
                        if (!erase[j].IsEmpty() && erase[i]->Equals(erase[j]))
                            break;

                    if (j == i)
                        arr->Set(n ++, erase[i]);
                }
    }

    retVal = arr;

    return 0;
}

result_t util_base::first(v8::Local<v8::Value> v, v8::Local<v8::Value> &retVal)
{
    Isolate* isolate = Isolate::current();

    if (v->IsUndefined() || v->IsNull())
    {
        retVal = v8::Undefined(isolate->m_isolate);
        return 0;
    }

    if (!v->IsArray())
        return CHECK_ERROR(CALL_E_TYPEMISMATCH);

    v8::Local<v8::Array> arr = v8::Local<v8::Array>::Cast(v);
    int32_t len = arr->Length();

    if (len == 0)
    {
        retVal = v8::Undefined(isolate->m_isolate);
        return 0;
    }

    retVal = arr->Get(0);

    return 0;
}

result_t util_base::first(v8::Local<v8::Value> v, int32_t n, v8::Local<v8::Value> &retVal)
{
    Isolate* isolate = Isolate::current();

    if (v->IsUndefined() || v->IsNull() || n <= 0)
    {
        retVal = v8::Array::New(isolate->m_isolate);
        return 0;
    }

    if (!v->IsArray())
        return CHECK_ERROR(CALL_E_TYPEMISMATCH);

    v8::Local<v8::Array> arr = v8::Local<v8::Array>::Cast(v);
    int32_t len = arr->Length();
    int32_t i;

    if (n > len)
        n = len;

    v8::Local<v8::Array> arr1 = v8::Array::New(isolate->m_isolate);

    for (i = 0; i < n; i ++)
        arr1->Set(i, arr->Get(i));

    retVal = arr1;

    return 0;
}

result_t util_base::last(v8::Local<v8::Value> v, v8::Local<v8::Value> &retVal)
{
    Isolate* isolate = Isolate::current();

    if (v->IsUndefined() || v->IsNull())
    {
        retVal = v8::Undefined(isolate->m_isolate);
        return 0;
    }

    if (!v->IsArray())
        return CHECK_ERROR(CALL_E_TYPEMISMATCH);

    v8::Local<v8::Array> arr = v8::Local<v8::Array>::Cast(v);
    int32_t len = arr->Length();

    if (len == 0)
    {
        retVal = v8::Undefined(isolate->m_isolate);
        return 0;
    }

    retVal = arr->Get(len - 1);
    return 0;
}

result_t util_base::last(v8::Local<v8::Value> v, int32_t n, v8::Local<v8::Value> &retVal)
{
    Isolate* isolate = Isolate::current();
    if (v->IsUndefined() || v->IsNull() || n <= 0)
    {
        retVal = v8::Array::New(isolate->m_isolate);
        return 0;
    }

    if (!v->IsArray())
        return CHECK_ERROR(CALL_E_TYPEMISMATCH);

    v8::Local<v8::Array> arr = v8::Local<v8::Array>::Cast(v);
    int32_t len = arr->Length();
    int32_t i;

    if (n > len)
        n = len;

    v8::Local<v8::Array> arr1 = v8::Array::New(isolate->m_isolate);

    for (i = 0; i < n; i ++)
        arr1->Set(i, arr->Get(len - n + i));

    retVal = arr1;

    return 0;
}

result_t util_base::unique(v8::Local<v8::Value> v, bool sorted, v8::Local<v8::Array> &retVal)
{
    Isolate* isolate = Isolate::current();
    if (v->IsUndefined() || v->IsNull())
    {
        retVal = v8::Array::New(isolate->m_isolate);
        return 0;
    }

    if (!v->IsArray())
        return CHECK_ERROR(CALL_E_TYPEMISMATCH);

    v8::Local<v8::Array> arr1 = v8::Array::New(isolate->m_isolate);
    v8::Local<v8::Array> arr = v8::Local<v8::Array>::Cast(v);
    int32_t len = arr->Length();
    QuickArray<v8::Local<v8::Value> > vals;
    int32_t i, j, n = 0;

    vals.resize(len);
    for (i = 0; i < len; i ++)
    {
        v8::Local<v8::Value> val = arr->Get(i);

        for (j = i - 1; j >= 0; j --)
            if (!vals[j].IsEmpty())
            {
                if (val->StrictEquals(vals[j]))
                    break;

                if (sorted)
                {
                    j = -1;
                    break;
                }
            }

        if (j < 0)
            vals[i] = val;
    }

    for (i = 0; i < len; i ++)
        if (!vals[i].IsEmpty())
            arr1->Set(n++, vals[i]);

    retVal = arr1;

    return 0;
}

result_t util_base::_union(const v8::FunctionCallbackInfo<v8::Value> &args,
                           v8::Local<v8::Array> &retVal)
{
    v8::Local<v8::Array> arr = v8::Array::New(Isolate::current()->m_isolate);
    int32_t argc = args.Length();
    int32_t i, j, k, n = 0;

    for (i = 0; i < argc; i ++)
    {
        v8::Local<v8::Value> a = args[i];

        if (!a->IsArray())
            return CHECK_ERROR(CALL_E_TYPEMISMATCH);

        v8::Local<v8::Array> arr1 = v8::Local<v8::Array>::Cast(a);
        int32_t len = arr1->Length();

        for (j = 0; j < len; j ++)
        {
            v8::Local<v8::Value> v = arr1->Get(j);

            for (k = 0; k < n; k ++)
                if (v->StrictEquals(arr->Get(k)))
                    break;

            if (k == n)
                arr->Set(n ++, v);
        }
    }

    retVal = arr;

    return 0;
}

result_t util_base::flatten(v8::Local<v8::Value> list, bool shallow,
                            v8::Local<v8::Array> &retVal)
{
    if (!list->IsObject())
        return CHECK_ERROR(CALL_E_TYPEMISMATCH);

    bool bNext = true;

    Isolate* isolate = Isolate::current();

    if (retVal.IsEmpty())
        retVal = v8::Array::New(isolate->m_isolate);
    else if (shallow)
        bNext = false;

    v8::Local<v8::Object> o = v8::Local<v8::Object>::Cast(list);
    v8::Local<v8::Value> v = o->Get(isolate->NewFromUtf8("length"));
    if (IsEmpty(v))
        return CHECK_ERROR(CALL_E_TYPEMISMATCH);

    int32_t len = v->Int32Value();
    int32_t cnt = retVal->Length();
    int32_t i;

    for (i = 0; i < len; i ++)
    {
        v = o->Get(i);
        if (bNext && v->IsObject())
        {
            v8::Local<v8::Object> o1 = v8::Local<v8::Object>::Cast(v);
            v = o->Get(isolate->NewFromUtf8("length"));
            if (IsEmpty(v))
                retVal->Set(cnt ++, o->Get(i));
            else
            {
                flatten(o1, shallow, retVal);
                cnt = retVal->Length();
            }
        }
        else
            retVal->Set(cnt ++, o->Get(i));
    }

    return 0;
}

result_t util_base::without(v8::Local<v8::Value> arr,
                            const v8::FunctionCallbackInfo<v8::Value> &args,
                            v8::Local<v8::Array> &retVal)
{
    if (!arr->IsObject())
        return CHECK_ERROR(CALL_E_TYPEMISMATCH);

    Isolate* isolate = Isolate::current();

    v8::Local<v8::Object> o = v8::Local<v8::Object>::Cast(arr);
    v8::Local<v8::Value> v = o->Get(isolate->NewFromUtf8("length"));
    if (IsEmpty(v))
        return CHECK_ERROR(CALL_E_TYPEMISMATCH);

    int32_t len = v->Int32Value();

    v8::Local<v8::Array> arr1 = v8::Array::New(isolate->m_isolate);
    int32_t argc = args.Length();
    int32_t i, j, n = 0;

    for (i = 0; i < len; i ++)
    {
        v8::Local<v8::Value> v = o->Get(i);

        for (j = 1; j < argc; j ++)
            if (v->StrictEquals(args[j]))
                break;

        if (j == argc)
            arr1->Set(n ++, v);
    }

    retVal = arr1;

    return 0;
}

result_t util_base::difference(v8::Local<v8::Array> arr,
                               const v8::FunctionCallbackInfo<v8::Value> &args,
                               v8::Local<v8::Array> &retVal)
{
    v8::Local<v8::Array> arr1 = v8::Array::New(Isolate::current()->m_isolate);
    int32_t len = arr->Length();
    int32_t argc = args.Length();
    int32_t i, j, k, n = 0, len1;

    for (j = 0; j < argc; j ++)
        if (!args[j]->IsArray())
            return CHECK_ERROR(CALL_E_TYPEMISMATCH);

    for (i = 0; i < len; i ++)
    {
        v8::Local<v8::Value> v = arr->Get(i);

        for (j = 1; j < argc; j ++)
        {
            v8::Local<v8::Array> without = v8::Local<v8::Array>::Cast(args[j]);
            len1 = without->Length();

            for (k = 0; k < len1; k ++)
                if (v->StrictEquals(without->Get(k)))
                    break;

            if (k < len1)
                break;
        }

        if (j == argc)
            arr1->Set(n ++, v);
    }

    retVal = arr1;

    return 0;
}

result_t util_base::each(v8::Local<v8::Value> list, v8::Local<v8::Function> iterator,
                         v8::Local<v8::Value> context, v8::Local<v8::Value> &retVal)
{
    if (!list->IsObject())
    {
        retVal = list;
        return 0;
    }

    v8::Local<v8::Value> args[3];
    args[2] = list;

    Isolate* isolate = Isolate::current();
    v8::Local<v8::Object> o = v8::Local<v8::Object>::Cast(list);
    v8::Local<v8::Value> v = o->Get(isolate->NewFromUtf8("length"));

    if (IsEmpty(v))
    {
        v8::Local<v8::Array> keys = o->GetPropertyNames();
        int32_t len = keys->Length();
        int32_t i;

        for (i = 0; i < len; i ++)
        {
            args[1] = keys->Get(i);
            args[0] = o->Get(args[1]);

            v = iterator->Call(context, 3, args);
            if (v.IsEmpty())
                return CALL_E_JAVASCRIPT;
        }
    }
    else
    {
        int32_t len = v->Int32Value();

        int32_t i;

        for (i = 0; i < len; i ++)
        {
            args[1] = v8::Int32::New(isolate->m_isolate, i);
            args[0] = o->Get(args[1]);

            v = iterator->Call(context, 3, args);
            if (v.IsEmpty())
                return CALL_E_JAVASCRIPT;
        }
    }

    retVal = list;

    return 0;
}

result_t util_base::map(v8::Local<v8::Value> list, v8::Local<v8::Function> iterator,
                        v8::Local<v8::Value> context, v8::Local<v8::Array> &retVal)
{
    Isolate* isolate = Isolate::current();
    v8::Local<v8::Array> arr = v8::Array::New(isolate->m_isolate);

    if (!list->IsObject())
    {
        retVal = arr;
        return 0;
    }

    v8::Local<v8::Value> args[3];
    args[2] = list;

    v8::Local<v8::Object> o = v8::Local<v8::Object>::Cast(list);
    v8::Local<v8::Value> v = o->Get(isolate->NewFromUtf8("length"));
    int32_t cnt = 0;

    if (IsEmpty(v))
    {
        v8::Local<v8::Array> keys = o->GetPropertyNames();
        int32_t len = keys->Length();
        int32_t i;

        for (i = 0; i < len; i ++)
        {
            args[1] = keys->Get(i);
            args[0] = o->Get(args[1]);

            v = iterator->Call(context, 3, args);
            if (v.IsEmpty())
                return CALL_E_JAVASCRIPT;

            arr->Set(cnt ++, v);
        }
    }
    else
    {
        int32_t len = v->Int32Value();

        int32_t i;

        for (i = 0; i < len; i ++)
        {
            args[1] = v8::Int32::New(isolate->m_isolate, i);
            args[0] = o->Get(args[1]);

            v = iterator->Call(context, 3, args);
            if (v.IsEmpty())
                return CALL_E_JAVASCRIPT;

            arr->Set(cnt ++, v);
        }
    }

    retVal = arr;

    return 0;
}

result_t util_base::reduce(v8::Local<v8::Value> list, v8::Local<v8::Function> iterator,
                           v8::Local<v8::Value> memo, v8::Local<v8::Value> context,
                           v8::Local<v8::Value> &retVal)
{
    if (!list->IsObject())
    {
        retVal = memo;
        return 0;
    }

    v8::Local<v8::Value> args[4];
    args[3] = list;

    Isolate* isolate = Isolate::current();
    v8::Local<v8::Object> o = v8::Local<v8::Object>::Cast(list);
    v8::Local<v8::Value> v = o->Get(isolate->NewFromUtf8("length"));

    if (IsEmpty(v))
    {
        v8::Local<v8::Array> keys = o->GetPropertyNames();
        int32_t len = keys->Length();
        int32_t i;

        for (i = 0; i < len; i ++)
        {
            args[2] = keys->Get(i);
            args[1] = o->Get(args[2]);

            args[0] = memo;

            memo = iterator->Call(context, 4, args);
            if (memo.IsEmpty())
                return CALL_E_JAVASCRIPT;
        }
    }
    else
    {
        int32_t len = v->Int32Value();

        int32_t i;

        for (i = 0; i < len; i ++)
        {
            args[2] = v8::Int32::New(isolate->m_isolate, i);
            args[1] = o->Get(args[2]);

            args[0] = memo;

            memo = iterator->Call(context, 4, args);
            if (memo.IsEmpty())
                return CALL_E_JAVASCRIPT;
        }
    }

    retVal = memo;

    return 0;
}

#define _STR(s) #s
#define STR(s)  _STR(s)

static const char s_version[] = "0.2.0";

result_t util_base::buildInfo(v8::Local<v8::Object> &retVal)
{
    Isolate* isolate = Isolate::current();
    retVal = v8::Object::New(isolate->m_isolate);

    retVal->Set(isolate->NewFromUtf8("fibjs"), isolate->NewFromUtf8(s_version));

#ifdef GIT_INFO
    retVal->Set(isolate->NewFromUtf8("git"), isolate->NewFromUtf8(GIT_INFO));
#endif

#if defined(__clang__)
    retVal->Set(isolate->NewFromUtf8("clang"),
                isolate->NewFromUtf8( STR(__clang_major__) "." STR(__clang_minor__)));
#elif defined(__GNUC__)
    retVal->Set(isolate->NewFromUtf8("gcc"),
                isolate->NewFromUtf8( STR(__GNUC__) "." STR(__GNUC_MINOR__) "." STR(__GNUC_PATCHLEVEL__)));
#elif defined(_MSC_VER)
    retVal->Set(isolate->NewFromUtf8("msvc"),
                isolate->NewFromUtf8( STR(_MSC_VER)));
#endif

    retVal->Set(isolate->NewFromUtf8("date"),
                isolate->NewFromUtf8(__DATE__ " " __TIME__));

#ifndef NDEBUG
    retVal->Set(isolate->NewFromUtf8("debug"), v8::True(isolate->m_isolate));
#endif

    {
        v8::Local<v8::Object> vender = v8::Object::New(isolate->m_isolate);
        char str[64];

        retVal->Set(isolate->NewFromUtf8("vender"), vender);

        vender->Set(isolate->NewFromUtf8("ev"),
                    isolate->NewFromUtf8( STR(EV_VERSION_MAJOR) "." STR(EV_VERSION_MINOR)));

        vender->Set(isolate->NewFromUtf8("expat"),
                    isolate->NewFromUtf8( STR(XML_MAJOR_VERSION) "." STR(XML_MINOR_VERSION) "." STR(XML_MICRO_VERSION)));

        vender->Set(isolate->NewFromUtf8("gd"), isolate->NewFromUtf8(GD_VERSION_STRING));
        vender->Set(isolate->NewFromUtf8("jpeg"), isolate->NewFromUtf8(
                        STR(JPEG_LIB_VERSION_MAJOR) "." STR(JPEG_LIB_VERSION_MINOR)));
        sprintf(str, "%d.%d", leveldb::kMajorVersion, leveldb::kMinorVersion);
        vender->Set(isolate->NewFromUtf8("leveldb"), isolate->NewFromUtf8( str));
        vender->Set(isolate->NewFromUtf8("mongo"), isolate->NewFromUtf8(
                        STR(MONGO_MAJOR) "." STR(MONGO_MINOR)));
        vender->Set(isolate->NewFromUtf8("pcre"), isolate->NewFromUtf8(
                        STR(PCRE_MAJOR) "." STR(PCRE_MINOR)));
        vender->Set(isolate->NewFromUtf8("png"), isolate->NewFromUtf8(PNG_LIBPNG_VER_STRING));
        vender->Set(isolate->NewFromUtf8("mbedtls"), isolate->NewFromUtf8(MBEDTLS_VERSION_STRING));
        vender->Set(isolate->NewFromUtf8("snappy"),
                    isolate->NewFromUtf8( STR(SNAPPY_MAJOR) "." STR(SNAPPY_MINOR) "." STR(SNAPPY_PATCHLEVEL)));
        vender->Set(isolate->NewFromUtf8("sqlite"), isolate->NewFromUtf8(SQLITE_VERSION));
        vender->Set(isolate->NewFromUtf8("tiff"), isolate->NewFromUtf8(TIFFLIB_VERSION_STR));
        vender->Set(isolate->NewFromUtf8("uuid"), isolate->NewFromUtf8("1.6.2"));
        vender->Set(isolate->NewFromUtf8("v8"), isolate->NewFromUtf8(v8::V8::GetVersion()));
        vender->Set(isolate->NewFromUtf8("zlib"), isolate->NewFromUtf8(ZLIB_VERSION));
    }

    return 0;
}

}
