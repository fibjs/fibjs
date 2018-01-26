/*
 * util_format.cpp
 *
 *  Created on: Jun 26, 2013
 *      Author: lion
 */

#include "object.h"
#include "ifs/util.h"
#include "ifs/encoding.h"
#include "ifs/Int64.h"
#include "QuickArray.h"
#include "StringBuffer.h"
#include <map>

namespace fibjs {

inline void newline(StringBuffer& strBuffer, int32_t padding)
{
    strBuffer.append('\n');
    if (padding > 0)
        strBuffer.append(exlib::string(padding, ' '));
}

class _item {
public:
    _item()
    {
        pos = 0;
        len = 0;
        mode = 0;
    }

    _item(const _item& v)
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

void string_format(StringBuffer& strBuffer, v8::Local<v8::Value> v)
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
    _item* it = NULL;
    v8::Local<v8::Context> _context = isolate->context();

    while (true) {
        if (v.IsEmpty())
            strBuffer.append("undefined");
        else if (v->IsUndefined() || v->IsNull() || v->IsDate() || v->IsBoolean() || v->IsBooleanObject())
            strBuffer.append(*v8::String::Utf8Value(v));
        else if (v->IsNumber() || v->IsNumberObject())
            strBuffer.append(*v8::String::Utf8Value(v->ToNumber(_context).ToLocalChecked()));
        else if (v->IsString() || v->IsStringObject())
            string_format(strBuffer, v);
        else if (v->IsRegExp()) {
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
        } else if (v->IsObject()) {
            do {
                v8::Local<v8::Object> obj = v->ToObject();

                if (obj->IsNativeError()) {
                    v8::String::Utf8Value msg(obj);
                    strBuffer.append(*msg);
                    break;
                }

                obj_ptr<Buffer_base> buf = Buffer_base::getInstance(v);
                if (buf) {
                    static char hexs[] = "0123456789abcdef";
                    exlib::string data;
                    exlib::string s;
                    int32_t len, i;

                    buf->toString(data);
                    len = (int32_t)data.length();

                    s.resize(len * 3 + 8);
                    memcpy(&s[0], "<Buffer", 7);

                    for (i = 0; i < len; i++) {
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
                if (int64Val) {
                    exlib::string s;
                    int64Val->toString(10, s);
                    strBuffer.append(s);
                    break;
                }

                v8::Local<v8::Array> keys = obj->GetPropertyNames();

                if (v->IsFunction() && keys->Length() == 0) {
                    strBuffer.append("[Function]");
                    break;
                }

                int32_t i, sz1 = (int32_t)vals.size();
                for (i = 0; i < sz1; i++)
                    if (vals[i]->Equals(obj))
                        break;

                if (i < sz1) {
                    strBuffer.append("[Circular]");
                    break;
                }

                vals.append(obj);

                v8::Local<v8::Value> toArray = obj->Get(isolate->NewString("toArray"));
                if (!IsEmpty(toArray) && toArray->IsFunction()) {
                    TryCatch try_catch;
                    v8::Local<v8::Value> v1 = v8::Local<v8::Function>::Cast(toArray)->Call(obj, 0, NULL);
                    if (!IsEmpty(v1) && v1->IsObject()) {
                        v = v1;
                        obj = v1->ToObject();
                    }
                }

                int32_t sz = (int32_t)stk.size();

                if (v->IsArray()) {
                    v8::Local<v8::Array> array = v8::Local<v8::Array>::Cast(v);
                    int32_t len = array->Length();

                    if (len == 0)
                        strBuffer.append("[]");
                    else {
                        if (len == 1 && v->StrictEquals(array->Get(0)))
                            strBuffer.append("[Circular]");
                        else {
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

                if (v->IsTypedArray()) {
                    v8::Local<v8::TypedArray> typedarray = v8::Local<v8::TypedArray>::Cast(v);
                    int32_t len = (int32_t)typedarray->Length();

                    if (len == 0)
                        strBuffer.append("[]");
                    else {
                        v8::Local<v8::Array> array = v8::Array::New(isolate->m_isolate);

                        for (i = 0; i < len; i++)
                            array->Set(i, typedarray->Get(i));

                        if (len == 1 && v->StrictEquals(array->Get(0)))
                            strBuffer.append("[Circular]");
                        else {
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
                else {
                    if (len == 1 && v->StrictEquals(obj->Get(keys->Get(0))))
                        strBuffer.append("[Circular]");
                    else {
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
            } while (false);
        }

        if (it) {
            while (it && it->pos == it->len) {
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

            v = it->keys->Get(it->pos++);

            if (!it->obj.IsEmpty()) {
                TryCatch try_catch;

                string_format(strBuffer, v);
                strBuffer.append(": ");
                v = it->obj->Get(v);
            }
        } else
            break;
    }

    return strBuffer.str();
}

result_t util_base::inspect(v8::Local<v8::Object> obj, v8::Local<v8::Object> options, exlib::string& retVal)
{
    retVal = json_format(obj);
    return 0;
}

result_t util_base::format(exlib::string fmt, OptArgs args, exlib::string& retVal)
{
    const char* s1;
    char ch;
    int32_t argc = args.Length();
    int32_t idx = 0;
    v8::Local<v8::Context> _context = Isolate::current()->context();

    if (argc == 0) {
        retVal = fmt;
        return 0;
    }

    const char* s = fmt.c_str();
    const char* s_end = s + fmt.length();

    while (s < s_end) {
        ch = 0;
        s1 = s;
        while ((s < s_end) && ((ch = *s) != '%'))
            s++;

        retVal.append(s1, s - s1);

        if (ch == '%') {
            s++;

            switch (ch = *s++) {
            case 's':
                if (idx < argc) {
                    v8::String::Utf8Value s(args[idx++]);
                    if (*s)
                        retVal.append(*s, s.length());
                } else
                    retVal.append("%s", 2);
                break;
            case 'd':
                if (idx < argc) {
                    v8::String::Utf8Value s(args[idx++]->ToNumber(_context).ToLocalChecked());
                    if (*s)
                        retVal.append(*s, s.length());
                } else
                    retVal.append("%d", 2);
                break;
            case 'j':
                if (idx < argc) {
                    exlib::string s;
                    s = json_format(args[idx++]);
                    retVal.append(s);
                } else
                    retVal.append("%j", 2);
                break;
            default:
                retVal.append(1, '%');
            case '%':
                retVal.append(1, ch);
                break;
            }
        } else
            break;
    }

    while (idx < argc) {
        if (!retVal.empty())
            retVal.append(1, ' ');

        bool bIsStr;
        v8::Local<v8::Value> v = args[idx++];

        util_base::isString(v, bIsStr);

        if (bIsStr) {
            v8::String::Utf8Value s(v);
            retVal.append(*s, s.length());
        } else {
            exlib::string s;
            s = json_format(v);

            retVal.append(s);
        }
    }

    return 0;
}

result_t util_base::format(OptArgs args, exlib::string& retVal)
{
    return format("", args, retVal);
}
}