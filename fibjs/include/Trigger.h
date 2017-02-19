/*
 * Event.h
 *
 *  Created on: Apr 8, 2012
 *      Author: lion
 */

#ifndef EVENT_H_
#define EVENT_H_

#include "ifs/EventEmitter.h"

namespace fibjs
{

class JSTrigger
{
public:
	JSTrigger(v8::Isolate* _iso, v8::Local<v8::Object> _o) :
		isolate(_iso), o(_o)
	{}

	JSTrigger(object_base* _o) :
		isolate(_o->holder()->m_isolate), o(_o->wrap())
	{}

	JSTrigger(const v8::FunctionCallbackInfo<v8::Value>& args) :
		isolate(args.GetIsolate()), o(args.This())
	{}

private:
	v8::Local<v8::String> NewFromUtf8(exlib::string str)
	{
		return v8::String::NewFromUtf8(isolate, str.c_str(), v8::String::kNormalString, (int32_t)str.length());
	}

public:
	v8::Local<v8::Value> GetPrivate(exlib::string key)
	{
		return o->GetPrivate(o->CreationContext(),
		                     v8::Private::ForApi(isolate, NewFromUtf8(key)))
		       .ToLocalChecked();
	}

	void SetPrivate(exlib::string key, v8::Local<v8::Value> value)
	{
		o->SetPrivate(o->CreationContext(),
		              v8::Private::ForApi(isolate, NewFromUtf8(key)),
		              value);
	}

	void DeletePrivate(exlib::string key)
	{
		o->DeletePrivate(o->CreationContext(),
		                 v8::Private::ForApi(isolate, NewFromUtf8(key)));
	}

	v8::Local<v8::Array> GetHiddenList(exlib::string k, bool create = false,
	                                   bool autoDelete = false)
	{
		v8::Local<v8::Value> es = GetPrivate(k);
		v8::Local<v8::Array> esa;

		if (es->IsUndefined())
		{
			if (create)
			{
				esa = v8::Array::New(isolate);
				SetPrivate(k, esa);
			}
		}
		else
			esa = v8::Local<v8::Array>::Cast(es);

		if (autoDelete)
			DeletePrivate(k);

		return esa;
	}

	inline int32_t putFunction(v8::Local<v8::Array> esa, v8::Local<v8::Function> func)
	{
		int32_t len = esa->Length();
		int32_t i;
		int32_t append = len;

		for (i = 0; i < len; i ++)
		{
			v8::Local<v8::Value> v = esa->Get(i);
			if (append == len && v->IsUndefined())
				append = i;
			else if (v->Equals(func))
				return 0;
		}

		esa->Set(append, func);

		return 1;
	}

	inline int32_t removeFunction(v8::Local<v8::Array> esa, v8::Local<v8::Function> func)
	{
		if (esa.IsEmpty())
			return 0;

		int32_t len = esa->Length();
		int32_t i;

		for (i = 0; i < len; i ++)
		{
			v8::Local<v8::Value> v = esa->Get(i);
			if (v->Equals(func))
			{
				esa->Delete(i);
				return 1;
			}
		}

		return 0;
	}

	inline result_t _map(v8::Local<v8::Object> m,
	                     result_t (JSTrigger::*fn)(exlib::string , v8::Local<v8::Function>, v8::Local<v8::Object>&),
	                     v8::Local<v8::Object>& retVal)
	{
		v8::Local<v8::Array> ks = m->GetPropertyNames();
		int32_t len = ks->Length();
		int32_t i;

		for (i = 0; i < len; i++)
		{
			v8::Local<v8::Value> k = ks->Get(i);

			if (!k->IsNumber())
			{
				v8::Local<v8::Value> v = m->Get(k);

				if (v->IsFunction())
					(this->*fn)(*v8::String::Utf8Value(k),
					            v8::Local<v8::Function>::Cast(v), retVal);
				else
					return CHECK_ERROR(CALL_E_BADVARTYPE);
			}
		}

		retVal = o;
		return 0;
	}

	result_t on(exlib::string ev, v8::Local<v8::Function> func, v8::Local<v8::Object>& retVal)
	{
		exlib::string strKey = "_e_";
		strKey.append(ev);
		putFunction(GetHiddenList(strKey, true), func);

		strKey = "_e1_";
		strKey.append(ev);
		removeFunction(GetHiddenList(strKey), func);

		retVal = o;
		return 0;
	}

