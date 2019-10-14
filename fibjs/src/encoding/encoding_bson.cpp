/*
 * bson_encoding.cpp
 *
 *  Created on: Oct 19, 2012
 *      Author: lion
 */

#include "object.h"
#include "encoding_bson.h"
#include "ifs/encoding.h"
#include "Buffer.h"
#include "utils.h"
#include "MongoID.h"

namespace fibjs {

DECLARE_MODULE(bson);

void encodeArray(Isolate* isolate, bson* bb, const char* name, v8::Local<v8::Value> element);
bool encodeObject(Isolate* isolate, bson* bb, const char* name, v8::Local<v8::Value> element,
    bool doJson);

void encodeValue(Isolate* isolate, bson* bb, const char* name, v8::Local<v8::Value> element,
    bool doJson)
{
    if (element.IsEmpty() || element->IsUndefined() || element->IsFunction())
        ;
    else if (element->IsNull())
        bson_append_null(bb, name);
    else if (element->IsDate())
        bson_append_date(bb, name, (bson_date_t)element->NumberValue());
    else if (element->IsBoolean())
        bson_append_bool(bb, name, element->IsTrue());
    else if (element->IsNumber() || element->IsNumberObject()) {
        double value = element->NumberValue();
        int64_t num = (int64_t)value;

        if (value == (double)num) {
            if (num >= -2147483648ll && num <= 2147483647ll)
                bson_append_int(bb, name, (int32_t)num);
            else
                bson_append_long(bb, name, num);
        } else
            bson_append_double(bb, name, value);
    } else if (element->IsArray())
        encodeArray(isolate, bb, name, element);
    else if (element->IsRegExp()) {
        v8::Local<v8::RegExp> re = v8::Local<v8::RegExp>::Cast(element);
        v8::Local<v8::String> src = re->GetSource();
        v8::RegExp::Flags flgs = re->GetFlags();
        char flgStr[4];
        char* p = flgStr;

        if (flgs & v8::RegExp::kIgnoreCase)
            *p++ = 'i';
        if (flgs & v8::RegExp::kGlobal)
            *p++ = 'g';
        if (flgs & v8::RegExp::kMultiline)
            *p++ = 'm';

        *p = 0;

        bson_append_regex(bb, name, ToCString(v8::String::Utf8Value(src)), flgStr);
    } else if (element->IsObject()) {
        {
            obj_ptr<Buffer_base> buf = Buffer_base::getInstance(element);

            if (buf) {
                exlib::string strBuf;

                buf->toString(strBuf);
                bson_append_binary(bb, name, BSON_BIN_BINARY, strBuf.c_str(),
                    (int32_t)strBuf.length());

                return;
            }
        }

        {
            obj_ptr<MongoID> oid = (MongoID*)MongoID_base::getInstance(
                element);

            if (oid) {
                bson_append_oid(bb, name, &oid->m_id);
                return;
            }
        }

        encodeObject(isolate, bb, name, element, doJson);
    } else {
        v8::String::Utf8Value v(element);
        bson_append_string(bb, name, ToCString(v));
    }
}

void encodeValue(Isolate* isolate, bson* bb, const char* name, v8::Local<v8::Value> element)
{
    encodeValue(isolate, bb, name, element, true);
}

void encodeArray(Isolate* isolate, bson* bb, const char* name, v8::Local<v8::Value> element)
{
    v8::Local<v8::Array> a = v8::Local<v8::Array>::Cast(element);

    bson_append_start_array(bb, name);

    for (int32_t i = 0, l = a->Length(); i < l; i++) {
        JSValue val = a->Get(i);
        char numStr[32];

        sprintf(numStr, "%d", i);
        encodeValue(isolate, bb, numStr, val);
    }

    bson_append_finish_array(bb);
}

bool encodeObject(Isolate* isolate, bson* bb, const char* name, v8::Local<v8::Value> element,
    bool doJson)
{
    v8::Local<v8::Object> object = element->ToObject();

    if (doJson) {
        JSValue jsonFun = object->Get(isolate->NewString("toJSON", 6));

        if (!IsEmpty(jsonFun) && jsonFun->IsFunction()) {
            JSValue p = isolate->NewString(name ? name : "");
            JSValue element1 = v8::Local<v8::Function>::Cast(jsonFun)->Call(object, 1, &p);

            if (name) {
                encodeValue(isolate, bb, name, element1, false);
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

    JSArray properties = object->GetPropertyNames();

    for (int32_t i = 0; i < (int32_t)properties->Length(); i++) {
        JSValue prop_name = properties->Get(i);
        JSValue prop_val = object->Get(prop_name);

        v8::String::Utf8Value n(prop_name);
        const char* pname = ToCString(n);

        encodeValue(isolate, bb, pname, prop_val);
    }

    if (name)
        bson_append_finish_object(bb);

    return true;
}

bool appendObject(Isolate* isolate, bson* bb, v8::Local<v8::Value> element)
{
    return encodeObject(isolate, bb, NULL, element, true);
}

result_t encodeObject(Isolate* isolate, bson* bb, v8::Local<v8::Value> element)
{
    bson_init(bb);
    if (!encodeObject(isolate, bb, NULL, element, true)) {
        bson_destroy(bb);
        return CHECK_ERROR(CALL_E_INVALIDARG);
    }
    bson_finish(bb);

    return 0;
}

result_t bson_base::encode(v8::Local<v8::Object> data,
    obj_ptr<Buffer_base>& retVal)
{
    Isolate* isolate = Isolate::current();
    bson bb;
    result_t hr;

    hr = encodeObject(isolate, &bb, data);
    if (hr < 0)
        return hr;

    exlib::string strBuffer(bson_data(&bb), bson_size(&bb));
    retVal = new Buffer(strBuffer);

    bson_destroy(&bb);

    return 0;
}

v8::Local<v8::Object> decodeObject(Isolate* isolate, bson_iterator* it, bool bArray);

void decodeValue(Isolate* isolate, v8::Local<v8::Object> obj, bson_iterator* it)
{
    bson_type type = bson_iterator_type(it);
    const char* key = bson_iterator_key(it);

    switch (type) {
    case BSON_NULL:
        obj->Set(isolate->NewString(key), v8::Null(isolate->m_isolate));
        break;
    case BSON_STRING:
        obj->Set(isolate->NewString(key),
            isolate->NewString(bson_iterator_string(it)));
        break;
    case BSON_BOOL:
        obj->Set(isolate->NewString(key),
            bson_iterator_bool(it) ? v8::True(isolate->m_isolate) : v8::False(isolate->m_isolate));
        break;
    case BSON_INT:
        obj->Set(isolate->NewString(key), v8::Number::New(isolate->m_isolate, bson_iterator_int(it)));
        break;
    case BSON_LONG: {
        obj->Set(isolate->NewString(key),
            v8::Number::New(isolate->m_isolate, (double)bson_iterator_long(it)));
        break;
    }
    case BSON_DOUBLE:
        obj->Set(isolate->NewString(key),
            v8::Number::New(isolate->m_isolate, bson_iterator_double(it)));
        break;
    case BSON_DATE:
        obj->Set(isolate->NewString(key),
            v8::Date::New(isolate->m_isolate, (double)bson_iterator_date(it)));
        break;
    case BSON_BINDATA: {
        obj_ptr<Buffer_base> buf = new Buffer(
            bson_iterator_bin_data(it), bson_iterator_bin_len(it));

        obj->Set(isolate->NewString(key), buf->wrap());
        break;
    }
    case BSON_OID: {
        obj_ptr<MongoID> oid = new MongoID(bson_iterator_oid(it));
        obj->Set(isolate->NewString(key), oid->wrap());
        break;
    }
    case BSON_REGEX: {
        v8::RegExp::Flags flgs = v8::RegExp::kNone;
        const char* opts = bson_iterator_regex_opts(it);
        char ch;

        while ((ch = *opts++) != 0)
            if (ch == 'm')
                flgs = (v8::RegExp::Flags)(flgs | v8::RegExp::kMultiline);
            else if (ch == 'g')
                flgs = (v8::RegExp::Flags)(flgs | v8::RegExp::kGlobal);
            else if (ch == 'i')
                flgs = (v8::RegExp::Flags)(flgs | v8::RegExp::kIgnoreCase);

        obj->Set(isolate->NewString(key),
            v8::RegExp::New(isolate->NewString(bson_iterator_regex(it)),
                flgs));
        break;
    }
    case BSON_OBJECT:
    case BSON_ARRAY: {
        bson_iterator it1;

        bson_iterator_subiterator(it, &it1);
        obj->Set(isolate->NewString(key), decodeObject(isolate, &it1, type == BSON_ARRAY));
        break;
    }
    default:
        printf("unknown type: %d\n", type);
        break;
    }
}

v8::Local<v8::Object> decodeObject(Isolate* isolate, bson_iterator* it, bool bArray)
{
    v8::Local<v8::Object> obj;

    if (bArray)
        obj = v8::Array::New(isolate->m_isolate);
    else
        obj = v8::Object::New(isolate->m_isolate);

    while (bson_iterator_next(it))
        decodeValue(isolate, obj, it);

    return obj;
}

v8::Local<v8::Object> decodeObject(Isolate* isolate, const bson* bb)
{
    bson_iterator it;
    bson_iterator_from_buffer(&it, bson_data(bb));

    return decodeObject(isolate, &it, false);
}

v8::Local<v8::Object> decodeObject(Isolate* isolate, const char* buffer)
{
    bson_iterator it;
    bson_iterator_from_buffer(&it, buffer);

    return decodeObject(isolate, &it, false);
}

result_t bson_base::decode(Buffer_base* data,
    v8::Local<v8::Object>& retVal)
{
    Isolate* isolate = Isolate::current();
    exlib::string strBuf;

    data->toString(strBuf);
    retVal = decodeObject(isolate, strBuf.c_str());

    return 0;
}
}
