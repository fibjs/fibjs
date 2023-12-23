/*
 * util_format.cpp
 *
 *  Created on: Jun 26, 2013
 *      Author: lion
 */

#include "ifs/encoding.h"
#include "util.h"

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

void string_format(StringBuffer& strBuffer, v8::Local<v8::Value> v, bool color, int32_t maxStringLength)
{
    exlib::string ret;

    Isolate* isolate = Isolate::current();
    exlib::string s = isolate->toString(v);
    exlib::wstring32 str32 = utf8to32String(s);

    int32_t sz = (int32_t)str32.length();
    if (sz > maxStringLength) {
        char str_buf[256];
        int32_t cnt = sz - maxStringLength;
        if (cnt > 1)
            snprintf(str_buf, sizeof(str_buf), "... %d more characters", cnt);
        else
            snprintf(str_buf, sizeof(str_buf), "... 1 more character");

        if (maxStringLength < 0)
            str32.resize(sz + maxStringLength);
        else
            str32 = str32.substr(0, maxStringLength);

        s = utf32to8String(str32);
        json_base::encode(isolate->NewString(s), ret);
        ret.append(str_buf);
    } else {
        json_base::encode(v, ret);
    }

    strBuffer.append(color_string(COLOR_GREEN, ret, color));
}

#define MAX_BUFFER_ITEM 50

