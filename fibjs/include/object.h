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

#include "object_async.inl"

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

public:
	void Ref()
	{
		refs_++;
	}

	void Unref()
	{
		if (--refs_ == 0)
			delete this;
	}

public:
	virtual void enter()
	{
		if(!m_lock.trylock())
		{
			v8::Unlocker unlocker(isolate);
			m_lock.lock();
		}
	}

	virtual void leave()
	{
		m_lock.unlock();
	}

	exlib::Locker m_lock;

private:
	int refs_;
	v8::Persistent<v8::Object> handle_;

private:
	static void WeakCallback(v8::Persistent<v8::Value> value, void* data)
	{
		(static_cast<object_base*> (data))->dispose();
	}

public:
	v8::Handle<v8::Object> wrap(v8::Handle<v8::Object> o)
	{
		if (handle_.IsEmpty())
		{
			handle_ = v8::Persistent<v8::Object>::New(o);
			handle_->SetPointerInInternalField(0, this);
			handle_.MakeWeak(this, WeakCallback);

			v8::V8::AdjustAmountOfExternalAllocatedMemory(32);

			Ref();
		}

		return handle_;
	}

	v8::Handle<v8::Object> wrap()
	{
		if (handle_.IsEmpty())
			return wrap(Classinfo().CreateInstance());

		return handle_;
	}

public:
	class scope
	{
	public:
		scope(object_base* pInst) :
			m_pInst(pInst)
		{
			m_pInst->enter();
		}

		~scope()
		{
			m_pInst->leave();
		}

	private:
		object_base* m_pInst;
	};

public:
	// Event
	result_t on(const char* ev, v8::Handle<v8::Function> func);
	result_t once(const char* ev, v8::Handle<v8::Function> func);
	result_t off(const char* ev, v8::Handle<v8::Function> func);
	result_t trigger(const char* ev, const v8::Arguments& args);

private:
	v8::Handle<v8::Array> GetHiddenArray(const char* k, bool create = false, bool autoDelete = false);

public:
	// object_base
	virtual result_t dispose()
	{
		if (!handle_.IsEmpty())
		{
			handle_.ClearWeak();
			handle_->SetPointerInInternalField(0, 0);
			handle_.Dispose();
			handle_.Clear();

			v8::V8::AdjustAmountOfExternalAllocatedMemory(-32);

			Unref();
		}

		return 0;
	}

	virtual result_t toString(std::string& retVal)
	{
		retVal = Classinfo().name();
		return 0;
	}

	virtual result_t toJSON(const char* key, v8::Handle<v8::Object>& retVal)
	{
		retVal = v8::Object::New();
		return 0;
	}

	virtual result_t ValueOf(v8::Handle<v8::Object>& retVal)
	{
		retVal = wrap();
		return 0;
	}

	//------------------------------------------------------------------
public:
	static ClassInfo& class_info();

	virtual ClassInfo& Classinfo()
	{
		return class_info();
	}

private:
	static v8::Handle<v8::Value> s_dispose(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_toString(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_toJSON(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_ValueOf(const v8::Arguments& args);
};

}

namespace fibjs
{
	inline ClassInfo& object_base::class_info()
	{
		static ClassMethod s_method[] =
		{
			{"dispose", s_dispose},
			{"toString", s_toString},
			{"toJSON", s_toJSON},
			{"ValueOf", s_ValueOf}
		};

		static ClassData s_cd =
		{
			"object", NULL,
			4, s_method, 0, NULL, 0, NULL, NULL
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}


	inline v8::Handle<v8::Value> object_base::s_dispose(const v8::Arguments& args)
	{
		METHOD_INSTANCE(object_base);
		METHOD_ENTER(0, 0);

		hr = pInst->dispose();

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> object_base::s_toString(const v8::Arguments& args)
	{
		std::string vr;

		METHOD_INSTANCE(object_base);
		METHOD_ENTER(0, 0);

		hr = pInst->toString(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> object_base::s_toJSON(const v8::Arguments& args)
	{
		v8::Handle<v8::Object> vr;

		METHOD_INSTANCE(object_base);
		METHOD_ENTER(1, 0);

		OPT_ARG_String(0, "");

		hr = pInst->toJSON(v0, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> object_base::s_ValueOf(const v8::Arguments& args)
	{
		v8::Handle<v8::Object> vr;

		METHOD_INSTANCE(object_base);
		METHOD_ENTER(0, 0);

		hr = pInst->ValueOf(vr);

		METHOD_RETURN();
	}

}

#endif

