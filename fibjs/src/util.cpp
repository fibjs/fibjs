/*
 * utils.cpp
 *
 *  Created on: Dec 10, 2013
 *      Author: lion
 */

#include "ifs/util.h"
#include "ifs/encoding.h"
#include "ifs/Int64.h"
#include <v8/v8.h>
#include <zlib.h>
#include <sqlite3.h>
#include <ev.h>
#include <gd.h>
#include <jpeglib.h>
#include <png.h>
#include <pcre.h>
#include <tiffvers.h>
#include <mongo/mongo.h>
#include <polarssl/version.h>
#include <snappy.h>
#include <leveldb/db.h>
#include "QuickArray.h"
#include <map>

#ifndef WIN32
#include "gitinfo.h"
#endif

namespace fibjs
{

inline void newline(std::string &strBuffer, int32_t padding)
{
    static char s_spc[] = "                                                                ";
    int32_t n, n1;

    strBuffer.append("\n", 1);
    if (padding > 0)
    {
        n = padding;
        while (n)
        {
            n1 = n > 64 ? 64 : n;
            strBuffer.append(s_spc, n1);
            n -= n1;
        }
    }
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

void string_format(std::string &strBuffer, v8::Local<v8::Value> v)
{
    std::string s;
    encoding_base::jsonEncode(v, s);
    strBuffer += s;
}

std::string json_format(v8::Local<v8::Value> obj)
{
    std::string strBuffer;

    QuickArray<_item> stk;
    v8::Local<v8::Value> v = obj;
    int32_t padding = 0;
    const int32_t tab_size = 2;
    _item *it = NULL;

    while (true)
    {
        if (v.IsEmpty())
            strBuffer += "undefined";
        else if (v->IsUndefined() || v->IsNull() || v->IsDate() ||
                 v->IsBoolean() || v->IsBooleanObject())
            strBuffer += *v8::String::Utf8Value(v);
        else if (v->IsFunction())
            strBuffer += "[Function]";
        else if (v->IsNumber() || v->IsNumberObject())
            strBuffer += *v8::String::Utf8Value(v->ToNumber());
        else if (v->IsString() || v->IsStringObject())
            string_format(strBuffer, v);
        else if (v->IsRegExp())
        {
            v8::Local<v8::RegExp> re = v8::Local<v8::RegExp>::Cast(v);
            v8::Local<v8::String> src = re->GetSource();
            v8::RegExp::Flags flgs = re->GetFlags();

            strBuffer += '/';
            strBuffer += *v8::String::Utf8Value(src);
            strBuffer += '/';

            if (flgs & v8::RegExp::kIgnoreCase)
                strBuffer += 'i';
            if (flgs & v8::RegExp::kGlobal)
                strBuffer += 'g';
            if (flgs & v8::RegExp::kMultiline)
                strBuffer += 'm';
        }
        else if (v->IsObject())
        {
            int32_t sz = (int32_t)stk.size();
            int32_t i;
            bool bCircular = false;

            for (i = 0; i < sz; i ++)
            {
                if (v->StrictEquals(stk[i].val))
                {
                    bCircular = true;
                    break;
                }
            }

            do
            {
                if (bCircular)
                {
                    strBuffer += "[Circular]";
                    break;
                }

                v8::Local<v8::Object> obj = v->ToObject();

                obj_ptr<Buffer_base> buf = Buffer_base::getInstance(v);
                if (buf)
                {
                    std::string s;
                    buf->base64(s);
                    strBuffer += s;
                    break;
                }

                obj_ptr<Int64_base> int64Val = Int64_base::getInstance(v);
                if (int64Val)
                {
                    std::string s;
                    int64Val->toString(10, s);
                    strBuffer += s;
                    break;
                }

                v8::Local<v8::Value> toArray = obj->Get(v8::String::NewFromUtf8(isolate, "toArray"));
                if (!IsEmpty(toArray))
                {
                    v = v8::Local<v8::Function>::Cast(toArray)->Call(obj, 0, NULL);
                    obj = v->ToObject();
                }

                if (v->IsArray())
                {
                    v8::Local<v8::Array> keys = v8::Local<v8::Array>::Cast(v);
                    int32_t len = keys->Length();

                    if (len == 0)
                        strBuffer += "[]";
                    else
                    {
                        if (len == 1 && v->StrictEquals(keys->Get(0)))
                            strBuffer += "[Circular]";
                        else
                        {
                            stk.resize(sz + 1);
                            it = &stk[sz];

                            it->val = v;

                            it->keys = keys;
                            it->len = len;

                            strBuffer += '[';
                            padding += tab_size;
                        }
                    }
                    break;
                }

                v8::Local<v8::Array> keys = obj->GetPropertyNames();
                int32_t len = keys->Length();

                if (len == 0)
                    strBuffer += "{}";
                else
                {
                    if (len == 1 && v->StrictEquals(obj->Get(keys->Get(0))))
                        strBuffer += "[Circular]";
                    else
                    {
                        stk.resize(sz + 1);
                        it = &stk[sz];

                        it->val = v;

                        it->obj = obj;
                        it->keys = keys;
                        it->len = len;

                        strBuffer += '{';
                        padding += tab_size;
                    }
                }
            }
            while (false);
        }

        if (it)
        {
            while (it->pos == it->len)
            {
                padding -= tab_size;
                newline(strBuffer, padding);
                strBuffer += it->obj.IsEmpty() ? ']' : '}';

                int32_t sz = (int32_t)stk.size();
                if (sz == 1)
                    return strBuffer;
                stk.resize(sz - 1);
                it = &stk[sz - 2];
            }

            if (it->pos)
                strBuffer += ',';
            newline(strBuffer, padding);

            v = it->keys->Get(it->pos ++);

            if (!it->obj.IsEmpty())
            {
                v8::TryCatch try_catch;

                string_format(strBuffer, v);
                strBuffer += ": ";
                v = it->obj->Get(v);
            }
        }
        else
            break;
    }

    return strBuffer;
}

result_t util_base::format(const char *fmt, const v8::FunctionCallbackInfo<v8::Value> &args,
                           std::string &retVal)
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

    while (1)
    {
        s1 = s;
        while ((ch = *s++) && (ch != '%'));

        retVal.append(s1, s - s1 - 1);

        if (ch == '%')
        {
            switch (ch = *s++)
            {
            case 's':
                if (idx < argc)
                {
                    v8::String::Utf8Value s(args[idx++]);
                    if (*s)
                        retVal.append(*s);
                }
                else
                    retVal.append("%s", 2);
                break;
            case 'd':
                if (idx < argc)
                {
                    v8::String::Utf8Value s(args[idx++]->ToNumber());
                    if (*s)
                        retVal.append(*s);
                }
                else
                    retVal.append("%d", 2);
                break;
            case 'j':
                if (idx < argc)
                {
                    std::string s;
                    s = json_format(args[idx++]);
                    retVal.append(s);
                }
                else
                    retVal.append("%j", 2);
                break;
            default:
                retVal.append("%", 1);
            case '%':
                retVal.append(&ch, 1);
                break;
            }
        }
        else
            break;
    }

    while (idx < argc)
    {
        if (!retVal.empty())
            retVal.append(" ", 1);

        bool v;

        isString(args[idx], v);

        if (v)
        {
            v8::String::Utf8Value s(args[idx++]);
            retVal.append(*s);
        }
        else
        {
            std::string s;
            s = json_format(args[idx++]);

            retVal.append(s);
        }
    }

    return 0;
}

result_t util_base::format(const v8::FunctionCallbackInfo<v8::Value> &args, std::string &retVal)
{
    return format(NULL, args, retVal);
}

result_t util_base::isEmpty(v8::Local<v8::Value> v, bool &retVal)
{
    if (v->IsNull())
    {
        retVal = true;
        return 0;
    }

    if (v->IsString() || v->IsStringObject())
    {
        retVal = v8::Local<v8::String>::Cast(v)->Length() == 0;
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

result_t util_base::has(v8::Local<v8::Value> v, const char *key, bool &retVal)
{
    if (v->IsUndefined() || v->IsNull())
    {
        retVal = false;
        return 0;
    }

    if (!v->IsObject())
        return CHECK_ERROR(CALL_E_TYPEMISMATCH);

    v8::Local<v8::Object> obj = v->ToObject();
    retVal = obj->HasOwnProperty(v8::String::NewFromUtf8(isolate, key));
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
        retVal = v8::Array::New(isolate);

    return 0;
}

result_t util_base::values(v8::Local<v8::Value> v, v8::Local<v8::Array> &retVal)
{
    if (v->IsObject())
    {
        v8::Local<v8::Object> obj = v->ToObject();
        v8::Local<v8::Array> keys = obj->GetPropertyNames();
        v8::Local<v8::Array> arr = v8::Array::New(isolate);

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
        retVal = v8::Array::New(isolate);

    return 0;
}

result_t util_base::clone(v8::Local<v8::Value> v, v8::Local<v8::Value> &retVal)
{
    if (v->IsObject())
        retVal = v->ToObject()->Clone();
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
    if (v->IsUndefined() || v->IsNull())
    {
        retVal = v8::Object::New(isolate);
        return 0;
    }

    if (!v->IsObject())
        return CHECK_ERROR(CALL_E_TYPEMISMATCH);

    v8::Local<v8::Object> obj = v->ToObject();
    v8::Local<v8::Object> obj1 = v8::Object::New(isolate);
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
    if (v->IsUndefined() || v->IsNull())
    {
        retVal = v8::Object::New(isolate);
        return 0;
    }

    if (!v->IsObject())
        return CHECK_ERROR(CALL_E_TYPEMISMATCH);

    v8::Local<v8::Object> obj = v->ToObject();

    std::map<std::string, bool> _map;
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
                std::string k;
                hr = GetArgumentValue(arr->Get(j), k);
                if (hr < 0)
                    return CHECK_ERROR(hr);

                _map.insert(std::pair<std::string, bool>(k, true));
            }
        }
        else
        {
            std::string k;
            hr = GetArgumentValue(args[i], k);
            if (hr < 0)
                return CHECK_ERROR(hr);

            _map.insert(std::pair<std::string, bool>(k, true));
        }
    }

    v8::Local<v8::Array> keys = obj->GetPropertyNames();
    int32_t len = keys->Length();
    v8::Local<v8::Object> obj1 = v8::Object::New(isolate);

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
    v8::Local<v8::Array> arr = v8::Array::New(isolate);
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
    if (v->IsUndefined() || v->IsNull())
    {
        retVal = v8::Undefined(isolate);
        return 0;
    }

    if (!v->IsArray())
        return CHECK_ERROR(CALL_E_TYPEMISMATCH);

    v8::Local<v8::Array> arr = v8::Local<v8::Array>::Cast(v);
    int32_t len = arr->Length();

    if (len == 0)
    {
        retVal = v8::Undefined(isolate);
        return 0;
    }

    retVal = arr->Get(0);

    return 0;
}

result_t util_base::first(v8::Local<v8::Value> v, int32_t n, v8::Local<v8::Value> &retVal)
{
    if (v->IsUndefined() || v->IsNull() || n <= 0)
    {
        retVal = v8::Array::New(isolate);
        return 0;
    }

    if (!v->IsArray())
        return CHECK_ERROR(CALL_E_TYPEMISMATCH);

    v8::Local<v8::Array> arr = v8::Local<v8::Array>::Cast(v);
    int32_t len = arr->Length();
    int32_t i;

    if (n > len)
        n = len;

    v8::Local<v8::Array> arr1 = v8::Array::New(isolate);

    for (i = 0; i < n; i ++)
        arr1->Set(i, arr->Get(i));

    retVal = arr1;

    return 0;
}

result_t util_base::last(v8::Local<v8::Value> v, v8::Local<v8::Value> &retVal)
{
    if (v->IsUndefined() || v->IsNull())
    {
        retVal = v8::Undefined(isolate);
        return 0;
    }

    if (!v->IsArray())
        return CHECK_ERROR(CALL_E_TYPEMISMATCH);

    v8::Local<v8::Array> arr = v8::Local<v8::Array>::Cast(v);
    int32_t len = arr->Length();

    if (len == 0)
    {
        retVal = v8::Undefined(isolate);
        return 0;
    }

    retVal = arr->Get(len - 1);
    return 0;
}

result_t util_base::last(v8::Local<v8::Value> v, int32_t n, v8::Local<v8::Value> &retVal)
{
    if (v->IsUndefined() || v->IsNull() || n <= 0)
    {
        retVal = v8::Array::New(isolate);
        return 0;
    }

    if (!v->IsArray())
        return CHECK_ERROR(CALL_E_TYPEMISMATCH);

    v8::Local<v8::Array> arr = v8::Local<v8::Array>::Cast(v);
    int32_t len = arr->Length();
    int32_t i;

    if (n > len)
        n = len;

    v8::Local<v8::Array> arr1 = v8::Array::New(isolate);

    for (i = 0; i < n; i ++)
        arr1->Set(i, arr->Get(len - n + i));

    retVal = arr1;

    return 0;
}

result_t util_base::unique(v8::Local<v8::Value> v, bool sorted, v8::Local<v8::Array> &retVal)
{
    if (v->IsUndefined() || v->IsNull())
    {
        retVal = v8::Array::New(isolate);
        return 0;
    }

    if (!v->IsArray())
        return CHECK_ERROR(CALL_E_TYPEMISMATCH);

    v8::Local<v8::Array> arr1 = v8::Array::New(isolate);
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
    v8::Local<v8::Array> arr = v8::Array::New(isolate);
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

result_t util_base::without(v8::Local<v8::Array> arr,
                            const v8::FunctionCallbackInfo<v8::Value> &args,
                            v8::Local<v8::Array> &retVal)
{
    v8::Local<v8::Array> arr1 = v8::Array::New(isolate);
    int32_t len = arr->Length();
    int32_t argc = args.Length();
    int32_t i, j, n = 0;

    for (i = 0; i < len; i ++)
    {
        v8::Local<v8::Value> v = arr->Get(i);

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
    v8::Local<v8::Array> arr1 = v8::Array::New(isolate);
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

#define _STR(s) #s
#define STR(s)  _STR(s)

static const char s_version[] = "0.1.0";

result_t util_base::buildInfo(v8::Local<v8::Object> &retVal)
{
    retVal = v8::Object::New(isolate);

    retVal->Set(v8::String::NewFromUtf8(isolate, "fibjs"), v8::String::NewFromUtf8(isolate, s_version));

#ifdef GIT_INFO
    retVal->Set(v8::String::NewFromUtf8(isolate, "git"), v8::String::NewFromUtf8(isolate, GIT_INFO));
#endif

    retVal->Set(v8::String::NewFromUtf8(isolate, "build"),
                v8::String::NewFromUtf8(isolate, __DATE__ " " __TIME__));

#ifndef NDEBUG
    retVal->Set(v8::String::NewFromUtf8(isolate, "debug"), v8::True(isolate));
#endif

    {
        v8::Local<v8::Object> vender = v8::Object::New(isolate);
        char str[64];

        retVal->Set(v8::String::NewFromUtf8(isolate, "vender"), vender);

        vender->Set(v8::String::NewFromUtf8(isolate, "ev"),
                    v8::String::NewFromUtf8(isolate,  STR(EV_VERSION_MAJOR) "." STR(EV_VERSION_MINOR)));
        vender->Set(v8::String::NewFromUtf8(isolate, "gd"), v8::String::NewFromUtf8(isolate, GD_VERSION_STRING));
        vender->Set(v8::String::NewFromUtf8(isolate, "jpeg"), v8::String::NewFromUtf8(isolate,
                    STR(JPEG_LIB_VERSION_MAJOR) "." STR(JPEG_LIB_VERSION_MINOR)));
        sprintf(str, "%d.%d", leveldb::kMajorVersion, leveldb::kMinorVersion);
        vender->Set(v8::String::NewFromUtf8(isolate, "leveldb"), v8::String::NewFromUtf8(isolate,  str));
        vender->Set(v8::String::NewFromUtf8(isolate, "mongo"), v8::String::NewFromUtf8(isolate,
                    STR(MONGO_MAJOR) "." STR(MONGO_MINOR)));
        vender->Set(v8::String::NewFromUtf8(isolate, "pcre"), v8::String::NewFromUtf8(isolate,
                    STR(PCRE_MAJOR) "." STR(PCRE_MINOR)));
        vender->Set(v8::String::NewFromUtf8(isolate, "png"), v8::String::NewFromUtf8(isolate, PNG_LIBPNG_VER_STRING));
        vender->Set(v8::String::NewFromUtf8(isolate, "polarssl"), v8::String::NewFromUtf8(isolate, POLARSSL_VERSION_STRING));
        vender->Set(v8::String::NewFromUtf8(isolate, "snappy"),
                    v8::String::NewFromUtf8(isolate,  STR(SNAPPY_MAJOR) "." STR(SNAPPY_MINOR) "." STR(SNAPPY_PATCHLEVEL)));
        vender->Set(v8::String::NewFromUtf8(isolate, "sqlite"), v8::String::NewFromUtf8(isolate, SQLITE_VERSION));
        vender->Set(v8::String::NewFromUtf8(isolate, "tiff"), v8::String::NewFromUtf8(isolate, TIFFLIB_VERSION_STR));
        vender->Set(v8::String::NewFromUtf8(isolate, "uuid"), v8::String::NewFromUtf8(isolate, "1.6.2"));
        vender->Set(v8::String::NewFromUtf8(isolate, "v8"), v8::String::NewFromUtf8(isolate, v8::V8::GetVersion()));
        vender->Set(v8::String::NewFromUtf8(isolate, "zlib"), v8::String::NewFromUtf8(isolate, ZLIB_VERSION));
    }

    return 0;
}

}
