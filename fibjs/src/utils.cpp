/*
 * utils.cpp
 *
 *  Created on: Dec 10, 2013
 *      Author: lion
 */

#include "ifs/util.h"
#include "ifs/encoding.h"
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

#ifndef WIN32
#include "gitinfo.h"
#endif

namespace fibjs
{

inline void newline(std::string &strBuffer, int padding)
{
    static char s_spc[] = "                                                                ";
    int n, n1;

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
    const int tab_size = 2;
    _item *it = NULL;

    while (true)
    {
        if (v->IsUndefined() || v->IsNull() || v->IsDate() ||
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
                if (v->Equals(stk[i].val))
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
                        if (len == 1 && v->Equals(keys->Get(0)))
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
                    if (len == 1 && v->Equals(obj->Get(keys->Get(0))))
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
    int argc = args.Length();
    int idx = 1;

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