exlib::string json_format(v8::Local<v8::Value> obj, bool color, int32_t depth, int32_t maxArrayLength, int32_t maxStringLength)
{
    StringBuffer strBuffer;

    Isolate* isolate = Isolate::current();
    v8::Local<v8::Context> _context = isolate->context();

    QuickArray<_item> stk;
    QuickArray<v8::Local<v8::Object>> vals;
    v8::Local<v8::Value> v = obj;
    int32_t padding = 0;
    const int32_t tab_size = 2;
    _item* it = NULL;

    while (true) {
        if (v.IsEmpty())
            strBuffer.append(color_string(COLOR_TITLE, "undefined", color));
        else if (v->IsUndefined() || v->IsNull())
            strBuffer.append(color_string(COLOR_TITLE, isolate->toString(v), color));
        else if (v->IsDate())
            strBuffer.append(color_string(COLOR_MAGENTA, isolate->toString(v), color));
        else if (v->IsBoolean() || v->IsBooleanObject())
            strBuffer.append(color_string(COLOR_YELLOW, isolate->toString(v), color));
        else if (v->IsNumber() || v->IsNumberObject())
            strBuffer.append(color_string(COLOR_YELLOW, isolate->toString(v), color));
        else if (v->IsBigInt() || v->IsBigIntObject())
            strBuffer.append(color_string(COLOR_YELLOW, isolate->toString(v) + 'n', color));
        else if (v->IsString() || v->IsStringObject())
            string_format(strBuffer, v, color, maxStringLength);
        else if (v->IsRegExp()) {
            exlib::string s;
            v8::Local<v8::RegExp> re = v8::Local<v8::RegExp>::Cast(v);
            v8::Local<v8::String> src = re->GetSource();
            v8::RegExp::Flags flgs = re->GetFlags();

            s.append(1, '/');
            s.append(isolate->toString(src));
            s.append(1, '/');

            if (flgs & v8::RegExp::kIgnoreCase)
                s.append(1, 'i');
            if (flgs & v8::RegExp::kGlobal)
                s.append(1, 'g');
            if (flgs & v8::RegExp::kMultiline)
                s.append(1, 'm');

            strBuffer.append(color_string(COLOR_RED, s, color));
        } else if (v->IsPromise()) {
            strBuffer.append(color_string(COLOR_CYAN, "[Promise]", color));
        } else if (v->IsNativeError()) {
            v8::Local<v8::Object> obj = v8::Local<v8::Object>::Cast(v);
            exlib::string s(isolate->toString(JSValue(obj->Get(_context, isolate->NewString("stack")))));
            strBuffer.append(color_string(COLOR_LIGHTRED, s, color));
        } else if (v->IsSymbol()) {
            exlib::string s("Symbol(");

            v8::Local<v8::Symbol> _symbol = v8::Local<v8::Symbol>::Cast(v);
            v8::Local<v8::Value> _name = _symbol->Description(isolate->m_isolate);

            if (!_name->IsUndefined())
                s.append(isolate->toString(_name));

            strBuffer.append(color_string(COLOR_GREEN, s + ')', color));
        } else if (v->IsObject()) {
            bool isFunction = false;

            if (v->IsFunction()) {
                exlib::string s(v->IsAsyncFunction() ? "[AsyncFunction" : "[Function");
                v8::String::Utf8Value n(isolate->m_isolate, v8::Local<v8::Function>::Cast(v)->GetName());

                if (n.length()) {
                    s.append(1, ' ');
                    s.append(*n, n.length());
                }
                strBuffer.append(color_string(COLOR_CYAN, s + ']', color));
                isFunction = true;
            }

            do {
                v8::Local<v8::Object> obj = v8::Local<v8::Object>::Cast(v);

                obj_ptr<Buffer> buf = Buffer::getInstance(v);
                if (buf) {
                    static char hexs[] = "0123456789abcdef";
                    exlib::string s;
                    int32_t i, p;

                    const uint8_t* data = buf->data();
                    int32_t len = buf->length();

                    if (len <= MAX_BUFFER_ITEM)
                        s.resize(len * 3 + 8);
                    else
                        s.resize(MAX_BUFFER_ITEM * 3 + 8 + 32);

                    char* _s = s.data();

                    memcpy(_s, "<Buffer", 7);

                    for (i = 0, p = 7; i < len; i++) {
                        if (i >= MAX_BUFFER_ITEM) {
                            int32_t cnt = len - i;
                            if (cnt > 1)
                                p += snprintf(_s + p, s.length() - p, " ... %d more bytes", cnt);
                            else {
                                memcpy(_s + p, " ... 1 more byte", 16);
                                p += 16;
                            }
                            break;
                        }

                        int32_t ch = data[i];

                        _s[p++] = ' ';
                        _s[p++] = hexs[ch >> 4];
                        _s[p++] = hexs[ch & 0xf];
                    }

                    _s[p++] = '>';
                    s.resize(p);

                    strBuffer.append(s);
                    break;
                }

                JSArray keys;

                if (obj->IsMap()) {
                    strBuffer.append(color_string(COLOR_CYAN, "[Map] ", color));
                    v8::Local<v8::Map> m = v8::Local<v8::Map>::Cast(obj);
                    JSArray vs = m->AsArray();
                    int32_t len = vs->Length() / 2;

                    if (len > 0) {
                        keys = v8::Array::New(isolate->m_isolate);
                        for (int32_t i = 0; i < len; i++)
                            keys->Set(_context, i, JSValue(vs->Get(_context, i * 2))).IsJust();
                    } else
                        keys = vs;
                } else
                    keys = obj->GetPropertyNames(_context);

                if (keys.IsEmpty()) {
                    if (!isFunction)
                        strBuffer.append("{}");
                    break;
                }

                int32_t i, sz1 = (int32_t)vals.size();
                for (i = 0; i < sz1; i++)
                    if (vals[i]->StrictEquals(obj))
                        break;

                if (i < sz1) {
                    strBuffer.append(color_string(COLOR_CYAN, "[Circular]", color));
                    break;
                }

                vals.append(obj);

                JSValue v_toArray = obj->Get(_context, isolate->NewString("toArray"));
                if (v_toArray->IsFunction()) {
                    v8::Local<v8::Function> toArray = v8::Local<v8::Function>::Cast(v_toArray);

                    TryCatch try_catch;
                    v8::Local<v8::Value> v1 = toArray->Call(_context, obj, 0, NULL).FromMaybe(v8::Local<v8::Value>());
                    if (!IsEmpty(v1) && v1->IsObject()) {
                        v = v1;
                        obj = v8::Local<v8::Object>::Cast(v1);
                    }
                }

                int32_t sz = (int32_t)stk.size();

                if (v->IsSet()) {
                    strBuffer.append(color_string(COLOR_CYAN, "[Set] ", color));
                    v8::Local<v8::Set> s = v8::Local<v8::Set>::Cast(obj);
                    v = s->AsArray();
                }

                if (v->IsArray()) {
                    v8::Local<v8::Array> array = v8::Local<v8::Array>::Cast(v);
                    int32_t len = array->Length();

                    if (len == 0)
                        strBuffer.append("[]");
                    else {
                        if (sz >= (depth + 1)) {
                            strBuffer.append(color_string(COLOR_CYAN, "[Array]", color));
                            break;
                        }

                        stk.resize(sz + 1);
                        it = &stk[sz];

                        it->val = v;

                        it->keys = array;
                        it->len = len;

                        strBuffer.append('[');
                        padding += tab_size;
                    }
                    break;
                }

                if (v->IsTypedArray()) {
                    v8::Local<v8::TypedArray> typedarray = v8::Local<v8::TypedArray>::Cast(v);
                    int32_t len = (int32_t)typedarray->Length();

                    if (len == 0)
                        strBuffer.append("[]");
                    else {
                        if (sz >= (depth + 1)) {
                            strBuffer.append(color_string(COLOR_CYAN, "[TypedArray]", color));
                            break;
                        }

                        v8::Local<v8::Array> array = v8::Array::New(isolate->m_isolate);

                        for (i = 0; i < len; i++)
                            array->Set(_context, i, JSValue(typedarray->Get(_context, i))).IsJust();

                        stk.resize(sz + 1);
                        it = &stk[sz];

                        it->val = v;

                        it->keys = array;
                        it->len = len;

                        strBuffer.append('[');
                        padding += tab_size;
                    }
                    break;
                }

                int32_t len = keys->Length();

                if (len == 0) {
                    if (!isFunction)
                        strBuffer.append("{}");
                } else {
                    if (isFunction)
                        strBuffer.append(' ');

                    if (sz >= (depth + 1)) {
                        strBuffer.append(color_string(COLOR_CYAN, "[Object]", color));
                        break;
                    }

                    JSValue k1 = keys->Get(_context, 0);
                    JSValue v1;
                    if (!k1.IsEmpty())
                        v1 = obj->Get(_context, k1);

                    if (v1.IsEmpty())
                        v1 = v8::Undefined(isolate->m_isolate);

                    stk.resize(sz + 1);
                    it = &stk[sz];

                    it->val = v;

                    it->obj = obj;
                    it->keys = keys;
                    it->len = len;

                    strBuffer.append('{');
                    padding += tab_size;
                }
            } while (false);
        }

        if (it) {
            if (it->obj.IsEmpty() && it->pos >= maxArrayLength && it->len > it->pos) {
                char str_buf[256];

                if (maxArrayLength > 0)
                    strBuffer.append(',');
                newline(strBuffer, padding);

                int32_t cnt = it->len - it->pos;
                if (cnt > 1) {
                    snprintf(str_buf, sizeof(str_buf), "... %d more items", cnt);
                    strBuffer.append(str_buf);
                } else
                    strBuffer.append("... 1 more item");

                it->pos = it->len;
            }

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

            v = JSValue(it->keys->Get(_context, it->pos++));

            if (!it->obj.IsEmpty()) {
                TryCatch try_catch;
                string_format(strBuffer, v);

                if (it->obj->IsMap()) {
                    strBuffer.append(" => ");
                    v8::Local<v8::Map> m = v8::Local<v8::Map>::Cast(it->obj);
                    v = JSValue(m->Get(_context, v));
                } else {
                    strBuffer.append(": ");
                    v = JSValue(it->obj->Get(_context, v));
                }
            }
        } else
            break;
    }

    return strBuffer.str();
}

