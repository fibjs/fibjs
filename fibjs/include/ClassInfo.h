/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _fj_classinfo_H_
#define _fj_classinfo_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include <v8/v8.h>
#include <string>

namespace fibjs
{

struct ClassProperty
{
	const char* name;
	v8::AccessorGetter getter;
	v8::AccessorSetter setter;
};

struct ClassMethod
{
	const char* name;
	v8::InvocationCallback invoker;
};

struct ClassIndexed
{
	v8::IndexedPropertyGetter getter;
	v8::IndexedPropertySetter setter;
};

class ClassInfo;

struct ClassData
{
	const char* name;
	v8::InvocationCallback cor;
	int mc;
	const ClassMethod* cms;
	int pc;
	const ClassProperty* cps;
	const ClassIndexed* cis;
	ClassInfo *base;
};

class ClassInfo
{
public:
	ClassInfo(ClassData& cd) :
		m_cd(cd)
	{
		v8::HandleScope handle_scope;

		m_class = v8::Persistent<v8::FunctionTemplate>::New(
				v8::FunctionTemplate::New(cd.cor));

		m_class->SetClassName(v8::String::NewSymbol(cd.name));

		if (cd.base)
			m_class->Inherit(cd.base->m_class);

		v8::Local<v8::ObjectTemplate> pt = m_class->PrototypeTemplate();
		int i;

		pt->MarkAsUndetectable();

		for (i = 0; i < cd.mc; i++)
			pt->Set(cd.cms[i].name,
					v8::FunctionTemplate::New(cd.cms[i].invoker));

		for (i = 0; i < cd.pc; i++)
			if (cd.cps[i].setter)
				pt->SetAccessor(v8::String::NewSymbol(cd.cps[i].name),
						cd.cps[i].getter, cd.cps[i].setter);
			else
				pt->SetAccessor(v8::String::NewSymbol(cd.cps[i].name),
						cd.cps[i].getter, block_set);

		v8::Local<v8::ObjectTemplate> ot = m_class->InstanceTemplate();

		ot->SetInternalFieldCount(1);
		if (cd.cis)
			ot->SetIndexedPropertyHandler(cd.cis->getter, cd.cis->setter);

		m_cache = v8::Persistent<v8::Object>::New(
				m_class->GetFunction()->NewInstance());
	}

	void* getInstance(v8::Handle<v8::Value> o)
	{
		if (o.IsEmpty() || !o->IsObject() || !m_class->HasInstance(o))
			return NULL;

		return o->ToObject()->GetPointerFromInternalField(0);
	}

	v8::Handle<v8::Object> CreateInstance()
	{
		return m_cache->Clone();
	}

	v8::Handle<v8::FunctionTemplate> FunctionTemplate() const
	{
		return m_class;
	}

	const char* name()
	{
		return m_cd.name;
	}

	void Attach(v8::Handle<v8::Object> o)
	{
		int i;

		for (i = 0; i < m_cd.mc; i++)
			o->Set(
					v8::String::NewSymbol(m_cd.cms[i].name),
					v8::FunctionTemplate::New(m_cd.cms[i].invoker)->GetFunction());

		for (i = 0; i < m_cd.pc; i++)
			if (m_cd.cps[i].setter)
				o->SetAccessor(v8::String::NewSymbol(m_cd.cps[i].name),
						m_cd.cps[i].getter, m_cd.cps[i].setter);
			else
				o->SetAccessor(v8::String::NewSymbol(m_cd.cps[i].name),
						m_cd.cps[i].getter, block_set);
	}

protected:
	static void block_set(v8::Local<v8::String> property,
			v8::Local<v8::Value> value, const v8::AccessorInfo &info)
	{
		std::string strError = "Property \'";

		strError += *v8::String::Utf8Value(property);
		strError += "\' is read-only.";
		ThrowException(
				v8::String::New(strError.c_str(), (int) strError.length()));
	}

private:
	v8::Persistent<v8::FunctionTemplate> m_class;
	v8::Persistent<v8::Object> m_cache;
	ClassData& m_cd;
};

}

#endif

