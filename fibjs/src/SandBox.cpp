/*
 * SandBox.cpp
 *
 *  Created on: Oct 22, 2012
 *      Author: lion
 */

#include "SandBox.h"
#include "ifs/vm.h"

namespace fibjs
{

result_t vm_base::create(obj_ptr<SandBox_base>& retVal)
{
	retVal = new SandBox();
	return 0;
}

result_t vm_base::create(v8::Handle<v8::Object> mods,
		obj_ptr<SandBox_base>& retVal)
{
	obj_ptr < SandBox_base > sbox = new SandBox();
	result_t hr = sbox->add(mods);
	if (hr < 0)
		return hr;

	retVal = sbox;

	return 0;
}

result_t vm_base::current(obj_ptr<SandBox_base>& retVal)
{
	v8::Handle < v8::Context > ctx = v8::Context::GetCalling();

	if (ctx.IsEmpty())
		return CALL_E_INVALID_CALL;

	v8::Handle < v8::Value > sbox = ctx->Global()->GetHiddenValue(
			v8::String::NewSymbol("SandBox"));

	if (sbox.IsEmpty())
		return CALL_E_INTERNAL;

	retVal = SandBox_base::getInstance(sbox);
	return retVal ? 0 : CALL_E_INTERNAL;
}

void SandBox::InstallModule(std::string fname, v8::Handle<v8::Value> o,
		date_t d, date_t now)
{
	std::map<std::string, obj_ptr<mod> >::iterator it = m_mods.find(fname);
	obj_ptr<mod> m;

	if (it == m_mods.end())
	{
		m = new mod();
		m_mods[fname] = m;
	}
	else
	{
		m = it->second;
		m->m_mod.Dispose();
	}

	m->m_mod = v8::Persistent < v8::Value > ::New(o);
	m->m_mtime = d;
	m->m_check = now;
}

result_t SandBox::add(const char* id, v8::Handle<v8::Value> mod)
{
	static date_t s_noneDate;
	date_t now;

	now.now();

	InstallModule(id, mod, s_noneDate, now);
	return 0;
}

result_t SandBox::add(v8::Handle<v8::Object> mods)
{
	v8::Handle < v8::Array > ks = mods->GetPropertyNames();
	int len = ks->Length();
	int i;

	for (i = 0; i < len; i++)
	{
		v8::Handle < v8::Value > k = ks->Get(i);
		add(*v8::String::Utf8Value(k), mods->Get(k));
	}

	return 0;
}

result_t SandBox::remove(const char* id)
{
	m_mods.erase(id);
	return 0;
}

} /* namespace fibjs */