result_t util_format(exlib::string fmt, OptArgs args, bool color, exlib::string& retVal)
{
    const char* s1;
    char ch;
    int32_t argc = args.Length();
    int32_t idx = 0;

    if (argc == 0) {
        retVal = fmt;
        return 0;
    }

    Isolate* isolate = Isolate::current();

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
                if (idx < argc)
                    retVal.append(isolate->toString(args[idx++]));
                else
                    retVal.append("%s", 2);
                break;
            case 'd':
                if (idx < argc) {
                    v8::String::Utf8Value s(isolate->m_isolate, args[idx++]);
                    if (*s) {
                        int64_t n = atoi(*s);
                        v8::Local<v8::Value> v = v8::Number::New(isolate->m_isolate, (int32_t)n);

                        exlib::string s;
                        s = json_format(v, color);
                        retVal.append(s);
                    }
                } else
                    retVal.append("%d", 2);
                break;
            case 'j':
                if (idx < argc) {
                    exlib::string s;
                    s = json_format(args[idx++], color);
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

        if (bIsStr)
            retVal.append(isolate->toString(v));
        else {
            exlib::string s;
            s = json_format(v, color);

            retVal.append(s);
        }
    }

    return 0;
}

result_t util_base::format(exlib::string fmt, OptArgs args, exlib::string& retVal)
{
    return util_format(fmt, args, false, retVal);
}

result_t util_base::format(OptArgs args, exlib::string& retVal)
{
    return util_format("", args, false, retVal);
}
}