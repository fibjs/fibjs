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
	static result_t base32Encode(obj_ptr<Buffer_base> data, std::string& retVal);
	static result_t base32Decode(const char* data, obj_ptr<Buffer_base>& retVal);
	static result_t base64Encode(obj_ptr<Buffer_base> data, std::string& retVal);
	static result_t base64Decode(const char* data, obj_ptr<Buffer_base>& retVal);
	static result_t hexEncode(obj_ptr<Buffer_base> data, std::string& retVal);
	static result_t hexDecode(const char* data, obj_ptr<Buffer_base>& retVal);

public:
	static ClassInfo& class_info()
	{
		static ClassMethod s_method[] = 
		{
			{"base32Encode", s_base32Encode},
			{"base32Decode", s_base32Decode},
			{"base64Encode", s_base64Encode},
			{"base64Decode", s_base64Decode},
			{"hexEncode", s_hexEncode},
			{"hexDecode", s_hexDecode}
		};

		static ClassData s_cd = 
		{ 
			"encoding", NULL, 
			6, s_method, 0, NULL, NULL,
			&module_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}

	virtual ClassInfo& Classinfo()
	{
		return class_info();
	}

private:
	static v8::Handle<v8::Value> s_base32Encode(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_base32Decode(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_base64Encode(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_base64Decode(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_hexEncode(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_hexDecode(const v8::Arguments& args);
};

}

#include "Buffer.h"

namespace fibjs
{

	inline v8::Handle<v8::Value> encoding_base::s_base32Encode(const v8::Arguments& args)
	{
		std::string vr;

		METHOD_ENTER(1, 1);

		ARG_CLASS(Buffer_base, 0);

		hr = base32Encode(v0, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> encoding_base::s_base32Decode(const v8::Arguments& args)
	{
		obj_ptr<Buffer_base> vr;

		METHOD_ENTER(1, 1);

		ARG_String(0);

		hr = base32Decode(v0, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> encoding_base::s_base64Encode(const v8::Arguments& args)
	{
		std::string vr;

		METHOD_ENTER(1, 1);

		ARG_CLASS(Buffer_base, 0);

		hr = base64Encode(v0, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> encoding_base::s_base64Decode(const v8::Arguments& args)
	{
		obj_ptr<Buffer_base> vr;

		METHOD_ENTER(1, 1);

		ARG_String(0);

		hr = base64Decode(v0, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> encoding_base::s_hexEncode(const v8::Arguments& args)
	{
		std::string vr;

		METHOD_ENTER(1, 1);

		ARG_CLASS(Buffer_base, 0);

		hr = hexEncode(v0, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> encoding_base::s_hexDecode(const v8::Arguments& args)
	{
		obj_ptr<Buffer_base> vr;

		METHOD_ENTER(1, 1);

		ARG_String(0);

		hr = hexDecode(v0, vr);

		METHOD_RETURN();
	}

}

#endif