	result_t on(v8::Local<v8::Object> map, v8::Local<v8::Object>& retVal)
	{
		return _map(map, &JSTrigger::on, retVal);
	}

	result_t once(exlib::string ev, v8::Local<v8::Function> func, v8::Local<v8::Object>& retVal)
	{
		exlib::string strKey = "_e1_";
		strKey.append(ev);
		putFunction(GetHiddenList(strKey, true), func);

		strKey = "_e_";
		strKey.append(ev);
		removeFunction(GetHiddenList(strKey), func);

		retVal = o;
		return 0;
	}

	result_t once(v8::Local<v8::Object> map, v8::Local<v8::Object>& retVal)
	{
		return _map(map, &JSTrigger::once, retVal);
	}

	result_t off(exlib::string ev, v8::Local<v8::Function> func, v8::Local<v8::Object>& retVal)
	{
		exlib::string strKey = "_e_";
		strKey.append(ev);
		removeFunction(GetHiddenList(strKey), func);

		strKey = "_e1_";
		strKey.append(ev);
		removeFunction(GetHiddenList(strKey), func);

		retVal = o;
		return 0;
	}

	result_t off(exlib::string ev, v8::Local<v8::Object>& retVal)
	{
		exlib::string strKey = "_e_";
		strKey.append(ev);

		GetHiddenList(strKey, false, true);

		strKey = "_e1_";
		strKey.append(ev);

		GetHiddenList(strKey, false, true);

		retVal = o;
		return 0;
	}

	result_t off(v8::Local<v8::Object> map, v8::Local<v8::Object>& retVal)
	{
		return _map(map, &JSTrigger::off, retVal);
	}

	result_t removeAllListeners(v8::Local<v8::Array> evs, v8::Local<v8::Object>& retVal)
	{
		int32_t len = evs->Length();
		int32_t i;
		result_t hr;

		for (i = 0; i < len; i ++)
		{
			v8::Local<v8::Value> v = evs->Get(i);
			exlib::string key;

			hr = GetArgumentValue(v, key, true);
			if (hr < 0)
				return hr;

			hr = off(key, retVal);
			if (hr < 0)
				return hr;
		}

		retVal = o;
		return 0;
	}

	result_t listeners(exlib::string ev, v8::Local<v8::Array>& retVal)
	{
		int32_t n = 0;

		exlib::string strKey = "_e_";
		strKey.append(ev);

		retVal = v8::Array::New(isolate);

		v8::Local<v8::Array> esa = GetHiddenList(strKey);
		if (!esa.IsEmpty())
		{
			int32_t len = esa->Length();
			int32_t i;

			for (i = 0; i < len; i ++)
			{
				v8::Local<v8::Value> v = esa->Get(i);
				if (!v->IsUndefined())
					retVal->Set(n++, v);
			}
		}

		strKey = "_e1_";
		strKey.append(ev);

		esa = GetHiddenList(strKey);
		if (!esa.IsEmpty())
		{
			int32_t len = esa->Length();
			int32_t i;

			for (i = 0; i < len; i ++)
			{
				v8::Local<v8::Value> v = esa->Get(i);
				if (!v->IsUndefined())
					retVal->Set(n++, v);
			}
		}

		return 0;
	}

	result_t fireTrigger(v8::Local<v8::Array> esa, v8::Local<v8::Value> *args, int32_t argCount,
	                     QuickArray<obj_ptr<Fiber_base> >& evs)
	{
		if (esa.IsEmpty())
			return 0;

		int32_t len = esa->Length();
		int32_t i;
		result_t hr;

		for (i = 0; i < len; i ++)
		{
			v8::Local<v8::Value> func = esa->Get(i);
			if (func->IsFunction())
			{
				obj_ptr<Fiber_base> f;
				hr = JSFiber::New(o, v8::Local<v8::Function>::Cast(func), args, argCount, f);
				if (hr < 0)
					return hr;
				evs.append(f);
			}
		}

		return 0;
	}

