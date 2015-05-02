#include "ifs/global.h"
#include "ifs/coroutine.h"
#include "ifs/vm.h"
#include "SandBox.h"

namespace fibjs
{

result_t global_base::get_console(obj_ptr<console_base> &retVal)
{
	static obj_ptr<console_base> s_console;

	if (s_console == NULL)
		s_console = new console_base();

	retVal = s_console;

	return 0;
}

result_t global_base::GC()
{
	Isolate::now().isolate->LowMemoryNotification();
	return 0;
}

result_t global_base::run(const char *fname)
{
	return Isolate::now().s_topSandbox->run(fname);
}

result_t global_base::require(const char *id, v8::Local<v8::Value> &retVal)
{
	return Isolate::now().s_topSandbox->require(id, retVal);
}

}
