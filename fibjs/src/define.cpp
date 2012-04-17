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
#include <map>
#include <sstream>

namespace fibjs
{

void InstallModule(std::string fname, v8::Handle<v8::Value> o);

inline std::string resolvePath(std::string base, const char* id)
{
	std::string fname;

	if (id[0] == '.' && (isPathSlash(id[1]) || (id[1] == '.' && isPathSlash(
			id[2]))))
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

	// cache string
	v8::Handle<v8::String> strRequire = v8::String::NewSymbol("require");
	v8::Handle<v8::String> strExports = v8::String::NewSymbol("exports");
	v8::Handle<v8::String> strModule = v8::String::NewSymbol("module");
	v8::Handle<v8::String> strDefs = v8::String::NewSymbol("defs");
	v8::Handle<v8::String> strId = v8::String::NewSymbol("id");

	// fetch default module object
	v8::Handle<v8::Object> mod = glob->Get(strModule)->ToObject();

	// fetch module.id
	std::string path;
	path_base::dirname(*v8::String::Utf8Value(mod->Get(strId)), path);

	v8::Handle<v8::Array> defs;
	{
		v8::Handle<v8::Value> v;

		// fetch hidden value module.defs
		v = mod->GetHiddenValue(strDefs);
		if (!v.IsEmpty() && v->IsArray())
			defs = v8::Handle<v8::Array>::Cast(v);
		else
		{
			// create one if not exists.
			defs = v8::Array::New(0);
			mod->SetHiddenValue(strDefs, defs);
		}
	}

	v8::Handle<v8::Array> deps;
	std::string id;

	int n = argc - 1, i;

	// check deps array
	if ((n > 0) && (args[n - 1]->IsArray()))
	{
		v8::Handle<v8::Array> a = v8::Handle<v8::Array>::Cast(args[n - 1]);
		int an = a->Length();

		deps = v8::Array::New(an);

		// copy deps array to module.deps
		for (i = 0; i < an; i++)
		{
			v8::String::Utf8Value s(a->Get(i));
			deps->Set(i, v8::String::New(resolvePath(path, *s).c_str()));
		}
		n--;
	}
	else
	{
		deps = v8::Array::New(3);

		// default deps: ['require', 'exports', 'module']
		deps->Set(0, strRequire);
		deps->Set(1, strExports);
		deps->Set(2, strModule);
	}

	if (n > 1)
		return ThrowResult(CALL_E_INVALIDARG);
	else if (n == 1)
	{
		// we have an id name
		v8::String::Utf8Value s(args[0]);
		id = *s;
	}

	v8::Handle<v8::Object> modDef;

	if (id.empty())
	{
		// anonymous module attach default module
		modDef = mod;
	}
	else
	{
		// named module
		id = resolvePath(path, id.c_str());

		modDef = v8::Object::New();
		v8::Handle<v8::Object> exports = v8::Object::New();

		// init module properties
		modDef->Set(strExports, exports);
		modDef->Set(strRequire, glob->Get(strRequire), v8::ReadOnly);

		v8::Handle<v8::String> strFname = v8::String::New(id.c_str(),
				id.length());
		modDef->Set(strId, strFname, v8::ReadOnly);

		// add to modules
		InstallModule(id, exports);
	}

	v8::Handle<v8::StackTrace> stackTrace = v8::StackTrace::CurrentStackTrace(
			1, v8::StackTrace::kOverview);
	std::stringstream strBuffer;

	v8::Local<v8::StackFrame> f = stackTrace->GetFrame(0);

	v8::String::Utf8Value funname(f->GetFunctionName());
	v8::String::Utf8Value filename(f->GetScriptName());

	strBuffer << "\n    at ";

	if (**funname)
		strBuffer << *funname << " (";

	strBuffer << *filename << ':' << f->GetLineNumber() << ':'
			<< f->GetColumn();

	if (**funname)
		strBuffer << ')';
	std::string strStack = strBuffer.str();

	// set hidden value module.deps and module.factory
	modDef->SetHiddenValue(v8::String::NewSymbol("deps"), deps);
	modDef->SetHiddenValue(v8::String::NewSymbol("factory"), args[argc - 1]);
	modDef->SetHiddenValue(v8::String::NewSymbol("stack"),
			v8::String::New(strStack.c_str(), strStack.length()));

	// append to define array
	defs->Set(defs->Length(), modDef);

	return v8::Undefined();
}

void doDefine(v8::Handle<v8::Object>& mod)
{
	v8::Handle<v8::Array> defs;
	{
		v8::Handle<v8::Value> v;
		v8::Handle<v8::String> strDefs = v8::String::NewSymbol("defs");

		// get define array from default module
		v = mod->GetHiddenValue(strDefs);
		mod->DeleteHiddenValue(strDefs);

		if (!v.IsEmpty() && v->IsArray())
			defs = v8::Handle<v8::Array>::Cast(v);
		else
			return;
	}

	int an = defs->Length(), i, j;
	std::vector<v8::Handle<v8::Object> > mods;
	std::vector<std::string> modIds;
	std::set<std::string> depns;

	// cache string
	v8::Handle<v8::String> strRequire = v8::String::NewSymbol("require");
	v8::Handle<v8::String> strExports = v8::String::NewSymbol("exports");
	v8::Handle<v8::String> strDeps = v8::String::NewSymbol("deps");
	v8::Handle<v8::String> strFactory = v8::String::NewSymbol("factory");
	v8::Handle<v8::String> strId = v8::String::NewSymbol("id");

	// copy data to local
	mods.resize(an);
	modIds.resize(an);
	for (i = 0; i < an; i++)
	{
		mods[i] = defs->Get(i)->ToObject();
		modIds[i] = *v8::String::Utf8Value(mods[i]->Get(strId));
		depns.insert(modIds[i]);
	}

	// two step
	int doStep = 2;
	bool bNext = false;

	while (doStep)
	{
		bNext = true;

		for (i = 0; i < an; i++)
		{
			if (!modIds[i].empty())
			{
				// get deps array
				v8::Handle<v8::Value> a1 = mods[i]->GetHiddenValue(strDeps);

				if (a1.IsEmpty() || !a1->IsArray())
				{
					// not found deps array
					depns.erase(modIds[i]);
					modIds[i].resize(0);
					bNext = false;

					continue;
				}

				v8::Handle<v8::Array> a = v8::Handle<v8::Array>::Cast(a1);

				int n = a->Length();

				// check if the module depend a module defined in same script
				for (j = 0; j < n; j++)
					if (doStep == 2 && depns.find(
							*v8::String::Utf8Value(a->Get(j))) != depns.end())
						break;

				if (j == n)
				{
					std::vector<v8::Handle<v8::Value> > deps;
					deps.resize(n);

					for (j = 0; j < n; j++)
					{
						v8::String::Utf8Value id(a->Get(j));

						if (!qstrcmp(*id, "require"))
							deps[j] = mods[i]->Get(strRequire);
						else if (!qstrcmp(*id, "exports"))
							deps[j] = mods[i]->Get(strExports);
						else if (!qstrcmp(*id, "module"))
							deps[j] = mods[i];
						else
						{
							// load module use require
							result_t hr;
							v8::TryCatch try_catch;

							hr = global_base::require(*id, deps[j]);

							if (hr < 0)
							{
								std::string str = getResultMessage(hr);

								str.append(
										*v8::String::Utf8Value(
												mods[i]->GetHiddenValue(
														v8::String::NewSymbol(
																"stack"))));
								ThrowError(str.c_str());
								return;
							}
							else if (hr == 0)
								return;
							else if (try_catch.HasCaught())
							{
								try_catch.ReThrow();
								return;
							}
						}
					}

					v8::Handle<v8::Value> v;

					// get factory and remove hidden value.
					v = mods[i]->GetHiddenValue(strFactory);
					mods[i]->DeleteHiddenValue(strFactory);
					mods[i]->DeleteHiddenValue(strDeps);

					if (v->IsFunction())
					{
						v8::Handle<v8::Function> func =
								v8::Handle<v8::Function>::Cast(v);

						v8::TryCatch try_catch;
						v = func->Call(func, n, deps.data());
						if (try_catch.HasCaught())
						{
							try_catch.ReThrow();
							return;
						}
					}

					// use the result as exports if the factory return something
					if (!v->IsUndefined())
						mods[i]->Set(strExports, v);
					else
						v = mods[i]->Get(strExports);

					InstallModule(modIds[i], v);

					// remove id name, we don't like to call it again
					depns.erase(modIds[i]);
					modIds[i].resize(0);
					bNext = false;
				}
			}
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
