/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _encoding_base_H_
#define _encoding_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "module.h"

namespace fibjs
{

class module_base;
class Buffer_base;

class encoding_base : public module_base
{
public:
	// encoding_base
	static result_t base32Encode(Buffer_base* data, std::string& retVal);
	static result_t base32Decode(const char* data, obj_ptr<Buffer_base>& retVal);
	static result_t base64Encode(Buffer_base* data, std::string& retVal);
	static result_t base64Decode(const char* data, obj_ptr<Buffer_base>& retVal);
	static result_t hexEncode(Buffer_base* data, std::string& retVal);
	static result_t hexDecode(const char* data, obj_ptr<Buffer_base>& retVal);
	static result_t jsstr(const char* str, std::string& retVal);
	static result_t encodeURI(const char* url, std::string& retVal);
	static result_t encodeURIComponent(const char* url, std::string& retVal);
	static result_t decodeURI(const char* url, std::string& retVal);
	static result_t jsonEncode(v8::Handle<v8::Value> data, std::string& retVal);
	static result_t jsonDecode(const char* data, v8::Handle<v8::Value>& retVal);
	static result_t bsonEncode(v8::Handle<v8::Object> data, obj_ptr<Buffer_base>& retVal);
	static result_t bsonDecode(Buffer_base* data, v8::Handle<v8::Object>& retVal);

	DECLARE_CLASSINFO(encoding_base);

public:
	static void s_base32Encode(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_base32Decode(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_base64Encode(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_base64Decode(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_hexEncode(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_hexDecode(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_jsstr(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_encodeURI(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_encodeURIComponent(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_decodeURI(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_jsonEncode(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_jsonDecode(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_bsonEncode(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_bsonDecode(const v8::FunctionCallbackInfo<v8::Value>& args);
};

}

#include "Buffer.h"

namespace fibjs
{
	inline ClassInfo& encoding_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"base32Encode", s_base32Encode, true},
			{"base32Decode", s_base32Decode, true},
			{"base64Encode", s_base64Encode, true},
			{"base64Decode", s_base64Decode, true},
			{"hexEncode", s_hexEncode, true},
			{"hexDecode", s_hexDecode, true},
			{"jsstr", s_jsstr, true},
			{"encodeURI", s_encodeURI, true},
			{"encodeURIComponent", s_encodeURIComponent, true},
			{"decodeURI", s_decodeURI, true},
			{"jsonEncode", s_jsonEncode, true},
			{"jsonDecode", s_jsonDecode, true},
			{"bsonEncode", s_bsonEncode, true},
			{"bsonDecode", s_bsonDecode, true}
		};

		static ClassData s_cd = 
		{ 
			"encoding", NULL, 
			14, s_method, 0, NULL, 0, NULL, NULL, NULL,
			&module_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}


	inline void encoding_base::s_base32Encode(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		std::string vr;

		METHOD_ENTER(1, 1);

		ARG(obj_ptr<Buffer_base>, 0);

		hr = base32Encode(v0, vr);

		METHOD_RETURN();
	}

	inline void encoding_base::s_base32Decode(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<Buffer_base> vr;

		METHOD_ENTER(1, 1);

		ARG_String(0);

		hr = base32Decode(v0, vr);

		METHOD_RETURN();
	}

	inline void encoding_base::s_base64Encode(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		std::string vr;

		METHOD_ENTER(1, 1);

		ARG(obj_ptr<Buffer_base>, 0);

		hr = base64Encode(v0, vr);

		METHOD_RETURN();
	}

	inline void encoding_base::s_base64Decode(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<Buffer_base> vr;

		METHOD_ENTER(1, 1);

		ARG_String(0);

		hr = base64Decode(v0, vr);

		METHOD_RETURN();
	}

	inline void encoding_base::s_hexEncode(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		std::string vr;

		METHOD_ENTER(1, 1);

		ARG(obj_ptr<Buffer_base>, 0);

		hr = hexEncode(v0, vr);

		METHOD_RETURN();
	}

	inline void encoding_base::s_hexDecode(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<Buffer_base> vr;

		METHOD_ENTER(1, 1);

		ARG_String(0);

		hr = hexDecode(v0, vr);

		METHOD_RETURN();
	}

	inline void encoding_base::s_jsstr(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		std::string vr;

		METHOD_ENTER(1, 1);

		ARG_String(0);

		hr = jsstr(v0, vr);

		METHOD_RETURN();
	}

	inline void encoding_base::s_encodeURI(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		std::string vr;

		METHOD_ENTER(1, 1);

		ARG_String(0);

		hr = encodeURI(v0, vr);

		METHOD_RETURN();
	}

	inline void encoding_base::s_encodeURIComponent(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		std::string vr;

		METHOD_ENTER(1, 1);

		ARG_String(0);

		hr = encodeURIComponent(v0, vr);

		METHOD_RETURN();
	}

	inline void encoding_base::s_decodeURI(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		std::string vr;

		METHOD_ENTER(1, 1);

		ARG_String(0);

		hr = decodeURI(v0, vr);

		METHOD_RETURN();
	}

	inline void encoding_base::s_jsonEncode(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		std::string vr;

		METHOD_ENTER(1, 1);

		ARG(v8::Handle<v8::Value>, 0);

		hr = jsonEncode(v0, vr);

		METHOD_RETURN();
	}

	inline void encoding_base::s_jsonDecode(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		v8::Handle<v8::Value> vr;

		METHOD_ENTER(1, 1);

		ARG_String(0);

		hr = jsonDecode(v0, vr);

		METHOD_RETURN();
	}

	inline void encoding_base::s_bsonEncode(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<Buffer_base> vr;

		METHOD_ENTER(1, 1);

		ARG(v8::Handle<v8::Object>, 0);

		hr = bsonEncode(v0, vr);

		METHOD_RETURN();
	}

	inline void encoding_base::s_bsonDecode(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		v8::Handle<v8::Object> vr;

		METHOD_ENTER(1, 1);

		ARG(obj_ptr<Buffer_base>, 0);

		hr = bsonDecode(v0, vr);

		METHOD_RETURN();
	}

}

#endif

