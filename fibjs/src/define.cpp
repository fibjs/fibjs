/*
 * define.cpp
 *
 *  Created on: Apr 11, 2012
 *      Author: lion
 */

#include "ifs/global.h"
#include "ifs/path.h"
#include <vector>
#include <set>

namespace fibjs
{

extern v8::Persistent<v8::Object> s_Modules;

inline std::string resolvePath(std::string base, const char* id)
{
	std::string fname;

	if (id[0] == '.'
			&& (isPathSlash(id[1]) || (id[1] == '.' && isPathSlash(id[2]))))
	{
		if (base.length())
			base += '/';
		base += id;
		path_base::normalize(base.c_str(), fname);

		return fname;
	}

	path_base::normalize(id, fname);

	return fname;
}

v8::Handle<v8::Value> _define(const v8::Arguments& args)
{
	int argc = args.Length();
	if (argc == 0)
		return ThrowResult(CALL_E_PARAMNOTOPTIONAL);
	if (argc > 3)
		return ThrowResult(CALL_E_BADPARAMCOUNT);

	v8::HandleScope handle_scope;

	v8::Handle<v8::Object> glob = v8::Context::GetCurrent()->Global();

	v8::Handle<v8::Object> mod =
			glob->Get(v8::String::NewSymbol("module"))->ToObject();

	std::string path;
	path_base::dirname(
			*v8::String::Utf8Value(mod->Get(v8::String::NewSymbol("id"))),
			path);

	v8::Handle<v8::Array> defs;
	{
		v8::Handle<v8::Value> v;

		v = mod->GetHiddenValue(v8::String::NewSymbol("defs"));
		if (!v.IsEmpty() && v->IsArray())
			defs = v8::Handle<v8::Array>::Cast(v);
		else
		{
			defs = v8::Array::New(0);
			mod->SetHiddenValue(v8::String::NewSymbol("defs"), defs);
		}
	}

	v8::Handle<v8::Array> deps;
	std::string id;

	int n = argc - 1, i;

	if ((n > 0) && (args[n - 1]->IsArray()))
	{
		v8::Handle<v8::Array> a = v8::Handle<v8::Array>::Cast(args[n - 1]);
		int an = a->Length();

		deps = v8::Array::New(an);

		for (i = 0; i < an; i++)
		{
			v8::Handle<v8::Value> v = a->Get(i);
			v8::String::Utf8Value s(v);

			if (*s == NULL)
				return ThrowResult(CALL_E_INVALIDARG);

			deps->Set(i, v8::String::New(resolvePath(path, *s).c_str()));
		}
		n--;
	}
	else
	{
		deps = v8::Array::New(3);

		deps->Set(0, v8::String::NewSymbol("require"));
		deps->Set(1, v8::String::NewSymbol("exports"));
		deps->Set(2, v8::String::NewSymbol("module"));
	}

	if (n > 1)
		return ThrowResult(CALL_E_INVALIDARG);
	else if (n == 1)
	{
		v8::String::Utf8Value s(args[0]);

		if (*s == NULL)
			return ThrowResult(CALL_E_INVALIDARG);

		id = *s;
	}

	v8::Handle<v8::Object> modDef;

	if (id.empty())
		modDef = mod;
	else
	{
		id = resolvePath(path, id.c_str());

		modDef = v8::Object::New();
		v8::Handle<v8::Object> exports = v8::Object::New();

		modDef->Set(v8::String::NewSymbol("exports"), exports);
		modDef->Set(v8::String::NewSymbol("require"),
				glob->Get(v8::String::NewSymbol("require")), v8::ReadOnly);

		v8::Handle<v8::String> strFname = v8::String::New(id.c_str());
		modDef->Set(v8::String::NewSymbol("id"), strFname, v8::ReadOnly);

		s_Modules->Set(strFname, mod, v8::ReadOnly);
	}

	modDef->SetHiddenValue(v8::String::NewSymbol("deps"), deps);
	modDef->SetHiddenValue(v8::String::NewSymbol("factory"), args[argc - 1]);

	defs->Set(defs->Length(), modDef);

	return v8::Undefined();
}

void doDefine(v8::Handle<v8::Object>& mod)
{
	v8::Handle<v8::Array> defs;
	{
		v8::Handle<v8::Value> v;

		v = mod->GetHiddenValue(v8::String::NewSymbol("defs"));
		mod->DeleteHiddenValue(v8::String::NewSymbol("defs"));

		if (!v.IsEmpty() && v->IsArray())
			defs = v8::Handle<v8::Array>::Cast(v);
		else
			return;
	}

	int an = defs->Length(), i, j;
	std::vector<v8::Handle<v8::Object> > mods;
	std::vector<std::string> modIds;
	std::set<std::string> depns;

	mods.resize(an);
	modIds.resize(an);
	for (i = 0; i < an; i++)
	{
		mods[i] = defs->Get(i)->ToObject();
		modIds[i] = *v8::String::Utf8Value(
				mods[i]->Get(v8::String::NewSymbol("id")));
		depns.insert(modIds[i]);
	}

	int doStep = 2;
	bool bNext = false;

	while (doStep)
	{
		bNext = true;

		for (i = 0; i < an; i++)
			if (!modIds[i].empty())
			{
				v8::Handle<v8::Value> a1 = mods[i]->GetHiddenValue(
						v8::String::NewSymbol("deps"));

				if (a1.IsEmpty() || !a1->IsArray())
				{
					depns.erase(modIds[i]);
					modIds[i].resize(0);

					continue;
				}

				v8::Handle<v8::Array> a = v8::Handle<v8::Array>::Cast(a1);
				mods[i]->DeleteHiddenValue(v8::String::NewSymbol("deps"));

				int n = a->Length();
				std::vector<std::string> ids;

				ids.resize(n);

				for (j = 0; j < n; j++)
				{
					ids[j] = *v8::String::Utf8Value(a->Get(j));

					if (doStep == 2 && depns.find(ids[j]) != depns.end())
						break;
				}

				if (j == n)
				{
					std::vector<v8::Handle<v8::Value> > deps;
					deps.resize(n);

					for (j = 0; j < n; j++)
					{
						result_t hr;
						v8::TryCatch try_catch;

						hr = global_base::require(ids[j].c_str(), deps[j]);

						if(hr < 0)
						{
							ThrowResult(hr);
							return;
						}

						if (try_catch.HasCaught())
						{
							try_catch.ReThrow();
							return;
						}
					}

					v8::Handle<v8::Value> v;

					v = mods[i]->GetHiddenValue(
							v8::String::NewSymbol("factory"));
					mods[i]->DeleteHiddenValue(
							v8::String::NewSymbol("factory"));

					if (v->IsFunction())
					{
						v8::Handle<v8::Function> func =
								v8::Handle<v8::Function>::Cast(v);

						v8::TryCatch try_catch;
						v = func->Call(func, n, &deps[0]);
						if (try_catch.HasCaught())
						{
							try_catch.ReThrow();
							return;
						}
					}

					if (!v->IsUndefined())
						mods[i]->Set(v8::String::NewSymbol("exports"), v);

					depns.erase(modIds[i]);
					modIds[i].resize(0);
				}
				else
					bNext = false;
			}
		if (bNext)
			doStep--;
	}
}

result_t global_base::define(const char* id, v8::Handle<v8::Array> deps,
		v8::Handle<v8::Value> factory)
{
	return 0;
}

}
