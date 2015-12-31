#include "ifs/global.h"
#include "ifs/coroutine.h"
#include "ifs/vm.h"
#include "ifs/process.h"
#include "SandBox.h"

namespace fibjs
{

result_t global_base::GC()
{
	Isolate::current()->m_isolate->LowMemoryNotification();
	return 0;
}

result_t global_base::run(const char *fname, v8::Local<v8::Array> argv)
{
	return Isolate::current()->m_topSandbox->run(fname, argv);
}

result_t global_base::get_argv(v8::Local<v8::Array>& retVal)
{
	return process_base::get_argv(retVal);
}

result_t global_base::get___filename(std::string& retVal)
{
	return 0;
}

result_t global_base::get___dirname(std::string& retVal)
{
	return 0;
}

result_t global_base::get___sbname(std::string& retVal)
{
	return 0;
}

result_t global_base::require(const char *id, v8::Local<v8::Value> &retVal)
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
