/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _SslSocket_base_H_
#define _SslSocket_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "Stream.h"

namespace fibjs
{

class Stream_base;

class SslSocket_base : public Stream_base
{
public:
	// SslSocket_base
	static result_t _new(obj_ptr<SslSocket_base>& retVal);
	virtual result_t connect(Stream_base* s, exlib::AsyncEvent* ac) = 0;
	virtual result_t accept(Stream_base* s, exlib::AsyncEvent* ac) = 0;

	DECLARE_CLASSINFO(SslSocket_base);

public:
	static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_connect(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_accept(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
	ASYNC_MEMBER1(SslSocket_base, connect, Stream_base*);
	ASYNC_MEMBER1(SslSocket_base, accept, Stream_base*);
};

}


namespace fibjs
{
	inline ClassInfo& SslSocket_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"connect", s_connect},
			{"accept", s_accept}
		};

		static ClassData s_cd = 
		{ 
			"SslSocket", s__new, 
			2, s_method, 0, NULL, 0, NULL, NULL, NULL,
			&Stream_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}


	inline void SslSocket_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<SslSocket_base> vr;

		CONSTRUCT_ENTER(0, 0);

		hr = _new(vr);

		CONSTRUCT_RETURN();
	}

	inline void SslSocket_base::s_connect(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(SslSocket_base);
		METHOD_ENTER(1, 1);

		ARG(obj_ptr<Stream_base>, 0);

		hr = pInst->ac_connect(v0);

		METHOD_VOID();
	}

	inline void SslSocket_base::s_accept(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(SslSocket_base);
		METHOD_ENTER(1, 1);

		ARG(obj_ptr<Stream_base>, 0);

		hr = pInst->ac_accept(v0);

		METHOD_VOID();
	}

}

#endif

