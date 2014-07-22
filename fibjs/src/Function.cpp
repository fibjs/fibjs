#include "ifs/Function.h"
#include "Fiber.h"

namespace fibjs
{

result_t Function_base::start(const v8::FunctionCallbackInfo<v8::Value> &args, obj_ptr<Fiber_base> &retVal)
{
    if (!args.This()->IsFunction())
        return CHECK_ERROR(CALL_E_NOTINSTANCE);

    return JSFiber::New(v8::Local<v8::Function>::Cast(args.This()), args, 0, retVal);
}

}
