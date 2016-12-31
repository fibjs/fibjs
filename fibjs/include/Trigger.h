/*
 * Event.h
 *
 *  Created on: Apr 8, 2012
 *      Author: lion
 */

#ifndef EVENT_H_
#define EVENT_H_

#include "ifs/Trigger.h"
#include "ifs/EventEmitter.h"
#include "ifs/events.h"

namespace fibjs
{

class Trigger: public Trigger_base
{
	EVENT_SUPPORT();
};

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
	                     result_t (JSTrigger::*fn)(exlib::string , v8::Local<v8::Function>, int32_t &),
	                     int32_t &retVal)
	{
		v8::Local<v8::Array> ks = m->GetPropertyNames();
		int32_t len = ks->Length();
		int32_t i;

		retVal = 0;
		for (i = 0; i < len; i++)
		{
			v8::Local<v8::Value> k = ks->Get(i);

			if (!k->IsNumber())
			{
				v8::Local<v8::Value> v = m->Get(k);

				if (v->IsFunction())
				{
					int32_t n = 0;
					(this->*fn)(*v8::String::Utf8Value(k),
					            v8::Local<v8::Function>::Cast(v), n);
					retVal += n;
				}
				else
					return CHECK_ERROR(CALL_E_BADVARTYPE);
			}
		}

		return 0;
	}

	result_t on(exlib::string ev, v8::Local<v8::Function> func, int32_t &retVal)
	{
		retVal = 0;

		exlib::string strKey = "_e_";
		strKey.append(ev);
		retVal += putFunction(GetHiddenList(strKey, true), func);

		strKey = "_e1_";
		strKey.append(ev);
		retVal -= removeFunction(GetHiddenList(strKey), func);

		return 0;
	}

	result_t on(v8::Local<v8::Object> map, int32_t &retVal)
	{
		return _map(map, &JSTrigger::on, retVal);
	}

	result_t once(exlib::string ev, v8::Local<v8::Function> func, int32_t &retVal)
	{
		retVal = 0;

		exlib::string strKey = "_e1_";
		strKey.append(ev);
		retVal += putFunction(GetHiddenList(strKey, true), func);

		strKey = "_e_";
		strKey.append(ev);
		retVal -= removeFunction(GetHiddenList(strKey), func);

		return 0;
	}

	result_t once(v8::Local<v8::Object> map, int32_t &retVal)
	{
		return _map(map, &JSTrigger::once, retVal);
	}

	result_t off(exlib::string ev, v8::Local<v8::Function> func, int32_t &retVal)
	{
		retVal = 0;

		exlib::string strKey = "_e_";
		strKey.append(ev);
		retVal += removeFunction(GetHiddenList(strKey), func);

		strKey = "_e1_";
		strKey.append(ev);
		retVal += removeFunction(GetHiddenList(strKey), func);

		return 0;
	}

	result_t off(exlib::string ev, int32_t &retVal)
	{
		retVal = 0;

		exlib::string strKey = "_e_";
		strKey.append(ev);

		v8::Local<v8::Array> esa = GetHiddenList(strKey, false, true);
		if (!esa.IsEmpty())
		{
			int32_t len = esa->Length();
			int32_t i;

			for (i = 0; i < len; i ++)
			{
				v8::Local<v8::Value> v = esa->Get(i);
				if (!v->IsUndefined())
					retVal ++;
			}
		}

		strKey = "_e1_";
		strKey.append(ev);

		esa = GetHiddenList(strKey, false, true);
		if (!esa.IsEmpty())
		{
			int32_t len = esa->Length();
			int32_t i;

			for (i = 0; i < len; i ++)
			{
				v8::Local<v8::Value> v = esa->Get(i);
				if (!v->IsUndefined())
					retVal ++;
			}
		}

		return 0;
	}

	result_t off(v8::Local<v8::Object> map, int32_t &retVal)
	{
		return _map(map, &JSTrigger::off, retVal);
	}


	result_t removeAllListeners(v8::Local<v8::Array> evs, int32_t& retVal)
	{
		int32_t len = evs->Length();
		int32_t i;
		result_t hr;

		retVal = 0;

		for (i = 0; i < len; i ++)
		{
			v8::Local<v8::Value> v = evs->Get(i);
			exlib::string key;
			int32_t n = 0;

			hr = GetArgumentValue(v, key, true);
			if (hr < 0)
				return hr;

			hr = off(key, n);
			if (hr < 0)
				return hr;

			retVal += n;
		}

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

	result_t fireTrigger(v8::Local<v8::Array> esa, v8::Local<v8::Value> *args, int32_t argCount)
	{
		if (esa.IsEmpty())
			return 0;

		int32_t len = esa->Length();
		int32_t i;
		result_t hr;
		QuickArray<obj_ptr<Fiber_base> > evs;

		for (i = 0; i < len; i ++)
		{
			v8::Local<v8::Value> func = esa->Get(i);
			if (func->IsFunction())
			{
				obj_ptr<Fiber_base> retVal;
				hr = JSFiber::New(v8::Local<v8::Function>::Cast(func), args, argCount, retVal);
				if (hr < 0)
					return hr;
				evs.append(retVal);
			}
		}

		for (i = 0; i < evs.size(); i ++)
			evs[i]->join();

		return 0;
	}

	result_t _trigger(exlib::string ev, v8::Local<v8::Value> *args,
	                  int32_t argCount)
	{
		result_t hr;
		exlib::string strKey = "_e_";
		strKey.append(ev);

		hr = fireTrigger(GetHiddenList(strKey), args, argCount);
		if (hr < 0)
			return hr;

		strKey = "_e1_";
		strKey.append(ev);

		hr = fireTrigger(GetHiddenList(strKey, false, true), args,
		                 argCount);
		if (hr < 0)
			return hr;

		return 0;
	}

	result_t trigger(exlib::string ev, const v8::FunctionCallbackInfo<v8::Value> &args)
	{
		std::vector< v8::Local<v8::Value> > _args;
		int32_t len = args.Length();

		_args.resize(len - 1);

		for (int32_t i = 1; i < len; i ++)
			_args[i - 1] = args[i];

		return _trigger(ev, _args.data(), (int32_t) _args.size());
	}

public:
	static void s_on(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		int32_t vr;
		JSTrigger t(args);

		METHOD_ENTER(2, 2);

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
		int32_t vr;
		JSTrigger t(args);

		METHOD_ENTER(2, 2);

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
		int32_t vr;
		JSTrigger t(args);

		METHOD_ENTER(2, 2);

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
		int32_t vr;
		JSTrigger t(args);

		METHOD_ENTER(1, 0);

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

		METHOD_ENTER(1, 1);

		ARG(exlib::string, 0);

		hr = t.listeners(v0, vr);

		METHOD_RETURN();
	}

	static void s_trigger(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		JSTrigger t(args);

		METHOD_ENTER(-1, 1);

		ARG(exlib::string, 0);

		hr = t.trigger(v0, args);

		METHOD_VOID();
	}

private:
	v8::Isolate* isolate;
	v8::Local<v8::Object> o;
};

