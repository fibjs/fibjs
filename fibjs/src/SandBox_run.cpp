/*
 * SandBox_run.cpp
 *
 *  Created on: Oct 22, 2012
 *      Author: lion
 */

#include "SandBox.h"

#include "ifs/vm.h"
#include "ifs/fs.h"
#include "ifs/path.h"

#include "ifs/Stat.h"
#include "ifs/global.h"
#include "ifs/Function.h"

#include <sstream>

namespace fibjs
{

inline const char* ToCString(const v8::String::Utf8Value& value)
{
	return *value ? *value : "<string conversion failed>";
}

inline result_t throwSyntaxError(v8::TryCatch& try_catch)
{
	v8::String::Utf8Value exception(try_catch.Exception());

	v8::Handle<v8::Message> message = try_catch.Message();
	if (message.IsEmpty())
		ThrowError(ToCString(exception));
	else
	{
		std::stringstream strError;

		v8::String::Utf8Value filename(message->GetScriptResourceName());

		if (qstrcmp(ToCString(exception), "SyntaxError: ", 13))
			strError << ToCString(exception) << "\n    at ";
		else
			strError << (ToCString(exception) + 13) << "\n    at ";

		strError << ToCString(filename);
		int lineNumber = message->GetLineNumber();
		if (lineNumber > 0)
			strError << ':' << lineNumber << ':'
					<< (message->GetStartColumn() + 1);

		return Runtime::setError(strError.str());
	}

	return CALL_E_JAVASCRIPT;
}

inline std::string resolvePath(const char* id)
{
	std::string fname;

	if (id[0] == '.'
			&& (isPathSlash(id[1]) || (id[1] == '.' && isPathSlash(id[2]))))
	{
		v8::Handle<v8::Context> ctx = v8::Context::GetCalling();

		if (!ctx.IsEmpty())
		{
			v8::Handle<v8::Value> path = ctx->Global()->GetHiddenValue(
					v8::String::NewSymbol("id"));

			if (!path.IsEmpty())
			{
				std::string strPath;

				path_base::dirname(*v8::String::Utf8Value(path), strPath);
				if (strPath.length())
					strPath += '/';
				strPath += id;
				path_base::normalize(strPath.c_str(), fname);

				return fname;
			}
		}
	}

	path_base::normalize(id, fname);

	return fname;
}

inline result_t compileScript(const char* fname, std::string& buf,
		v8::Handle<v8::Script>& script)
{
	v8::TryCatch try_catch;

	script = v8::Script::Compile(
			v8::String::New(buf.c_str(), (int) buf.length()),
			v8::String::New(fname));
	if (script.IsEmpty())
		return throwSyntaxError(try_catch);

	return 0;
}

v8::Handle<v8::Value> _define(const v8::Arguments& args);
result_t doDefine(v8::Handle<v8::Object>& mod);

result_t SandBox::runScript(const char* id, v8::Handle<v8::Value>& retVal,
		bool bMod)
{
	std::string fname = resolvePath(id);
	std::map<std::string, obj_ptr<mod> >::iterator it;
	date_t now;

	// remove .js ext name if exists
	if (fname.length() > 3 && !qstrcmp(&fname[fname.length() - 3], ".js"))
		fname.resize(fname.length() - 3);

	if (bMod)
	{
		now.now();

		it = m_mods.find(fname);

		if (it != m_mods.end()
				&& (it->second->m_mtime.empty()
						|| now.diff(it->second->m_check) < 1000))
		{
			retVal = it->second->m_mod;
			return 1;
		}
	}

	// append .js ext name
	fname += ".js";

	result_t hr;
	const char* pname = fname.c_str();
	obj_ptr < Stat_base > st;
	date_t mtime;

	if (bMod)
	{
		hr = fs_base::ac_stat(pname, st);
		if (hr < 0)
			return hr;

		st->get_mtime(mtime);

		if (it != m_mods.end())
		{
			if (mtime.diff(it->second->m_mtime) == 0)
			{
				it->second->m_check = now;
				retVal = it->second->m_mod;
				return 0;
			}
		}
	}

	std::string buf;
	hr = fs_base::ac_readFile(pname, buf);
	if (hr < 0)
		return hr;

	v8::HandleScope handle_scope;

	v8::Persistent < v8::Context > context = v8::Context::New();
	v8::Context::Scope context_scope(context);

	v8::Handle < v8::Script > script;
	hr = compileScript(fname.c_str(), buf, script);
	if (hr < 0)
	{
		context.Dispose();
		return hr;
	}

	v8::Handle < v8::Object > glob = context->Global();
	v8::Handle < v8::Object > mod;
	v8::Handle < v8::Object > exports;

	glob->SetHiddenValue(v8::String::NewSymbol("SandBox"), wrap());

	// cache string
	v8::Handle < v8::String > strRequire = v8::String::NewSymbol("require");
	v8::Handle < v8::String > strExports = v8::String::NewSymbol("exports");
	v8::Handle < v8::String > strModule = v8::String::NewSymbol("module");
	v8::Handle < v8::String > strDefine = v8::String::NewSymbol("define");
	v8::Handle < v8::String > strId = v8::String::NewSymbol("id");

	// attach define function first.
	if (bMod)
	{
		v8::Handle < v8::Function > def =
				v8::FunctionTemplate::New(_define)->GetFunction();

		def->ToObject()->Set(v8::String::NewSymbol("amd"), v8::Object::New(),
				v8::ReadOnly);
		glob->Set(strDefine, def, v8::ReadOnly);
	}

	// clone global function
	fibjs::global_base::class_info().Attach(glob);

	// clone Function.start
	fibjs::Function_base::class_info().Attach(
			glob->Get(v8::String::NewSymbol("Function"))->ToObject()->GetPrototype()->ToObject());

	// module.id
	fname.resize(fname.length() - 3);
	v8::Handle < v8::String > strFname = v8::String::New(fname.c_str(),
			(int) fname.length());
	glob->SetHiddenValue(strId, strFname);

	if (bMod)
	{
		exports = v8::Object::New();

		// module and exports object
		if (mod.IsEmpty())
			mod = v8::Object::New();

		// init module
		mod->Set(strExports, exports);
		mod->Set(strRequire, glob->Get(strRequire), v8::ReadOnly);

		mod->Set(strId, strFname, v8::ReadOnly);

		// add to modules
		InstallModule(fname, exports, mtime, now);

		// attach to global
		glob->Set(strModule, mod, v8::ReadOnly);
		glob->Set(strExports, exports, v8::ReadOnly);
	}
	else
		// remove define function
		glob->ForceDelete(strDefine);

	if (script->Run().IsEmpty())
	{
		if (bMod)
		{
			// delete from modules
			m_mods.erase(fname);
		}

		context.Dispose();
		return CALL_E_JAVASCRIPT;
	}

	if (bMod)
	{
		// remove commonjs function
		glob->ForceDelete(strDefine);
		glob->ForceDelete(strModule);
		glob->ForceDelete(strExports);

		// process defined modules
		hr = doDefine(mod);
		if (hr < 0)
		{
			// delete from modules
			m_mods.erase(fname);
			context.Dispose();

			return hr;
		}

		// attach again
		glob->Set(strModule, mod, v8::ReadOnly);
		glob->Set(strExports, exports, v8::ReadOnly);

		// use module.exports as result value
		v8::Handle < v8::Value > v = mod->Get(strExports);
		InstallModule(fname, v, mtime, now);

		retVal = handle_scope.Close(v);
	}

	context.Dispose();
	return 0;
}

result_t SandBox::run(const char* fname)
{
	v8::Handle < v8::Value > retTemp;
	return runScript(fname, retTemp, false);
}

result_t SandBox::require(const char* id, v8::Handle<v8::Value>& retVal)
{
	return runScript(id, retVal, true);
}

}

