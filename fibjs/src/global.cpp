#include "ifs/global.h"
#include "ifs/Fiber.h"
#include "ifs/Function.h"

#include "ifs/Event.h"
#include "ifs/Buffer.h"
#include "ifs/assert.h"
#include "ifs/path.h"
#include "ifs/os.h"
#include "ifs/coroutine.h"
#include "ifs/encoding.h"
#include <sstream>

#ifdef SEEK_SET
#undef SEEK_SET
#undef SEEK_CUR
#undef SEEK_END
#endif

#include "ifs/fs.h"

namespace fibjs
{

result_t global_base::get_console(obj_ptr<console_base>& retVal)
{
	static obj_ptr<console_base> s_console;

	if (s_console == NULL)
		s_console = new console_base();

	retVal = s_console;

	return 0;
}

result_t global_base::print(const char* fmt, const v8::Arguments& args)
{
	return console_base::log(fmt, args);
}

v8::Persistent<v8::Object> s_Modules;

inline void InstallNativeModule(const char* name, ClassInfo& ci)
{
	v8::Handle<v8::Object> mod = v8::Object::New();

	mod->Set(v8::String::NewSymbol("exports"), ci.CreateInstance(),
			v8::ReadOnly);
	s_Modules->Set(v8::String::New(name), mod, v8::ReadOnly);
}

void initMdule()
{
	s_Modules = v8::Persistent<v8::Object>::New(v8::Object::New());

	InstallNativeModule("assert", assert_base::class_info());
	InstallNativeModule("path", path_base::class_info());

	InstallNativeModule("coroutine", coroutine_base::class_info());

	InstallNativeModule("fs", fs_base::class_info());
	InstallNativeModule("os", os_base::class_info());

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
			strError << ':' << lineNumber << ':'
					<< (message->GetStartColumn() + 1);
		v8::ThrowException(
				v8::Exception::SyntaxError(
						v8::String::New(strError.str().c_str())));
	}
}

inline std::string resolvePath(const char* id)
{
	std::string fname;

	if (id[0] == '.'
			&& (isPathSlash(id[1]) || (id[1] == '.' && isPathSlash(id[2]))))
	{
		v8::Handle<v8::Value> mod = v8::Context::GetCurrent()->Global()->Get(
				v8::String::NewSymbol("module"));

		if (!mod.IsEmpty() && mod->IsObject())
		{
			v8::Handle<v8::Value> path = mod->ToObject()->Get(
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

inline v8::Handle<v8::Object> initRuntime(v8::Handle<v8::Context> context,
		std::string fname)
{
	v8::Handle<v8::Object> glob = context->Global();

	// define first.
	v8::Handle<v8::Function> def =
			v8::FunctionTemplate::New(_define)->GetFunction();
	glob->Set(v8::String::NewSymbol("define"), def, v8::ReadOnly);
	def->ToObject()->Set(v8::String::NewSymbol("amd"), v8::Object::New(),
			v8::ReadOnly);

	// clone global function
	fibjs::global_base::class_info().Attach(glob);

	// basic class Buffer
	glob->Set(v8::String::NewSymbol("Buffer"),
			fibjs::Buffer_base::class_info().GetFunction(), v8::ReadOnly);

	// basic class Event
	glob->Set(v8::String::NewSymbol("Event"),
			fibjs::Event_base::class_info().GetFunction(), v8::ReadOnly);

	// clone Function.start
	fibjs::Function_base::class_info().Attach(
			glob->Get(v8::String::NewSymbol("Function"))->ToObject()->GetPrototype()->ToObject());

	// module and exports object
	v8::Handle<v8::Object> mod = v8::Object::New();
	v8::Handle<v8::Object> exports = v8::Object::New();
	mod->Set(v8::String::NewSymbol("exports"), exports);
	mod->Set(v8::String::NewSymbol("require"),
			glob->Get(v8::String::NewSymbol("require")), v8::ReadOnly);

	// attach to global
	glob->Set(v8::String::NewSymbol("module"), mod, v8::ReadOnly);
	glob->Set(v8::String::NewSymbol("exports"), exports, v8::ReadOnly);

	// module.id
	fname.resize(fname.length() - 3);
	v8::Handle<v8::String> strFname = v8::String::New(fname.c_str());
	mod->Set(v8::String::NewSymbol("id"), strFname, v8::ReadOnly);

	// the end, add to modules
	s_Modules->Set(strFname, mod, v8::ReadOnly);

	return mod;
}

extern void doDefine(v8::Handle<v8::Object>& mod);

inline result_t runScript(std::string& fname, v8::Handle<v8::Value>& retVal)
{
	std::string buf;

	result_t hr = fs_base::readFile(fname.c_str(), buf);
	if (hr < 0)
		return hr;

	v8::Persistent<v8::Context> context = v8::Context::New();
	v8::Context::Scope context_scope(context);

	v8::Handle<v8::Script> script = compileScript(fname.c_str(), buf);
	if (script.IsEmpty())
	{
		context.Dispose();
		return 0;
	}

	v8::Handle<v8::Object> mod = initRuntime(context, fname);

	retVal = script->Run();

	context.Dispose();

	doDefine(mod);
	retVal = mod->Get(v8::String::NewSymbol("exports"));

	return 1;
}

result_t global_base::run(const char* fname)
{
	std::string strname = resolvePath(fname);
	v8::Handle<v8::Value> retVal;

	return runScript(strname, retVal);
}

result_t global_base::require(const char* id, v8::Handle<v8::Value>& retVal)
{
	std::string fname = resolvePath(id);

	{
		v8::Handle<v8::Value> mod = s_Modules->Get(
				v8::String::New(fname.c_str()));

		if (!mod.IsEmpty() && mod->IsObject())
		{
			retVal = mod->ToObject()->Get(v8::String::NewSymbol("exports"));
			return 0;
		}
	}

	fname += ".js";
	return runScript(fname, retVal);
}

result_t global_base::sleep(int32_t ms)
{
	return coroutine_base::sleep(ms);
}

result_t global_base::GC()
{
	v8::V8::LowMemoryNotification();
	return 0;
}

}
