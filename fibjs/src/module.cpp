/*
 * require.cpp
 *
 *  Created on: Apr 25, 2012
 *      Author: lion
 */

#include "ifs/global.h"
#include "ifs/Function.h"

#include "ifs/assert.h"
#include "ifs/path.h"
#include "ifs/os.h"
#include "ifs/coroutine.h"
#include "ifs/encoding.h"

#include "ifs/io.h"
#include "ifs/net.h"
#include "ifs/zmq.h"

#include <sstream>
#include <map>

namespace fibjs
{

static std::map<std::string, v8::Persistent<v8::Value> > s_mapModules;

void InstallModule(std::string fname, v8::Handle<v8::Value> o)
{
	std::map<std::string, v8::Persistent<v8::Value> >::iterator it =
			s_mapModules.find(fname);

	if (it == s_mapModules.end())
		s_mapModules[fname] = v8::Persistent<v8::Value>::New(o);
	else if (!o->StrictEquals(it->second))
	{
		it->second.Dispose();
		it->second = v8::Persistent<v8::Value>::New(o);
	}
}

inline void InstallNativeModule(const char* fname, ClassInfo& ci)
{
	InstallModule(fname, ci.CreateInstance());
}

void initModule()
{
	InstallNativeModule("assert", assert_base::class_info());
	InstallNativeModule("path", path_base::class_info());

	InstallNativeModule("coroutine", coroutine_base::class_info());

	InstallNativeModule("io", io_base::class_info());
	InstallNativeModule("os", os_base::class_info());
	InstallNativeModule("net", net_base::class_info());
	InstallNativeModule("zmq", zmq_base::class_info());

	InstallNativeModule("encoding", encoding_base::class_info());
}

inline const char* ToCString(const v8::String::Utf8Value& value)
{
	return *value ? *value : "<string conversion failed>";
}

inline void throwSyntaxError(v8::TryCatch& try_catch)
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
			strError << ':' << lineNumber << ':' << (message->GetStartColumn()
					+ 1);
		v8::ThrowException(
				v8::Exception::SyntaxError(
						v8::String::New(strError.str().c_str())));
	}
}

inline std::string resolvePath(const char* id)
{
	std::string fname;

	if (id[0] == '.' && (isPathSlash(id[1]) || (id[1] == '.' && isPathSlash(
			id[2]))))
	{
		v8::Handle<v8::Value> path =
				v8::Context::GetCurrent()->Global()->GetHiddenValue(
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

	path_base::normalize(id, fname);

	return fname;
}

inline v8::Handle<v8::Script> compileScript(const char* fname, std::string& buf)
{
	static bool s_top = true;
	v8::Handle<v8::Script> script;

	v8::TryCatch try_catch;

	script = v8::Script::Compile(
			v8::String::New(buf.c_str(), (int) buf.length()),
			v8::String::New(fname));
	if (script.IsEmpty())
	{
		if (s_top)
			ReportException(&try_catch, false);
		else
			throwSyntaxError(try_catch);
	}
	else
		s_top = false;

	return script;
}

v8::Handle<v8::Value> _define(const v8::Arguments& args);
void doDefine(v8::Handle<v8::Object>& mod);

inline result_t runScript(const char* id, v8::Handle<v8::Value>& retVal,
		bool bMod)
{
	std::string fname = resolvePath(id);

	// remove .js ext name if exists
	if (fname.length() > 3 && !qstrcmp(&fname[fname.length() - 3], ".js"))
		fname.resize(fname.length() - 3);

	if (bMod)
	{
		std::map<std::string, v8::Persistent<v8::Value> >::iterator it =
				s_mapModules.find(fname);

		if (it != s_mapModules.end())
		{
			retVal = it->second;
			return 1;
		}
	}

	// append .js ext name
	fname += ".js";

	std::string buf;

	exlib::AsyncEvent ac;
	result_t hr = io_base::readFile(fname.c_str(), buf, &ac);
	if (hr < 0)
		return hr;

	v8::HandleScope handle_scope;

	v8::Persistent<v8::Context> context = v8::Context::New();
	v8::Context::Scope context_scope(context);

	v8::Handle<v8::Script> script = compileScript(fname.c_str(), buf);
	if (script.IsEmpty())
	{
		context.Dispose();
		return 0;
	}

	v8::Handle<v8::Object> glob = context->Global();
	v8::Handle<v8::Object> mod;
	v8::Handle<v8::Object> exports;

	// cache string
	v8::Handle<v8::String> strRequire = v8::String::NewSymbol("require");
	v8::Handle<v8::String> strExports = v8::String::NewSymbol("exports");
	v8::Handle<v8::String> strModule = v8::String::NewSymbol("module");
	v8::Handle<v8::String> strDefine = v8::String::NewSymbol("define");
	v8::Handle<v8::String> strId = v8::String::NewSymbol("id");

	// attach define function first.
	if (bMod)
	{
		v8::Handle<v8::Function> def =
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
	v8::Handle<v8::String> strFname = v8::String::New(fname.c_str(),
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
		InstallModule(fname, exports);

		// attach to global
		glob->Set(strModule, mod, v8::ReadOnly);
		glob->Set(strExports, exports, v8::ReadOnly);
	}
	else
		// remove define function
		glob->ForceDelete(strDefine);

	if (!script->Run().IsEmpty())
	{
		if (bMod)
		{
			// remove commonjs function
			glob->ForceDelete(strDefine);
			glob->ForceDelete(strModule);
			glob->ForceDelete(strExports);

			// process defined modules
			doDefine(mod);

			// attach again
			glob->Set(strModule, mod, v8::ReadOnly);
			glob->Set(strExports, exports, v8::ReadOnly);

			// use module.exports as result value
			v8::Handle<v8::Value> v = mod->Get(strExports);
			InstallModule(fname, v);

			retVal = handle_scope.Close(v);
		}
	}
	else
	{
		if (bMod)
		{
			// delete from modules
			s_mapModules.erase(fname);
		}
	}

	context.Dispose();

	return 1;
}

result_t global_base::run(const char* fname)
{
	v8::Handle<v8::Value> retTemp;
	return runScript(fname, retTemp, false);
}

result_t global_base::require(const char* id, v8::Handle<v8::Value>& retVal)
{
	return runScript(id, retVal, true);
}

}
