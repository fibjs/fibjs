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

#ifdef _WIN32
#include <mmsystem.h>
inline int64_t Ticks()
{
	return timeGetTime(); // Convert to microseconds.
}

inline bool isSeparator(char ch)
{
	return ch == '/' || ch == '\\';
}

#else
#include <sys/time.h>

inline int64_t Ticks()
{
	struct timeval tv;
	if (gettimeofday(&tv, NULL) < 0)
		return 0;
	return (tv.tv_sec * 1000ll) + tv.tv_usec / 1000;
}

inline bool isSeparator(char ch)
{
	return ch == '/';
}

#endif

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

static void initGlobal(v8::Handle<v8::Object> global)
{
	fibjs::global_base::class_info().Attach(global);

	global->Set(v8::String::NewSymbol("Buffer"),
			fibjs::Buffer_base::class_info().GetFunction());

	global->Set(v8::String::NewSymbol("Event"),
			fibjs::Event_base::class_info().GetFunction());

	fibjs::Function_base::class_info().Attach(
			global->Get(v8::String::NewSymbol("Function"))->ToObject()->GetPrototype()->ToObject());
}

inline const char* ToCString(const v8::String::Utf8Value& value)
{
	return *value ? *value : "<string conversion failed>";
}

static void throwSyntaxError(v8::TryCatch& try_catch)
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

result_t global_base::run(const char* fname)
{
	std::string buf;
	static bool s_top = true;

	result_t hr = fs_base::readFile(fname, buf);
	if (hr < 0)
		return hr;

	v8::HandleScope handle_scope;

	v8::Persistent<v8::Context> context = v8::Context::New();
	v8::Context::Scope context_scope(context);

	v8::Handle<v8::Object> glob = context->Global();
	initGlobal(glob);

	v8::Handle<v8::Script> script;
	{
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
			context.Dispose();
			return 0;
		}
	}

	path_base::dirname(fname, buf);
	glob->SetHiddenValue(v8::String::NewSymbol("path"),
			v8::String::New(buf.c_str()));

	s_top = false;
	script->Run();

	context.Dispose();

	return 1;
}

v8::Persistent<v8::Object> s_Modules;

static void InstallNativeModule(const char* name, ClassInfo& ci)
{
	v8::Handle<v8::Object> mod = v8::Object::New();

	mod->Set(v8::String::NewSymbol("exports"), ci.CreateInstance());
	s_Modules->Set(v8::String::New(name), mod);
}

void initMdule()
{
	v8::HandleScope handle_scope;

	s_Modules = v8::Persistent<v8::Object>::New(v8::Object::New());

	InstallNativeModule("assert", assert_base::class_info());
	InstallNativeModule("path", path_base::class_info());

	InstallNativeModule("coroutine", coroutine_base::class_info());

	InstallNativeModule("fs", fs_base::class_info());
	InstallNativeModule("os", os_base::class_info());

	InstallNativeModule("encoding", encoding_base::class_info());
}

result_t global_base::require(const char* id, v8::Handle<v8::Value>& retVal)
{
	v8::HandleScope handle_scope;
	std::string fname;

	if (id[0] == '.'
			&& (isSeparator(id[1]) || (id[1] == '.' && isSeparator(id[2]))))
	{
		v8::Handle<v8::Value> path =
				v8::Context::GetCurrent()->Global()->GetHiddenValue(
						v8::String::NewSymbol("path"));

		if (!path.IsEmpty())
		{
			std::string strPath = *v8::String::Utf8Value(path);
			if (strPath.length())
				strPath += '/';
			strPath += id;
			path_base::normalize(strPath.c_str(), fname);
		}
		else
			path_base::normalize(id, fname);
	}
	else
		path_base::normalize(id, fname);

	{
		v8::Handle<v8::Value> mod = s_Modules->Get(
				v8::String::New(fname.c_str()));

		if (!mod.IsEmpty() && mod->IsObject())
		{
			retVal = mod->ToObject()->Get(v8::String::NewSymbol("exports"));
			return 0;
		}
	}

	{
		std::string buf;

		fname += ".js";
		result_t hr = fs_base::readFile(fname.c_str(), buf);
		if (hr < 0)
			return hr;

		v8::Persistent<v8::Context> context = v8::Context::New();
		v8::Context::Scope context_scope(context);

		v8::Handle<v8::Script> script;

		{
			v8::TryCatch try_catch;

			script = v8::Script::Compile(
					v8::String::New(buf.c_str(), (int) buf.length()),
					v8::String::New(fname.c_str()));
			if (script.IsEmpty())
			{
				throwSyntaxError(try_catch);
				context.Dispose();
				return 0;
			}
		}

		v8::Handle<v8::Object> glob = context->Global();
		initGlobal(glob);

		v8::Handle<v8::Object> mod = v8::Object::New();

		fname.resize(fname.length() - 3);
		glob->Set(v8::String::NewSymbol("module"), mod);

		v8::Handle<v8::Object> exports = v8::Object::New();
		mod->Set(v8::String::NewSymbol("exports"), exports);
		glob->Set(v8::String::NewSymbol("exports"), exports);

		path_base::dirname(fname.c_str(), buf);
		glob->SetHiddenValue(v8::String::NewSymbol("path"),
				v8::String::New(buf.c_str()));

		s_Modules->Set(v8::String::New(fname.c_str()), mod);

		script->Run();

		context.Dispose();

		retVal = mod->Get(v8::String::NewSymbol("exports"));
	}

	return 0;
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
