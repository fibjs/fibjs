/*
 * bson_encoding.cpp
 *
 *  Created on: Oct 19, 2012
 *      Author: lion
 */

#include "encoding_bson.h"
#include "ifs/encoding.h"
#include "Buffer.h"

namespace fibjs
{

inline const char *
ToCString(const v8::String::Utf8Value& value)
{
	return *value ? *value : "<string conversion failed>";
}

void encodeArray(bson *bb, const char *name,
		const v8::Handle<v8::Value> element);
void encodeObject(bson *bb, const char *name,
		const v8::Handle<v8::Value> element, bool doJson);

void encodeValue(bson *bb, const char *name,
		const v8::Handle<v8::Value> element, bool doJson)
{
	if (element.IsEmpty() || element->IsUndefined() || element->IsFunction())
		;
	else if (element->IsNull())
		bson_append_null(bb, name);
	else if (element->IsDate())
		bson_append_date(bb, name, (bson_date_t) element->NumberValue());
	else if (element->IsBoolean())
		bson_append_bool(bb, name, element->IsTrue());
	else if (element->IsNumber())
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
	else if (element->IsObject())
	{
		obj_ptr<Buffer_base> buf = Buffer_base::getInstance(element);

		if (buf)
		{
			std::string strBuf;

			buf->toString(strBuf);
			bson_append_binary(bb, name, BSON_BIN_BINARY, strBuf.c_str(),
					(int) strBuf.length());
		}
		else
			encodeObject(bb, name, element, doJson);
	}
	else
	{
		v8::String::Utf8Value v(element);
		bson_append_string(bb, name, ToCString(v));
	}
}

void encodeArray(bson *bb, const char *name,
		const v8::Handle<v8::Value> element)
{
	v8::Handle<v8::Array> a = v8::Handle<v8::Array>::Cast(element);

	bson_append_start_array(bb, name);

	for (int i = 0, l = a->Length(); i < l; i++)
	{
		v8::Handle<v8::Value> val = a->Get(v8::Number::New(i));
		char numStr[32];

		sprintf(numStr, "%d", i);
		encodeValue(bb, numStr, val, true);
	}

	bson_append_finish_array(bb);
}

void encodeObject(bson *bb, const char *name,
		const v8::Handle<v8::Value> element, bool doJson)
{
	v8::Handle<v8::Object> object = element->ToObject();

	if (doJson)
	{
		v8::Handle<v8::Value> jsonFun = object->Get(
				v8::String::New("toJSON", 6));

		if (!IsEmpty(jsonFun) && jsonFun->IsFunction())
		{
			v8::Handle<v8::Value> p = v8::String::New(name);
			v8::Handle<v8::Value> element1 = v8::Handle<v8::Function>::Cast(
					jsonFun)->Call(object, 1, &p);

			if (name)
			{
				encodeValue(bb, name, element1, false);
				return;
			}
			else if (!element1->IsObject())
				return;

			object = element1->ToObject();
		}
	}

	if (name)
		bson_append_start_object(bb, name);

	v8::Handle<v8::Array> properties = object->GetPropertyNames();

	for (int i = 0; i < (int) properties->Length(); i++)
	{
		v8::Handle<v8::Value> prop_name = properties->Get(v8::Integer::New(i));
		v8::Handle<v8::Value> prop_val = object->Get(prop_name->ToString());

		v8::String::Utf8Value n(prop_name);
		const char *pname = ToCString(n);

		encodeValue(bb, pname, prop_val, true);
	}

	if (name)
		bson_append_finish_object(bb);
}

void encodeObject(bson *bb, const v8::Handle<v8::Value> element)
{
	encodeObject(bb, NULL, element, true);
}

result_t encoding_base::bsonEncode(v8::Handle<v8::Object> data,
		obj_ptr<Buffer_base>& retVal)
{
	bson bb;

	bson_init(&bb);
	encodeObject(&bb, data);
	bson_finish(&bb);

	std::string strBuffer(bson_data(&bb), bson_size(&bb));
	retVal = new Buffer(strBuffer);

	bson_destroy(&bb);

	return 0;
}

v8::Handle<v8::Object> decodeObject(bson_iterator *it, bool bArray);

void decodeValue(v8::Handle<v8::Object> obj, bson_iterator *it)
{
	bson_type type = bson_iterator_type(it);
	const char *key = bson_iterator_key(it);

	switch (type)
	{
	case BSON_NULL:
		obj->Set(v8::String::New(key), v8::Null());
		break;
	case BSON_STRING:
		obj->Set(v8::String::New(key),
				v8::String::New(bson_iterator_string(it)));
		break;
	case BSON_BOOL:
		obj->Set(v8::String::New(key),
				bson_iterator_bool(it) ? v8::True() : v8::False());
		break;
	case BSON_INT:
		obj->Set(v8::String::New(key), v8::Number::New(bson_iterator_int(it)));
		break;
	case BSON_LONG:
		obj->Set(v8::String::New(key),
				v8::Number::New((double) bson_iterator_long(it)));
		break;
	case BSON_DOUBLE:
		obj->Set(v8::String::New(key),
				v8::Number::New(bson_iterator_double(it)));
		break;
	case BSON_DATE:
		obj->Set(v8::String::New(key),
				v8::Date::New((double) bson_iterator_date(it)));
		break;
	case BSON_BINDATA:
	{
		obj_ptr<Buffer_base> buf = new Buffer(
				std::string(bson_iterator_bin_data(it),
						bson_iterator_bin_len(it)));

		obj->Set(v8::String::New(key), buf->wrap());
		break;
	}
	case BSON_OBJECT:
	case BSON_ARRAY:
	{
		bson_iterator it1;

		bson_iterator_subiterator(it, &it1);
		obj->Set(v8::String::New(key), decodeObject(&it1, type == BSON_ARRAY));
		break;
	}
	default:
		break;
	}
}

v8::Handle<v8::Object> decodeObject(bson_iterator *it, bool bArray)
{
	v8::Handle<v8::Object> obj;

	if (bArray)
		obj = v8::Array::New();
	else
		obj = v8::Object::New();

	while (bson_iterator_next(it))
		decodeValue(obj, it);

	return obj;
}

result_t encoding_base::bsonDecode(Buffer_base* data,
		v8::Handle<v8::Object>& retVal)
{
	std::string strBuf;

	data->toString(strBuf);

	bson_iterator it;
	bson_iterator_from_buffer(&it, strBuf.c_str());

	retVal = decodeObject(&it, false);
	return 0;
}

}
