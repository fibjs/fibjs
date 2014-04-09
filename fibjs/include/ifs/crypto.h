/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _crypto_base_H_
#define _crypto_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "module.h"

namespace fibjs
{

class module_base;
class Cipher_base;
class Buffer_base;

class crypto_base : public module_base
{
public:
	// crypto_base
	static result_t randomBytes(int32_t size, obj_ptr<Buffer_base>& retVal, exlib::AsyncEvent* ac);
	static result_t pseudoRandomBytes(int32_t size, obj_ptr<Buffer_base>& retVal, exlib::AsyncEvent* ac);

	DECLARE_CLASSINFO(crypto_base);

public:
	static void s_randomBytes(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_pseudoRandomBytes(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
	ASYNC_STATICVALUE2(crypto_base, randomBytes, int32_t, obj_ptr<Buffer_base>);
	ASYNC_STATICVALUE2(crypto_base, pseudoRandomBytes, int32_t, obj_ptr<Buffer_base>);
};

}

#include "Cipher.h"
#include "Buffer.h"

namespace fibjs
{
	inline ClassInfo& crypto_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"randomBytes", s_randomBytes, true},
			{"pseudoRandomBytes", s_pseudoRandomBytes, true}
		};

		static ClassData::ClassObject s_object[] = 
		{
			{"Cipher", Cipher_base::class_info}
		};

		static ClassData s_cd = 
		{ 
			"crypto", NULL, 
			2, s_method, 1, s_object, 0, NULL, NULL, NULL,
			&module_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}


	inline void crypto_base::s_randomBytes(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<Buffer_base> vr;

		METHOD_ENTER(1, 1);

		ARG(int32_t, 0);

		hr = ac_randomBytes(v0, vr);

		METHOD_RETURN();
	}

	inline void crypto_base::s_pseudoRandomBytes(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<Buffer_base> vr;

		METHOD_ENTER(1, 1);

		ARG(int32_t, 0);

		hr = ac_pseudoRandomBytes(v0, vr);

		METHOD_RETURN();
	}

}

#endif

