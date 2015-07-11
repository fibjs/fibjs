#include "ifs/Function.h"
#include "ifs/console.h"
#include "Fiber.h"

namespace fibjs
{

result_t Function_base::start(const v8::FunctionCallbackInfo<v8::Value> &args, obj_ptr<Fiber_base> &retVal)
{
	if (!args.This()->IsFunction())
		return CHECK_ERROR(CALL_E_NOTINSTANCE);

	std::string strBuffer;

	strBuffer.append("Function.start has been deprecated. Please use coroutine.start instead.");
	strBuffer.append(traceInfo(10));

	asyncLog(console_base::_ERROR, strBuffer);

	return JSFiber::New(v8::Local<v8::Function>::Cast(args.This()), args, 0, retVal);
}

}