	result_t _emit(exlib::string ev, v8::Local<v8::Value> *args,
	               int32_t argCount, bool& retVal)
	{
		result_t hr;
		retVal = false;
		QuickArray<obj_ptr<Fiber_base> > evs;

		exlib::string strKey = "_e_";
		strKey.append(ev);

		hr = fireTrigger(GetHiddenList(strKey), args, argCount, evs);
		if (hr < 0)
			return hr;

		strKey = "_e1_";
		strKey.append(ev);

		hr = fireTrigger(GetHiddenList(strKey, false, true),
		                 args, argCount, evs);
		if (hr < 0)
			return hr;

		if (evs.size() > 0)
		{
			int32_t i;
			for (i = 0; i < (int32_t)evs.size(); i ++)
				evs[i]->join();

			retVal = true;
		}

		return 0;
	}

	result_t emit(exlib::string ev, const v8::FunctionCallbackInfo<v8::Value> &args,
	              bool& retVal)
	{
		std::vector< v8::Local<v8::Value> > _args;
		int32_t len = args.Length();

		_args.resize(len - 1);

		for (int32_t i = 1; i < len; i ++)
			_args[i - 1] = args[i];

		return _emit(ev, _args.data(), (int32_t) _args.size(), retVal);
	}

public:
	static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		if (args.IsConstructCall())
		{
			if (EventEmitter_base::class_info().init_isolate())
				return;
			EventEmitter_base::__new(args);
		} else
		{
			v8::Local<v8::Object> o = args.This();

			if (!o.IsEmpty())
				EventEmitter_base::class_info().Attach(Isolate::current(), o);
		}
	}

	static void s_on(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		v8::Local<v8::Object> vr;
		JSTrigger t(args);

		METHOD_ENTER();

		METHOD_OVER(2, 2);

		ARG(exlib::string, 0);
		ARG(v8::Local<v8::Function>, 1);

		hr = t.on(v0, v1, vr);

		METHOD_OVER(1, 1);

		ARG(v8::Local<v8::Object>, 0);

		hr = t.on(v0, vr);

		METHOD_RETURN();
	}

	static void s_once(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		v8::Local<v8::Object> vr;
		JSTrigger t(args);

		METHOD_ENTER();

		METHOD_OVER(2, 2);

		ARG(exlib::string, 0);
		ARG(v8::Local<v8::Function>, 1);

		hr = t.once(v0, v1, vr);

		METHOD_OVER(1, 1);

		ARG(v8::Local<v8::Object>, 0);

		hr = t.once(v0, vr);

		METHOD_RETURN();
	}

	static void s_off(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		v8::Local<v8::Object> vr;
		JSTrigger t(args);

		METHOD_ENTER();

		METHOD_OVER(2, 2);

		ARG(exlib::string, 0);
		ARG(v8::Local<v8::Function>, 1);

		hr = t.off(v0, v1, vr);

		METHOD_OVER(1, 1);

		ARG(exlib::string, 0);

		hr = t.off(v0, vr);

		METHOD_OVER(1, 1);

		ARG(v8::Local<v8::Object>, 0);

		hr = t.off(v0, vr);

		METHOD_RETURN();
	}

	static void s_removeAllListeners(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		v8::Local<v8::Object> vr;
		JSTrigger t(args);

		METHOD_ENTER();

		METHOD_OVER(1, 0);

		OPT_ARG(v8::Local<v8::Array>, 0, v8::Array::New(isolate));

		hr = t.removeAllListeners(v0, vr);

		METHOD_RETURN();
	}

	static void s_setMaxListeners(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
	}

	static void s_listeners(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		v8::Local<v8::Array> vr;
		JSTrigger t(args);

		METHOD_ENTER();

		METHOD_OVER(1, 1);

		ARG(exlib::string, 0);

		hr = t.listeners(v0, vr);

		METHOD_RETURN();
	}

	static void s_emit(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		bool vr;
		JSTrigger t(args);

		METHOD_ENTER();

		METHOD_OVER(-1, 1);

		ARG(exlib::string, 0);

		hr = t.emit(v0, args, vr);

		METHOD_RETURN();
	}

private:
	v8::Isolate* isolate;
	v8::Local<v8::Object> o;
};

class EventEmitter: public EventEmitter_base
{
	EVENT_SUPPORT();
};

}

#endif /* EVENT_H_ */
