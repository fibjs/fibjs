#include "ifs/global.h"
#include "ifs/coroutine.h"
#include "ifs/vm.h"
#include "SandBox.h"

namespace fibjs
{

result_t global_base::GC()
{
	Isolate::now()->m_isolate->LowMemoryNotification();
	return 0;
}

result_t global_base::run(const char *fname)
{
	return Isolate::now()->m_topSandbox->run(fname);
}

result_t global_base::require(const char *id, v8::Local<v8::Value> &retVal)
{
	return Isolate::now()->m_topSandbox->require(id, retVal);
}

result_t global_base::repl()
{
	return Isolate::now()->m_topSandbox->repl();
}

}