class EventEmitter: public EventEmitter_base
{
	DECLARE_CLASS(EventEmitter);
	EVENT_SUPPORT();
};

inline ClassInfo& EventEmitter::class_info()
{
	static ClassData::ClassMethod s_method[] =
	{
		{"on", JSTrigger::s_on, false},
		{"addListener", JSTrigger::s_on, false},
		{"once", JSTrigger::s_once, false},
		{"off", JSTrigger::s_off, false},
		{"removeListener", JSTrigger::s_off, false},
		{"removeAllListeners", JSTrigger::s_removeAllListeners, false},
		{"setMaxListeners", JSTrigger::s_setMaxListeners, false},
		{"listeners", JSTrigger::s_listeners, false},
		{"trigger", JSTrigger::s_trigger, false},
		{"emit", JSTrigger::s_trigger, false}
	};

	static ClassData s_cd =
	{
		"EventEmitterEx", NULL, NULL,
		10, s_method, 0, NULL, 0, NULL, NULL, NULL,
		&EventEmitter_base::class_info()
	};

	static ClassInfo s_ci(s_cd);
	return s_ci;
}

class events : public events_base
{
	DECLARE_CLASS(events);
};

inline ClassInfo& events::class_info()
{
	static ClassData::ClassObject s_object[] =
	{
		{"EventEmitter", EventEmitter::class_info}
	};

	static ClassData s_cd =
	{
		"events", NULL, NULL,
		0, NULL, 1, s_object, 0, NULL, NULL, NULL,
		&events_base::class_info()
	};

	static ClassInfo s_ci(s_cd);
	return s_ci;
}

}

#endif /* EVENT_H_ */
