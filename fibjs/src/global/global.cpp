#include "ifs/global.h"
#include "ifs/coroutine.h"
#include "ifs/vm.h"
#include "ifs/process.h"
#include "SandBox.h"

namespace fibjs
{

result_t global_base::GC()
{
	Isolate::now()->m_isolate->LowMemoryNotification();
	return 0;
}

result_t global_base::run(const char *fname, v8::Local<v8::Array> argv)
{
	return Isolate::now()->m_topSandbox->run(fname, argv);
}

result_t global_base::get_argv(v8::Local<v8::Array>& retVal)
{
	return process_base::get_argv(retVal);
}

result_t global_base::require(const char *id, v8::Local<v8::Value> &retVal)
{
	return Isolate::now()->m_topSandbox->require(id, retVal);
}

result_t global_base::repl(v8::Local<v8::Array> cmds)
{
	return Isolate::now()->m_topSandbox->repl(cmds);
}

result_t global_base::repl(Stream_base* out, v8::Local<v8::Array> cmds)
{
	return Isolate::now()->m_topSandbox->repl(cmds, out);
}

}
