/*
 * bson_encoding.cpp
 *
 *  Created on: Oct 19, 2012
 *      Author: lion
 */

#include "encoding_bson.h"
#include "ifs/encoding.h"
#include "Buffer.h"
#include "MongoID.h"
#include "Int64.h"

namespace fibjs
{

inline const char *
ToCString(const v8::String::Utf8Value &value)
{
    return *value ? *value : "<string conversion failed>";
}

void encodeArray(bson *bb, const char *name, v8::Local<v8::Value> element);
bool encodeObject(bson *bb, const char *name, v8::Local<v8::Value> element,
                  bool doJson);

void encodeValue(bson *bb, const char *name, v8::Local<v8::Value> element,
                 bool doJson)
{
    if (element.IsEmpty() || element->IsUndefined() || element->IsFunction())
        ;
    else if (element->IsNull())
        bson_append_null(bb, name);
    else if (element->IsDate())
        bson_append_date(bb, name, (bson_date_t) element->NumberValue());
    else if (element->IsBoolean())
        bson_append_bool(bb, name, element->IsTrue());
    else if (element->IsNumber() || element->IsNumberObject())
    {
        double value = element->NumberValue();
        int64_t num = (int64_t) value;

        if (value == (double) num)
        {
            if (num >= -2147483648ll && num <= 2147483647ll)
                bson_append_int(bb, name, (int) num);
            else
                bson_append_long(bb, name, num);
        }
        else
            bson_append_double(bb, name, value);
    }
    else if (element->IsArray())
        encodeArray(bb, name, element);
    else if (element->IsRegExp())
    {
        v8::Local<v8::RegExp> re = v8::Local<v8::RegExp>::Cast(element);
        v8::Local<v8::String> src = re->GetSource();
        v8::RegExp::Flags flgs = re->GetFlags();
        char flgStr[4];
        char *p = flgStr;

        if (flgs & v8::RegExp::kIgnoreCase)
            *p++ = 'i';
        if (flgs & v8::RegExp::kGlobal)
            *p++ = 'g';
        if (flgs & v8::RegExp::kMultiline)
            *p++ = 'm';

        *p = 0;

        bson_append_regex(bb, name, *v8::String::Utf8Value(src), flgStr);
    }
    else if (element->IsObject())
    {
        {
            obj_ptr<Int64> num = (Int64 *)Int64_base::getInstance(element);

            if (num)
            {
                if (num->m_num >= -2147483648ll && num->m_num <= 2147483647ll)
                    bson_append_int(bb, name, (int) num->m_num);
                else
                    bson_append_long(bb, name, num->m_num);
                return;
            }
        }

        {
            obj_ptr<Buffer_base> buf = Buffer_base::getInstance(element);

            if (buf)
            {
                std::string strBuf;

                buf->toString(strBuf);
                bson_append_binary(bb, name, BSON_BIN_BINARY, strBuf.c_str(),
                                   (int) strBuf.length());

                return;
            }
        }

        {
            obj_ptr<MongoID> oid = (MongoID *) MongoID_base::getInstance(
                                       element);

            if (oid)
            {
                bson_append_oid(bb, name, &oid->m_id);
                return;
            }
        }

        encodeObject(bb, name, element, doJson);
    }
    else
    {
        v8::String::Utf8Value v(element);
        bson_append_string(bb, name, ToCString(v));
    }
}

void encodeValue(bson *bb, const char *name, v8::Local<v8::Value> element)
{
    encodeValue(bb, name, element, true);
}

void encodeArray(bson *bb, const char *name, v8::Local<v8::Value> element)
{
    v8::Local<v8::Array> a = v8::Local<v8::Array>::Cast(element);

    bson_append_start_array(bb, name);

    for (int i = 0, l = a->Length(); i < l; i++)
    {
        v8::Local<v8::Value> val = a->Get(i);
        char numStr[32];

        sprintf(numStr, "%d", i);
        encodeValue(bb, numStr, val);
    }

    bson_append_finish_array(bb);
}

bool encodeObject(bson *bb, const char *name, v8::Local<v8::Value> element,
                  bool doJson)
{
    v8::Local<v8::Object> object = element->ToObject();

    if (doJson)
    {
        v8::Local<v8::Value> jsonFun = object->Get(
                                           v8::String::NewFromUtf8(isolate, "toJSON",
                                                   v8::String::kNormalString, 6));

        if (!IsEmpty(jsonFun) && jsonFun->IsFunction())
        {
            v8::Local<v8::Value> p = v8::String::NewFromUtf8(isolate, name ? name : "");
            v8::Local<v8::Value> element1 = v8::Local<v8::Function>::Cast(
                                                jsonFun)->Call(object, 1, &p);

            if (name)
            {
                encodeValue(bb, name, element1, false);
                return true;
            }

            if (!element1->IsObject())
                return false;

            object = element1->ToObject();
        }
    }

    if (!name
            && (object->IsDate() || object->IsArray() || object->IsRegExp()
                || Buffer_base::getInstance(object)))
        return false;

    if (name)
        bson_append_start_object(bb, name);

    v8::Local<v8::Array> properties = object->GetPropertyNames();

    for (int i = 0; i < (int) properties->Length(); i++)
    {
        v8::Local<v8::Value> prop_name = properties->Get(i);
        v8::Local<v8::Value> prop_val = object->Get(prop_name);

        v8::String::Utf8Value n(prop_name);
        const char *pname = ToCString(n);

        encodeValue(bb, pname, prop_val);
    }

    if (name)
        bson_append_finish_object(bb);

    return true;
}

bool appendObject(bson *bb, v8::Local<v8::Value> element)
{
    return encodeObject(bb, NULL, element, true);
}

result_t encodeObject(bson *bb, v8::Local<v8::Value> element)
{
    bson_init(bb);
    if (!encodeObject(bb, NULL, element, true))
    {
        bson_destroy(bb);
        return CHECK_ERROR(CALL_E_INVALIDARG);
    }
    bson_finish(bb);

    return 0;
}

result_t encoding_base::bsonEncode(v8::Local<v8::Object> data,
                                   obj_ptr<Buffer_base> &retVal)
{
    bson bb;
    result_t hr;

    hr = encodeObject(&bb, data);
    if (hr < 0)
        return hr;

    std::string strBuffer(bson_data(&bb), bson_size(&bb));
    retVal = new Buffer(strBuffer);

    bson_destroy(&bb);

    return 0;
}

v8::Local<v8::Object> decodeObject(bson_iterator *it, bool bArray);

void decodeValue(v8::Local<v8::Object> obj, bson_iterator *it)
{
    bson_type type = bson_iterator_type(it);
    const char *key = bson_iterator_key(it);

    switch (type)
    {
    case BSON_NULL:
        obj->Set(v8::String::NewFromUtf8(isolate, key), v8::Null(isolate));
        break;
    case BSON_STRING:
        obj->Set(v8::String::NewFromUtf8(isolate, key),
                 v8::String::NewFromUtf8(isolate, bson_iterator_string(it)));
        break;
    case BSON_BOOL:
        obj->Set(v8::String::NewFromUtf8(isolate, key),
                 bson_iterator_bool(it) ? v8::True(isolate) : v8::False(isolate));
        break;
    case BSON_INT:
        obj->Set(v8::String::NewFromUtf8(isolate, key), v8::Number::New(isolate, bson_iterator_int(it)));
        break;
    case BSON_LONG:
    {
        int64_t num = bson_iterator_long(it);
        if (num >= -2147483648ll && num <= 2147483647ll)
        {
            obj->Set(v8::String::NewFromUtf8(isolate, key),
                     v8::Number::New(isolate, (double) num));
        }
        else
        {
            obj_ptr<Int64> int64 = new Int64(num);
            obj->Set(v8::String::NewFromUtf8(isolate, key), int64->wrap());
        }
        break;
    }
    case BSON_DOUBLE:
        obj->Set(v8::String::NewFromUtf8(isolate, key),
                 v8::Number::New(isolate, bson_iterator_double(it)));
        break;
    case BSON_DATE:
        obj->Set(v8::String::NewFromUtf8(isolate, key),
                 v8::Date::New(isolate, (double) bson_iterator_date(it)));
        break;
    case BSON_BINDATA:
    {
        obj_ptr<Buffer_base> buf = new Buffer(
            std::string(bson_iterator_bin_data(it),
                        bson_iterator_bin_len(it)));

        obj->Set(v8::String::NewFromUtf8(isolate, key), buf->wrap());
        break;
    }
    case BSON_OID:
    {
        obj_ptr<MongoID> oid = new MongoID(bson_iterator_oid(it));
        obj->Set(v8::String::NewFromUtf8(isolate, key), oid->wrap());
        break;
    }
    case BSON_REGEX:
    {
        v8::RegExp::Flags flgs = v8::RegExp::kNone;
        const char *opts = bson_iterator_regex_opts(it);
        char ch;

        while ((ch = *opts++) != 0)
            if (ch == 'm')
                flgs = (v8::RegExp::Flags) (flgs | v8::RegExp::kMultiline);
            else if (ch == 'g')
                flgs = (v8::RegExp::Flags) (flgs | v8::RegExp::kGlobal);
            else if (ch == 'i')
                flgs = (v8::RegExp::Flags) (flgs | v8::RegExp::kIgnoreCase);

        obj->Set(v8::String::NewFromUtf8(isolate, key),
                 v8::RegExp::New(v8::String::NewFromUtf8(isolate, bson_iterator_regex(it)),
                                 flgs));
        break;
    }
    case BSON_OBJECT:
    case BSON_ARRAY:
    {
        bson_iterator it1;

        bson_iterator_subiterator(it, &it1);
        obj->Set(v8::String::NewFromUtf8(isolate, key), decodeObject(&it1, type == BSON_ARRAY));
        break;
    }
    default:
        printf("unknown type: %d\n", type);
        break;
    }
}

v8::Local<v8::Object> decodeObject(bson_iterator *it, bool bArray)
{
    v8::Local<v8::Object> obj;

    if (bArray)
        obj = v8::Array::New(isolate);
    else
        obj = v8::Object::New(isolate);

    while (bson_iterator_next(it))
        decodeValue(obj, it);

    return obj;
}

v8::Local<v8::Object> decodeObject(const bson *bb)
{
    bson_iterator it;
    bson_iterator_from_buffer(&it, bson_data(bb));

    return decodeObject(&it, false);
}

v8::Local<v8::Object> decodeObject(const char *buffer)
{
    bson_iterator it;
    bson_iterator_from_buffer(&it, buffer);

    return decodeObject(&it, false);
}

result_t encoding_base::bsonDecode(Buffer_base *data,
                                   v8::Local<v8::Object> &retVal)
{
    std::string strBuf;

    data->toString(strBuf);
    retVal = decodeObject(strBuf.c_str());

    return 0;
}

}
