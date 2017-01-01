#include "object.h"
#include "ifs/global.h"
#include "ifs/coroutine.h"
#include "ifs/vm.h"
#include "ifs/process.h"
#include "SandBox.h"
#include "Event.h"
#include <vector>

namespace fibjs
{

result_t global_base::get_global(v8::Local<v8::Object>& retVal)
{
	Isolate* isolate = Isolate::current();
	retVal = v8::Local<v8::Object>::New(isolate->m_isolate, isolate->m_global);
	return 0;
}

result_t global_base::GC()
{
	Isolate::current()->m_isolate->LowMemoryNotification();
	return 0;
}

static void sync_callback(const v8::FunctionCallbackInfo<v8::Value> &args)
{
	v8::Isolate* isolate = args.GetIsolate();
	v8::Local<v8::Object> _data = v8::Local<v8::Object>::Cast(args.Data());

	obj_ptr<Event_base> ev;
	ev = Event_base::getInstance(_data->Get(v8::String::NewFromUtf8(isolate, "_ev")));

	int32_t len = args.Length();

	if (len > 0)
		_data->Set(v8::String::NewFromUtf8(isolate, "_error"), args[0]);

	if (len > 1)
		_data->Set(v8::String::NewFromUtf8(isolate, "_result"), args[1]);

	ev->set();
}

static void sync_stub(const v8::FunctionCallbackInfo<v8::Value> &args)
{
	v8::Isolate* isolate = args.GetIsolate();
	obj_ptr<Event_base> ev = new Event();

	v8::Local<v8::Object> _data = v8::Object::New(isolate);

	_data->Set(v8::String::NewFromUtf8(isolate, "_ev"), ev->wrap());

	std::vector<v8::Local<v8::Value> > argv;

	int32_t len = args.Length();
	int32_t i;

	argv.resize(len + 1);
	for (i = 0; i < len; i++)
		argv[i] = args[i];

	argv[i] = createV8Function("sync_callback", isolate, sync_callback, _data);

	v8::Local<v8::Function> func = v8::Local<v8::Function>::Cast(args.Data());

	func->Call(args.This(), (int32_t) argv.size(), argv.data());

	ev->wait();

	v8::Local<v8::Value> error = _data->Get(v8::String::NewFromUtf8(isolate, "_error"));

	if (!error.IsEmpty() && !error->IsUndefined() && !error->IsNull())
	{
		args.GetReturnValue().Set(v8::Local<v8::Value>());
		isolate->ThrowException(error);
	} else
		args.GetReturnValue().Set(_data->Get(v8::String::NewFromUtf8(isolate, "_result")));
}

result_t global_base::sync(v8::Local<v8::Function> func, v8::Local<v8::Function>& retVal)
{
	retVal = createV8Function("require", Isolate::current()->m_isolate, sync_stub, func);
	return 0;
}

result_t global_base::run(exlib::string fname, v8::Local<v8::Array> argv)
{
	return Isolate::current()->m_topSandbox->run(fname, argv);
}

result_t global_base::get_argv(v8::Local<v8::Array>& retVal)
{
	return process_base::get_argv(retVal);
}

result_t global_base::get___filename(exlib::string& retVal)
{
	return 0;
}

result_t global_base::get___dirname(exlib::string& retVal)
{
	return 0;
}

result_t global_base::get___sbname(exlib::string& retVal)
{
	return 0;
}

result_t global_base::require(exlib::string id, v8::Local<v8::Value> &retVal)
{
	return Isolate::current()->m_topSandbox->require(id, retVal);
}

result_t global_base::repl(v8::Local<v8::Array> cmds)
{
	return Isolate::current()->m_topSandbox->repl(cmds);
}

result_t global_base::repl(Stream_base* out, v8::Local<v8::Array> cmds)
{
	return Isolate::current()->m_topSandbox->repl(cmds, out);
}

}
