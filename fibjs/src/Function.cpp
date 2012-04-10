#include "ifs/Function.h"

namespace fibjs
{

result_t startJSFiber(v8::Handle<v8::Function> func, const v8::Arguments& args, int nArgStart, obj_ptr<Fiber_base>& retVal);

result_t Function_base::start(const v8::Arguments& args, obj_ptr<Fiber_base>& retVal)
{
    if (!args.This()->IsFunction())
        return CALL_E_NOTINSTANCE;

    return startJSFiber(v8::Handle<v8::Function>::Cast(args.This()), args, 0, retVal);
}

}
