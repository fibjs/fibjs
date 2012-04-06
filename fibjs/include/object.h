/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _fj_object_H_
#define _fj_object_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "utils.h"
#include "ClassInfo.h"
#include "acPool.h"

namespace fibjs
{

extern v8::Isolate* isolate;

class object_base
{
public:
	object_base()
	{
		refs_ = 0;
	}

	virtual ~object_base()
	{
	}

	void Ref()
	{
		refs_++;
	}

	void Unref()
	{
		if (--refs_ == 0)
			delete this;
	}

protected:
	int refs_;
	exlib::Locker m_lock;

private:
	static void WeakCallback(v8::Persistent<v8::Value> value, void* data)
	{
		(static_cast<object_base*>(data))->dispose();
	}

protected:
	v8::Persistent<v8::Object> handle_;

	v8::Handle<v8::Value> wrap(ClassInfo& i)
	{
		if (handle_.IsEmpty())
			return wrap(i.CreateInstance());

		return handle_;
	}

	v8::Handle<v8::Value> wrap(v8::Handle<v8::Object> o)
	{
		if (handle_.IsEmpty())
		{
			handle_ = v8::Persistent<v8::Object>::New(o);
			handle_->SetPointerInInternalField(0, this);
			handle_.MakeWeak(this, WeakCallback);

			Ref();
		}

		return handle_;
	}

public:
	// object_base
	result_t dispose()
	{
		if (!handle_.IsEmpty())
		{
			handle_.ClearWeak();
			handle_->SetPointerInInternalField(0, 0);
			handle_.Dispose();
			handle_.Clear();

			Unref();
		}

		return 0;
	}

	virtual result_t toString(std::string& retVal)
	{
		retVal = Classinfo().name();
		return 0;
	}

public:
	static ClassInfo& class_info()
	{
		static ClassMethod s_method[] =
		{
		{ "dispose", m_dispose },
		{ "toString", m_toString } };

		static ClassData s_cd =
		{ "object", NULL, 2, s_method, 0, NULL, NULL };

		static ClassInfo s_ci(s_cd);

		return s_ci;
	}

	virtual ClassInfo& Classinfo()
	{
		return class_info();
	}

	v8::Handle<v8::Value> JSObject()
	{
		return wrap(Classinfo());
	}

private:
	static v8::Handle<v8::Value> m_dispose(const v8::Arguments& args)
	{
		METHOD_ENTER(0, 0);
		METHOD_INSTANCE(object_base);

		hr = pInst->dispose();

		METHOD_VOID();
	}

	static v8::Handle<v8::Value> m_toString(const v8::Arguments& args)
	{
		METHOD_ENTER(0, 0);
		METHOD_INSTANCE(object_base);

		std::string vr;
		hr = pInst->toString(vr);

		METHOD_RETURN();
	}
};

#include "object_async.inl"

}

#endif

